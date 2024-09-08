#include "widgets/application.h"
#include "widgets/widget_list.h"
#include "widgets/rectangle_widget.h"
#include <ranges>

namespace fw {

	Logger& operator<<(Logger& logger, const WidgetUpdateTarget& target) {
		return logger << target.toStr();
	}

	WidgetList::WidgetList(Application& application) : application(application) {
		root_widget = createWidget<EmptyWidget>();
		root_widget->setFillColor(sf::Color::Transparent);
		root_widget->setClickThrough(false);
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

	Widget* WidgetList::getBlockingWidget() const {
		for (RenderQueueLayer layer : render_queue.get() | std::views::reverse) {
			for (Widget* widget : layer.widgets | std::views::reverse) {
				if (widget->isMouseOver() && !widget->isClickThrough()) {
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

	Widget* WidgetList::getLeftGestureSource() const {
		return application.getLeftGestureSource();
	}

	Widget* WidgetList::getRightGestureSource() const {
		return application.getRightGestureSource();
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
		root_widget->processGlobalLeftRelease(pos);
		bool blocked_discard;
		CompVector<Widget*> widgets = getWidgetsUnderCursor(true, blocked_discard);
		for (size_t i = 0; i < widgets.size(); i++) {
			Widget* widget = widgets[i];
			widget->processBlockableLeftRelease(pos);
		}
	}

	void WidgetList::processRightRelease(const sf::Vector2f pos) {
		wAssert(!isLocked());
		root_widget->processGlobalRightRelease(pos);
		bool blocked_discard;
		CompVector<Widget*> widgets = getWidgetsUnderCursor(true, blocked_discard);
		for (size_t i = 0; i < widgets.size(); i++) {
			Widget* widget = widgets[i];
			widget->processBlockableRightRelease(pos);
		}
	}

	void WidgetList::processMouseMove(const sf::Vector2f pos) {
		wAssert(!isLocked());
		root_widget->processMouseMove(pos);
	}

	void WidgetList::processScrollX(const sf::Vector2f pos, float delta) {
		wAssert(!isLocked());
		CompVector<Widget*> widgets = getWidgetsUnderCursor(true, click_blocked);
		for (size_t i = 0; i < widgets.size(); i++) {
			Widget* widget = widgets[i];
			widget->processScrollX(pos, delta);
		}
	}

	void WidgetList::processScrollY(const sf::Vector2f pos, float delta) {
		wAssert(!isLocked());
		CompVector<Widget*> widgets = getWidgetsUnderCursor(true, click_blocked);
		for (size_t i = 0; i < widgets.size(); i++) {
			Widget* widget = widgets[i];
			widget->processScrollY(pos, delta);
		}
	}

	void WidgetList::processMouse(const sf::Vector2f pos) {
		wAssert(!isLocked());
		root_widget->processMouse(pos);
	}

	void WidgetList::processWindowEvent(const sf::Event& event) {
		if (event.type == sf::Event::Resized) {
			for (RenderQueueLayer layer : render_queue.get()) {
				for (Widget* widget : layer.widgets) {
					widget->OnWindowResized(
						static_cast<unsigned int>(event.size.width),
						static_cast<unsigned int>(event.size.height)
					);
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
		DataPointer<PendingMove> uptr = make_data_pointer<PendingMove>(*this, widget, index);
		pending_move.add(std::move(uptr));
	}

	void WidgetList::addPendingDelete(Widget* widget, bool with_children) {
		wAssert(!isLocked());
		DataPointer<PendingDelete> uptr = make_data_pointer<PendingDelete>(*this, widget, with_children);
		pending_delete.add(std::move(uptr));
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
		widget->removeSocket(widget->getPosXTarget());
		widget->removeSocket(widget->getPosYTarget());
		widget->removeSocket(widget->getSizeXTarget());
		widget->removeSocket(widget->getSizeYTarget());
		widget->removeSocket(widget->getChildrenXTarget());
		widget->removeSocket(widget->getChildrenYTarget());
		parent->removeChild(widget);
		render_queue.remove(widget);
		widgets.remove(widget);
	}

	void WidgetList::processAfterInput() {
		for (PendingMove* op : pending_move) {
			op->execute();
		}
		for (PendingDelete* op : pending_delete) {
			op->execute();
		}
		pending_move.clear();
		pending_delete.clear();
	}

	void WidgetList::updateRenderQueue() {
		wAssert(!isLocked());
		render_queue.update();
	}

	void WidgetList::updateWidgets() {
		wAssert(!isLocked());
		root_widget->preUpdate();
		update_queue.update();
		const std::vector<std::vector<WidgetUpdateTarget*>>& queue = update_queue.get();
		for (size_t layer_i = 0; layer_i < queue.size(); layer_i++) {
			const std::vector<WidgetUpdateTarget*>& layer = queue[layer_i];
			for (size_t target_i = 0; target_i < layer.size(); target_i++) {
				WidgetUpdateTarget* target = layer[target_i];
				target->execute();
			}
		}
		if (print_update_queue) {
			for (size_t layer_i = 0; layer_i < queue.size(); layer_i++) {
				logger << "Layer " << layer_i << ":" << "\n";
				LoggerIndent layer_indent;
				const std::vector<WidgetUpdateTarget*>& layer = queue[layer_i];
				for (size_t target_i = 0; target_i < layer.size(); target_i++) {
					WidgetUpdateTarget* target = layer[target_i];
					logger << target_i << ": " << *target << "\n";
				}
			}
		}
		print_update_queue = false;
		root_widget->postUpdate();
	}

	void WidgetList::printUpdateQueue() {
		print_update_queue = true;
	}

	void WidgetList::render(sf::RenderTarget& target) {
		wAssert(isLocked());
		root_widget->unclipped_region.invalidate();
#ifndef NDEBUG
		for (size_t i = 0; i < widgets.size(); i++) {
			widgets[i]->updateVisibility();
		}
#endif
		const CompVector<RenderQueueLayer>& layers = render_queue.get();
		for (RenderQueueLayer layer : layers) {
			for (size_t widget_i = 0; widget_i < layer.widgets.size(); widget_i++) {
				Widget* widget = layer.widgets[widget_i];
				widget->render(target);
			}
		}
		if (debug_render) {
			root_widget->renderBounds(target, DEBUG_RENDER_BOUNDS_COLOR, true, false);
			root_widget->renderBounds(target, DEBUG_RENDER_TRANSFORMED_BOUNDS_COLOR, true, true);
			root_widget->renderOrigin(target);
		}
		if (focused_widget) {
			focused_widget->renderBounds(target, DEBUG_RENDER_FOCUSED_WIDGET_BOUNDS_COLOR, false, true);
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

	PendingOperation::PendingOperation(WidgetList& widget_list) : widget_list(widget_list) { }

	PendingMove::PendingMove(WidgetList& widget_list, Widget* widget, size_t index) : PendingOperation(widget_list) {
		this->widget = widget;
		this->index = index;
	}

	void PendingMove::execute() {
		wAssert(widget_list.widgets.contains(widget));
		wAssert(widget_list.widgets.contains(widget->getParent()));
		Widget* parent = widget->getParent();
		parent->moveChildToIndex(widget, index);
	}

	PendingDelete::PendingDelete(WidgetList& widget_list, Widget* widget, bool with_children) : PendingOperation(widget_list) {
		this->widget = widget;
		this->with_children = with_children;
	}

	void PendingDelete::execute() {
		wAssert(widget_list.widgets.contains(widget));
		widget_list.removeWidget(widget, with_children);
	}

}
