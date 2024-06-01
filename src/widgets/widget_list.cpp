#include "widgets/application.h"
#include "widgets/widget_list.h"
#include "widgets/rectangle_widget.h"
#include <ranges>

namespace fw {

	WidgetList::WidgetList(Application& application) : application(application) {
		root_widget = createWidget<EmptyWidget>();
		root_widget->setFillColor(sf::Color::Transparent);
		root_widget->setClipChildren(true);
		root_widget->setName("root");
	}

	Application& WidgetList::getApplication() const {
		return application;
	}

	size_t WidgetList::getSize() const {
		return widgets.size();
	}

	bool WidgetList::contains(const Widget* widget) {
		return widgets.contains(widget);
	}

	bool WidgetList::isClickBlocked() const {
		return click_blocked;
	}

	bool WidgetList::isReleaseBlocked() const {
		return release_blocked;
	}

	Widget* WidgetList::getRootWidget() const {
		return root_widget;
	}

	Widget* WidgetList::getFocusedWidget() const {
		return focused_widget;
	}

	Widget* WidgetList::getTopWidgetUnderCursor() const {
		for (RenderQueueLayer layer : render_queue.get() | std::views::reverse) {
			for (Widget* widget : layer.widgets | std::views::reverse) {
				if (widget->isMouseOver()) {
					return widget;
				}
			}
		}
		return nullptr;
	}

	CompVector<Widget*> WidgetList::getWidgetsUnderCursor(bool can_block, bool& blocked) const {
		blocked = false;
		CompVector<Widget*> result;
		for (RenderQueueLayer layer : render_queue.get() | std::views::reverse) {
			for (Widget* widget : layer.widgets | std::views::reverse) {
				if (widget->isMouseOver()) {
					result.add(widget);
					if (can_block && !widget->isClickThrough()) {
						blocked = true;
						break;
					}
				}
			}
			if (blocked) {
				break;
			}
		}
		return result;
	}

	bool WidgetList::getCurrentCursorType(sf::Cursor::Type& result) const {
		bool blocked;
		CompVector<Widget*> cursor_widgets = getWidgetsUnderCursor(true, blocked);
		sf::Cursor::Type cursor_type = sf::Cursor::Arrow;
		for (size_t i = 0; i < cursor_widgets.size(); i++) {
			if (cursor_widgets[i]->getForceCustomCursor()) {
				result = cursor_widgets[i]->GetCursorType();
				return true;
			}
		}
		return false;
	}

	sf::Vector2f WidgetList::getMousePosf() const {
		return application.getMousePosf();
	}

	sf::Vector2u WidgetList::getWindowSize() const {
		return application.getWindowSize();
	}

	bool WidgetList::isLCtrlPressed() const {
		return application.isLCtrlPressed();
	}

	bool WidgetList::isLAltPressed() const {
		return application.isLAltPressed();
	}

	bool WidgetList::isLShiftPressed() const {
		return application.isLShiftPressed();
	}

	Widget* WidgetList::find(const std::string& name) const {
		if (root_widget->getName() == name) {
			return root_widget;
		}
		return root_widget->find(name);
	}

	const fw::Font& WidgetList::getDefaultFont() const {
		return default_font;
	}

	void WidgetList::setDefaultFont(const fw::Font& font) {
		default_font = font;
	}

	bool WidgetList::isLocked() const {
		return locked;
	}

	void WidgetList::lock() {
		wAssert(!locked);
		locked = true;
	}

	void WidgetList::unlock() {
		wAssert(locked);
		locked = false;
	}

	void WidgetList::processLeftPress(const sf::Vector2f pos) {
		wAssert(!isLocked());
		CompVector<Widget*> widgets = getWidgetsUnderCursor(true, click_blocked);
		bool modal_focus = focused_widget && focused_widget->getFocusableType() == Widget::FocusableType::MODAL;
		if (modal_focus) {
			click_blocked = true;
		}
		CompVector<Widget*> widgets_for_processing;
		// setting focused widget
		Widget* new_focused = nullptr;
		for (size_t i = 0; i < widgets.size(); i++) {
			Widget* widget = widgets[i];
			// modal widgets do not allow to interact with other widgets right away
			if (modal_focus) {
				if (!widget->getParentChain().contains(focused_widget)) {
					continue;
				}
			}
			widgets_for_processing.add(widget);
			if (!new_focused && widget->getFocusableType() != Widget::FocusableType::NONE) {
				new_focused = widget;
			}
		}
		Widget* prev_focused = focused_widget;
		setFocusedWidget(new_focused);
		// processLeftPress can modify focus after it was set
		for (size_t i = 0; i < widgets_for_processing.size(); i++) {
			Widget* widget = widgets_for_processing[i];
			bool became_focused = widget == new_focused && widget != prev_focused;
			widget->processLeftPress(pos, became_focused);
		}
	}

	void WidgetList::processRightPress(const sf::Vector2f pos) {
		wAssert(!isLocked());
		CompVector<Widget*> widgets = getWidgetsUnderCursor(true, click_blocked);
		for (size_t i = 0; i < widgets.size(); i++) {
			Widget* widget = widgets[i];
			widget->processRightPress(pos);
		}
	}

