#include "widgets/application.h"
#include "widgets/widget_list.h"
#include "widgets/rectangle_widget.h"
#include <ranges>

namespace fw {

	WidgetList::WidgetList(Application& application) : application(application) {
		root_widget = createWidget<RectangleWidget>();
		root_widget->setRenderable(false);
		root_widget->setFillColor(sf::Color::Transparent);
		root_widget->setClipChildren(true);
		root_widget->setName("root");
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
		Widget* focused = nullptr;
		for (size_t i = 0; i < widgets.size(); i++) {
			Widget* widget = widgets[i];
			widget->processLeftPress(pos);
			if (!focused && widget->isFocusable()) {
				focused = widget;
			}
		}
		setFocusedWidget(focused);
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

	void WidgetList::updateRenderQueue() {
		wAssert(!isLocked());
		render_queue.update();
	}

	void WidgetList::updateWidgets() {
		wAssert(!isLocked());
		root_widget->update();
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
			if (!widget->isFocusable()) {
				return;
			}
			wAssert(widgets.contains(widget));
		}
		if (focused_widget) {
			logger << focused_widget->getFullName() << " lost focus\n";
			focused_widget->internalOnFocusLost();
			focused_widget->OnFocusLost();
		}
		focused_widget = widget;
		if (focused_widget) {
			logger << focused_widget->getFullName() << " is focused\n";
			focused_widget->internalOnFocused();
			focused_widget->OnFocused();
		}
	}

}
