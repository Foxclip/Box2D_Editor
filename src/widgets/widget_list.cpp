#include "widgets/application.h"
#include "widgets/widget_list.h"
#include "widgets/rectangle_widget.h"
#include "widgets/text_widget.h"
#include "widgets/polygon_widget.h"
#include "widgets/container_widget.h"
#include "widgets/empty_widget.h"
#include "widgets/textbox_widget.h"
#include "widgets/checkbox_widget.h"
#include "widgets/dropdown_widget.h"
#include "widgets/canvas_widget.h"
#include "widgets/window_widget.h"
#include "widgets/scroll_area_widget.h"
#include "widgets/tree_view_widget.h"
#include "widgets/button_widget.h"
#include <ranges>

namespace fw {

	bool WidgetList::debug_render = false;
	bool WidgetList::debug_mouse = false;

	Logger& operator<<(Logger& logger, const WidgetUpdateTarget& target) {
		return logger << target.toStr();
	}

	WidgetList::WidgetList(Application& application) : application(application) {
		root_widget = createEmptyWidget();
		root_widget->setFillColor(sf::Color::Transparent);
		root_widget->setClickThrough(false);
		root_widget->setClipChildren(true);
		root_widget->setName("root");

		for (size_t i = 0; i < STAGE_COUNT; i++) {
			post_actions[i] = std::vector<PostAction>();
		}
	}