	void WidgetList::processLeftRelease(const sf::Vector2f pos) {
		wAssert(!isLocked());
		root_widget->processLeftRelease(pos);
	}

	void WidgetList::processRightRelease(const sf::Vector2f pos) {
		wAssert(!isLocked());
		root_widget->processRightRelease(pos);
	}

	void WidgetList::processMouse(const sf::Vector2f pos) {
		wAssert(!isLocked());
		root_widget->processMouse(pos);
	}

	void WidgetList::processWindowEvent(const sf::Event& event) {
		if (event.type == sf::Event::Resized) {
			for (RenderQueueLayer layer : render_queue.get()) {
				for (Widget* widget : layer.widgets) {
					widget->OnWindowResized(event.size.width, event.size.height);
				}
			}
		}
	}

	void WidgetList::processKeyboardEvent(const sf::Event& event) {
		wAssert(!isLocked());
		if (focused_widget) {
			focused_widget->processKeyboardEvent(event);
		}
	}

	void WidgetList::addPendingMove(Widget* widget, size_t index) {
		wAssert(!isLocked());
		PendingMove move;
		move.widget = widget;
		move.index = index;
		pending_moves.push_back(move);
	}

	void WidgetList::processAfterInput() {
		for (const PendingMove& move : pending_moves) {
			Widget* parent = move.widget->getParent();
			parent->moveChildToIndex(move.widget, move.index);
		}
		pending_moves = std::vector<PendingMove>();
	}

	void WidgetList::updateRenderQueue() {
		wAssert(!isLocked());
		render_queue.update();
	}

	void WidgetList::updateWidgets() {
		wAssert(!isLocked());
		update_queue.update();
		const std::vector<std::vector<WidgetUpdateQueueEntry*>>& queue = update_queue.get();
		for (const auto& layer : queue) {
			for (const WidgetUpdateQueueEntry* entry : layer) {
				if (entry->update_type == WidgetUpdateType::NORMAL) {
					entry->widget->update();
				} else if (entry->update_type == WidgetUpdateType::POS_X) {
					entry->widget->updatePositionX();
				} else if (entry->update_type == WidgetUpdateType::POS_Y) {
					entry->widget->updatePositionY();
				} else if (entry->update_type == WidgetUpdateType::SIZE_X) {
					entry->widget->updateSizeX();
				} else if (entry->update_type == WidgetUpdateType::SIZE_Y) {
					entry->widget->updateSizeY();
				} else if (entry->update_type == WidgetUpdateType::CHILDREN_X) {
					entry->widget->updateChildrenX();
				} else if (entry->update_type == WidgetUpdateType::CHILDREN_Y) {
					entry->widget->updateChildrenY();
				}
			}
		}
	}

	void WidgetList::render(sf::RenderTarget& target) {
		wAssert(isLocked());
		root_widget->unclipped_region.invalidate();
#ifndef NDEBUG
		for (size_t i = 0; i < widgets.size(); i++) {
			widgets[i]->updateVisibility();
		}
#endif
		const std::set<RenderQueueLayer>& layers = render_queue.get();
		for (RenderQueueLayer layer : layers) {
			for (size_t widget_i = 0; widget_i < layer.widgets.size(); widget_i++) {
				Widget* widget = layer.widgets[widget_i];
				widget->render(target);
			}
		}
		if (debug_render) {
			root_widget->renderBounds(target, sf::Color::Green, true);
			root_widget->renderOrigin(target);
		}
		if (focused_widget) {
			focused_widget->renderBounds(target, focused_widget_bounds_color, false);
		}
	}

	void WidgetList::reset(const sf::Vector2f& root_size, const sf::Vector2f& mouse_pos) {
		wAssert(!isLocked());
		root_widget->setSize(root_size);
		root_widget->updateMouseState(mouse_pos);
		click_blocked = false;
		release_blocked = false;
	}

	void WidgetList::setFocusedWidget(Widget* widget) {
		wAssert(!isLocked());
		LoggerTag tag_set_focused_widget("setFocusedWidget");
		if (widget == focused_widget) {
			return;
		}
		if (widget) {
			if (widget->getFocusableType() == Widget::FocusableType::NONE) {
				return;
			}
			wAssert(widgets.contains(widget));
		}
		Widget* prev_focused = focused_widget;
		focused_widget = widget;
		if (prev_focused) {
			logger << prev_focused->getFullName() << " lost focus\n";
			prev_focused->internalOnFocusLost();
			prev_focused->OnFocusLost();
		}
		if (focused_widget) {
			logger << focused_widget->getFullName() << " is focused\n";
			focused_widget->internalOnFocused();
			focused_widget->OnFocused();
		}
	}

	void WidgetList::removeWidget(Widget* widget, bool with_children) {
		wAssert(!isLocked());
		wAssert(widget);
		wAssert(widget != root_widget);
		wAssert(widgets.contains(widget));
		Widget* parent = widget->getParent();
		wAssert(parent);
		CompVector<Widget*> children = widget->getChildren();
		for (size_t i = 0; i < children.size(); i++) {
			Widget* child = children[i];
			if (with_children) {
				removeWidget(child, true);
			} else {
				child->setParent(parent);
			}
		}
		parent->removeChild(widget);
		widgets.remove(widget);
	}

}
