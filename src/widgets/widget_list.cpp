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
#include <ranges>

namespace fw {

	Logger& operator<<(Logger& logger, const WidgetUpdateTarget& target) {
		return logger << target.toStr();
	}

	WidgetList::WidgetList(Application& application) : application(application) {
		root_widget = createEmptyWidget();
		root_widget->setFillColor(sf::Color::Transparent);
		root_widget->setClickThrough(false);
		root_widget->setClipChildren(true);
		root_widget->setName("root");
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
		for (PendingSetParent* op : pending_setparent) {
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

	void WidgetList::addPendingMove(Widget* widget, size_t index) {
		wAssert(!isLocked());
		DataPointer<PendingMove> uptr = make_data_pointer<PendingMove>(
			"PendingMove " + widget->getFullName() + " " + std::to_string(index), *this, widget, index
		);
		pending_move.add(std::move(uptr));
	}

	void WidgetList::addPendingDelete(Widget* widget, bool with_children) {
		wAssert(!isLocked());
		DataPointer<PendingDelete> uptr = make_data_pointer<PendingDelete>(
			"PendingDelete " + widget->getFullName() + " " + std::to_string(with_children), *this, widget, with_children
		);
		pending_delete.add(std::move(uptr));
	}

	void WidgetList::addPendingSetParent(Widget* widget, Widget* new_parent) {
		wAssert(!isLocked());
		DataPointer<PendingSetParent> uptr = make_data_pointer<PendingSetParent>(
			"PendingSetParent " + widget->getFullName() + " " + new_parent->getFullName(), *this, widget, new_parent
		);
		pending_setparent.add(std::move(uptr));
	}

	Widget* WidgetList::operator[](size_t index) const {
		return widgets[index];
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

	PendingSetParent::PendingSetParent(WidgetList& widget_list, Widget* widget, Widget* new_parent) : PendingOperation(widget_list) {
		this->widget = widget;
		this->new_parent = new_parent;
	}

	void PendingSetParent::execute() {
		wAssert(widget_list.widgets.contains(widget));
		wAssert(new_parent == nullptr || widget_list.widgets.contains(new_parent));
		widget->setParent(new_parent);
	}

}