	WidgetList::~WidgetList() {
		is_being_destroyed = true;
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

	bool WidgetList::isBeingDestroyed() const {
		return is_being_destroyed;
	}

	const CompVector<Widget*>& WidgetList::getAllWidgets() const {
		return widgets.getCompVector();
	}

	CompVectorUptr<Widget>& WidgetList::getWidgetUptrs() {
		return widgets;
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

	bool WidgetList::isLeftButtonPressed() const {
		return application.isLeftButtonPressed();
	}

	bool WidgetList::isRightButtonPressed() const {
		return application.isRightButtonPressed();
	}

	bool WidgetList::isMiddleButtonPressed() const {
		return application.isMiddleButtonPressed();
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
		return root_widget->tryFind(name);
	}

	const fw::Font& WidgetList::getDefaultFont() const {
		return default_font;
	}

	void WidgetList::setDefaultFont(const fw::Font& font) {
		default_font = font;
	}

	RectangleWidget* WidgetList::createRectangleWidget(float width, float height) {
		RectangleWidget* rectangle_widget = createWidget<RectangleWidget>(width, height);
		return rectangle_widget;
	}

	RectangleWidget* WidgetList::createRectangleWidget(const sf::Vector2f& size) {
		RectangleWidget* rectangle_widget = createRectangleWidget(size.x, size.y);
		return rectangle_widget;
	}

	TextWidget* WidgetList::createTextWidget() {
		TextWidget* text_widget = createWidget<TextWidget>();
		return text_widget;
	}

	PolygonWidget* WidgetList::createPolygonWidget(const std::vector<sf::Vector2f>& vertices) {
		PolygonWidget* polygon_widget = createWidget<PolygonWidget>(vertices);
		return polygon_widget;
	}

	PolygonWidget* WidgetList::createPolygonWidget(size_t vertex_count, float radius, float angle_offset) {
		PolygonWidget* polygon_widget = createWidget<PolygonWidget>(vertex_count, radius, angle_offset);
		return polygon_widget;
	}

	ContainerWidget* WidgetList::createContainerWidget(float width, float height) {
		ContainerWidget* container_widget = createWidget<ContainerWidget>(width, height);
		return container_widget;
	}

	ContainerWidget* WidgetList::createContainerWidget(const sf::Vector2f& size) {
		ContainerWidget* container_widget = createContainerWidget(size.x, size.y);
		return container_widget;
	}

	EmptyWidget* WidgetList::createEmptyWidget() {
		EmptyWidget* empty_widget = createWidget<EmptyWidget>();
		return empty_widget;
	}

	TextBoxWidget* WidgetList::createTextBoxWidget() {
		TextBoxWidget* textbox_widget = createWidget<TextBoxWidget>();
		return textbox_widget;
	}

	TextBoxWidget* WidgetList::createTextBoxWidget(float width, float height) {
		TextBoxWidget* textbox_widget = createWidget<TextBoxWidget>(width, height);
		return textbox_widget;
	}

	TextBoxWidget* WidgetList::createTextBoxWidget(const sf::Vector2f& size) {
		TextBoxWidget* textbox_widget = createTextBoxWidget(size.x, size.y);
		return textbox_widget;
	}

	CheckboxWidget* WidgetList::createCheckboxWidget() {
		CheckboxWidget* checkbox_widget = createWidget<CheckboxWidget>();
		return checkbox_widget;
	}

	DropdownWidget* WidgetList::createDropdownWidget() {
		DropdownWidget* dropdown_widget = createWidget<DropdownWidget>();
		return dropdown_widget;
	}

	CanvasWidget* WidgetList::createCanvasWidget(float width, float height, unsigned int texture_width, unsigned int texture_height) {
		CanvasWidget* canvas_widget = createWidget<CanvasWidget>(width, height, texture_width, texture_height);
		return canvas_widget;
	}

	CanvasWidget* WidgetList::createCanvasWidget(const sf::Vector2f& size, const sf::Vector2u& texture_size) {
		CanvasWidget* canvas_widget = createCanvasWidget(size.x, size.y, texture_size.x, texture_size.y);
		return canvas_widget;
	}

	WindowWidget* WidgetList::createWindowWidget(float width, float height) {
		WindowWidget* window_widget = createWidget<WindowWidget>(width, height);
		return window_widget;
	}

	WindowWidget* WidgetList::createWindowWidget(const sf::Vector2f& size) {
		WindowWidget* window_widget = createWindowWidget(size.x, size.y);
		return window_widget;
	}

	ScrollAreaWidget* WidgetList::createScrollAreaWidget(float width, float height) {
		ScrollAreaWidget* scroll_area_widget = createWidget<ScrollAreaWidget>(width, height);
		return scroll_area_widget;
	}

	ScrollAreaWidget* WidgetList::createScrollAreaWidget(const sf::Vector2f& size) {
		ScrollAreaWidget* scroll_area_widget = createScrollAreaWidget(size.x, size.y);
		return scroll_area_widget;
	}

	TreeViewWidget* WidgetList::createTreeViewWidget(float width, float height) {
		TreeViewWidget* tree_view_widget = createWidget<TreeViewWidget>(width, height);
		return tree_view_widget;
	}

	TreeViewWidget* WidgetList::createTreeViewWidget(const sf::Vector2f& size) {
		TreeViewWidget* tree_view_widget = createTreeViewWidget(size.x, size.y);
		return tree_view_widget;
	}

	ButtonWidget* WidgetList::createButtonWidget(float width, float height) {
		ButtonWidget* button_widget = createWidget<ButtonWidget>(width, height);
		return button_widget;
	}

	ButtonWidget* WidgetList::createButtonWidget(const sf::Vector2f& size) {
		ButtonWidget* button_widget = createButtonWidget(size.x, size.y);
		return button_widget;
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

	void WidgetList::processBeforeInput() { }

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
		bool blocked_discard;
		CompVector<Widget*> widgets = getWidgetsUnderCursor(true, blocked_discard);
		for (size_t i = 0; i < widgets.size(); i++) {
			Widget* widget = widgets[i];
			widget->processBlockableLeftRelease(pos);
		}
		root_widget->processGlobalLeftRelease(pos);
	}

	void WidgetList::processRightRelease(const sf::Vector2f pos) {
		wAssert(!isLocked());
		bool blocked_discard;
		CompVector<Widget*> widgets = getWidgetsUnderCursor(true, blocked_discard);
		for (size_t i = 0; i < widgets.size(); i++) {
			Widget* widget = widgets[i];
			widget->processBlockableRightRelease(pos);
		}
		root_widget->processGlobalRightRelease(pos);
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
		if (event.type == sf::Event::KeyPressed) {
			OnKeyPressed(event.key.code);
		} else if (event.type == sf::Event::KeyReleased) {
			OnKeyReleased(event.key.code);
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
		for (size_t i = 0; i < STAGE_COUNT; i++) {
			for (const PostAction& action : post_actions[i]) {
				action.execute();
			}
		}
		for (size_t i = 0; i < STAGE_COUNT; i++) {
			post_actions[i].clear();
		}
		OnProcessAfterInput();
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
		if (TreeViewWidget::target_highlight.visible) {
			sf::Vector2f pos = TreeViewWidget::target_highlight.pos;
			sf::Vector2f size = TreeViewWidget::target_highlight.size;
			draw_rect(target, pos, size, TREEVIEW_TARGET_HIGHLIGHT_COLOR);
		}
		if (debug_render) {
			root_widget->renderBounds(target, DEBUG_RENDER_BOUNDS_COLOR, true, false);
			root_widget->renderBounds(target, DEBUG_RENDER_TRANSFORMED_BOUNDS_COLOR, true, true);
			root_widget->renderOrigin(target, true);
		} else {
			std::function<void(Widget*)> debug_render = [&](Widget* widget) {
				if (widget->debug_render) {
					widget->renderBounds(target, DEBUG_RENDER_BOUNDS_COLOR, false, false);
					widget->renderBounds(target, DEBUG_RENDER_TRANSFORMED_BOUNDS_COLOR, false, true);
					widget->renderOrigin(target, false);
				}
				for (Widget* child : widget->getChildren()) {
					debug_render(child);
				}
			};
			debug_render(root_widget);
		}
		if (focused_widget) {
			focused_widget->renderBounds(target, DEBUG_RENDER_FOCUSED_WIDGET_BOUNDS_COLOR, false, true);
		}
		if (debug_mouse) {
			if (mouse_debug_trace.getVertexCount() < DEBUG_RENDER_MOUSE_TRACE_MAX_LENGTH) {
				mouse_debug_trace.append(getMousePosf());
			} else {
				for (ptrdiff_t i = 0; i < (ptrdiff_t)mouse_debug_trace.getVertexCount() - 1; i++) {
					mouse_debug_trace[i] = mouse_debug_trace[i + 1];
				}
				mouse_debug_trace[mouse_debug_trace.getVertexCount() - 1] = getMousePosf();
			}
			for (size_t i = 0; i < mouse_debug_trace.getVertexCount(); i++) {
				float alpha = (float)i / ((float)mouse_debug_trace.getVertexCount() - 1);
				mouse_debug_trace[i].color = DEBUG_RENDER_MOUSE_TRACE_COLOR;
				mouse_debug_trace[i].color.a = (sf::Uint8)(alpha * 255.0f);
			}
			target.draw(mouse_debug_trace);

			float offset_outer = DEBUG_RENDER_MOUSE_SIZE;
			float offset_inner = 2.0f;
			float mouse_rect_offset_inner = 2.0f;
			float middle_mouse_rect_offset = 8.0f;
			sf::Vector2f hoffset_outer = sf::Vector2f(offset_outer, 0.0f);
			sf::Vector2f hoffset_inner = sf::Vector2f(offset_inner, 0.0f);
			sf::Vector2f voffset_outer = sf::Vector2f(0.0f, offset_outer);
			sf::Vector2f voffset_inner = sf::Vector2f(0.0f, offset_inner);
			sf::Vector2f mouse_pos = getMousePosf();
			if (isLeftButtonPressed()) {
				draw_rect(
					target,
					sf::Vector2f(mouse_pos.x - offset_outer, mouse_pos.y - offset_outer),
					sf::Vector2f(mouse_pos.x - offset_outer, mouse_pos.y + offset_outer + 1.0f),
					sf::Vector2f(mouse_pos.x - mouse_rect_offset_inner, mouse_pos.y - offset_outer),
					sf::Vector2f(mouse_pos.x - mouse_rect_offset_inner, mouse_pos.y + offset_outer + 1.0f),
					DEBUG_RENDER_MOUSE_LEFT_RECT_COLOR
				);
			}
			if (isRightButtonPressed()) {
				draw_rect(
					target,
					sf::Vector2f(mouse_pos.x + mouse_rect_offset_inner + 1.0f, mouse_pos.y - offset_outer),
					sf::Vector2f(mouse_pos.x + mouse_rect_offset_inner + 1.0f, mouse_pos.y + offset_outer + 1.0f),
					sf::Vector2f(mouse_pos.x + offset_outer + 1.0f, mouse_pos.y - offset_outer),
					sf::Vector2f(mouse_pos.x + offset_outer + 1.0f, mouse_pos.y + offset_outer + 1.0f),
					DEBUG_RENDER_MOUSE_RIGHT_RECT_COLOR
				);
			}
			if (isMiddleButtonPressed()) {
				draw_rect(
					target,
					sf::Vector2f(mouse_pos.x - middle_mouse_rect_offset, mouse_pos.y - middle_mouse_rect_offset),
					sf::Vector2f(mouse_pos.x - middle_mouse_rect_offset, mouse_pos.y + middle_mouse_rect_offset + 1.0f),
					sf::Vector2f(mouse_pos.x + middle_mouse_rect_offset + 1.0f, mouse_pos.y - middle_mouse_rect_offset),
					sf::Vector2f(mouse_pos.x + middle_mouse_rect_offset + 1.0f, mouse_pos.y + middle_mouse_rect_offset + 1.0f),
					DEBUG_RENDER_MOUSE_MIDDLE_RECT_COLOR
				);
			}
			draw_line(target, mouse_pos - hoffset_outer, mouse_pos - hoffset_inner, DEBUG_RENDER_MOUSE_POSITION_COLOR);
			draw_line(target, mouse_pos + hoffset_outer, mouse_pos + hoffset_inner, DEBUG_RENDER_MOUSE_POSITION_COLOR);
			draw_line(target, mouse_pos - voffset_outer, mouse_pos - voffset_inner, DEBUG_RENDER_MOUSE_POSITION_COLOR);
			draw_line(target, mouse_pos + voffset_outer, mouse_pos + voffset_inner, DEBUG_RENDER_MOUSE_POSITION_COLOR);
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

	void WidgetList::addPostAction(const PostActionFuncType& func, PostActionStage stage) {
		PostAction post_action(*this, func);
		int stage_index = static_cast<int>(stage);
		post_actions[stage_index].push_back(post_action);
	}

	Widget* WidgetList::operator[](size_t index) const {
		return widgets[index];
	}

	PostAction::PostAction(WidgetList& widget_list, const PostActionFuncType& action)
		: widget_list(widget_list), action(action) { }

	void PostAction::execute() const {
		action(widget_list);
	}

}
