#pragma once

#include "widget.h"
#include "empty_widget.h"
#include "widget_update_queue.h"
#include "widget_render_queue.h"

namespace fw {

	const float DEBUG_RENDER_ORIGIN_SIZE = 10.0f;
	const float DEBUG_RENDER_MOUSE_SIZE = 10.0f;
	const sf::Color DEBUG_RENDER_TRANSFORMED_BOUNDS_COLOR = sf::Color(0, 255, 0);
	const sf::Color DEBUG_RENDER_BOUNDS_COLOR = sf::Color(0, 128, 0);
	const sf::Color DEBUG_RENDER_POSITION_COLOR = sf::Color(255, 0, 0);
	const sf::Color DEBUG_RENDER_TRANSFORM_POSITION_COLOR = sf::Color(0, 0, 255);
	const sf::Color DEBUG_RENDER_MOUSE_POSITION_COLOR = sf::Color(255, 255, 255);
	const sf::Color DEBUG_RENDER_FOCUSED_WIDGET_BOUNDS_COLOR = sf::Color(0, 200, 255);

	class PendingOperation {
	public:
		PendingOperation(WidgetList& widget_list);
		virtual void execute() = 0;

	protected:
		WidgetList& widget_list;
	};

	class PendingMove : public PendingOperation {
	public:
		Widget* widget = nullptr;
		size_t index = 0;

		PendingMove(WidgetList& widget_list, Widget* widget, size_t index);
		void execute() override;

	};

	class PendingDelete : public PendingOperation {
	public:
		Widget* widget = nullptr;
		bool with_children = false;

		PendingDelete(WidgetList& widget_list, Widget* widget, bool with_children);
		void execute() override;

	};

	class PendingSetParent : public PendingOperation {
	public:
		Widget* widget = nullptr;
		Widget* new_parent = nullptr;
		bool keep_pos = false;
		ptrdiff_t move_to_index = -1;

		PendingSetParent(WidgetList& widget_list, Widget* widget, Widget* new_parent, bool keep_pos, ptrdiff_t move_to_index = -1);
		void execute() override;

	};

	class Application;
	class RectangleWidget;
	class TextWidget;
	class PolygonWidget;
	class ContainerWidget;
	class EmptyWidget;
	class TextBoxWidget;
	class CheckboxWidget;
	class DropdownWidget;
	class CanvasWidget;
	class WindowWidget;
	class ScrollAreaWidget;
	class TreeViewWidget;

	// Adding methods:
	// If method changes widgets, check this:
	// wAssert(!isLocked());
	class WidgetList {
	public:
		bool debug_render = false;
		bool debug_mouse = true;
		Event<> OnProcessAfterInput;

		WidgetList(Application& application);
		virtual ~WidgetList();
		Application& getApplication() const;
		size_t getSize() const;
		bool contains(const Widget* widget);
		bool isClickBlocked() const;
		bool isReleaseBlocked() const;
		bool isBeingDestroyed() const;
		const CompVector<Widget*>& getAllWidgets() const;
		CompVectorUptr<Widget>& getWidgetUptrs();
		Widget* getRootWidget() const;
		Widget* getFocusedWidget() const;
		Widget* getTopWidgetUnderCursor() const;
		Widget* getBlockingWidget() const;
		CompVector<Widget*> getWidgetsUnderCursor(bool can_block, bool& blocked) const;
		bool getCurrentCursorType(sf::Cursor::Type& result) const;
		Widget* getLeftGestureSource() const;
		Widget* getRightGestureSource() const;
		sf::Vector2f getMousePosf() const;
		sf::Vector2u getWindowSize() const;
		bool isLCtrlPressed() const;
		bool isLAltPressed() const;
		bool isLShiftPressed() const;
		Widget* find(const std::string& name) const;
		const fw::Font& getDefaultFont() const;
		void setDefaultFont(const fw::Font& font);
		template<typename T, typename... Args>
		requires std::derived_from<T, Widget>
		T* createWidget(Args&&... args);
		RectangleWidget* createRectangleWidget(float width, float height);
		RectangleWidget* createRectangleWidget(const sf::Vector2f& size);
		TextWidget* createTextWidget();
		PolygonWidget* createPolygonWidget(const std::vector<sf::Vector2f>& vertices);
		ContainerWidget* createContainerWidget(float width, float height);
		ContainerWidget* createContainerWidget(const sf::Vector2f& size);
		EmptyWidget* createEmptyWidget();
		TextBoxWidget* createTextBoxWidget();
		TextBoxWidget* createTextBoxWidget(float width, float height);
		TextBoxWidget* createTextBoxWidget(const sf::Vector2f& size);
		CheckboxWidget* createCheckboxWidget();
		DropdownWidget* createDropdownWidget();
		CanvasWidget* createCanvasWidget(float width, float height, unsigned int texture_width, unsigned int texture_height);
		CanvasWidget* createCanvasWidget(const sf::Vector2f& size, const sf::Vector2u& texture_size);
		WindowWidget* createWindowWidget(float width, float height);
		WindowWidget* createWindowWidget(const sf::Vector2f& size);
		ScrollAreaWidget* createScrollAreaWidget(float width, float height);
		ScrollAreaWidget* createScrollAreaWidget(const sf::Vector2f& size);
		TreeViewWidget* createTreeViewWidget(float width, float height);
		TreeViewWidget* createTreeViewWidget(const sf::Vector2f& size);
		bool isLocked() const;
		void lock();
		void unlock();
		void processLeftPress(const sf::Vector2f pos);
		void processRightPress(const sf::Vector2f pos);
		void processLeftRelease(const sf::Vector2f pos);
		void processRightRelease(const sf::Vector2f pos);
		void processMouseMove(const sf::Vector2f pos);
		void processScrollX(const sf::Vector2f pos, float delta);
		void processScrollY(const sf::Vector2f pos, float delta);
		void processMouse(const sf::Vector2f pos);
		void processWindowEvent(const sf::Event& event);
		void processKeyboardEvent(const sf::Event& event);
		void processAfterInput();
		void updateRenderQueue();
		void updateWidgets();
		void printUpdateQueue();
		void render(sf::RenderTarget& target);
		void reset(const sf::Vector2f& root_size, const sf::Vector2f& mouse_pos);
		void setFocusedWidget(Widget* widget);
		void addPendingMove(Widget* widget, size_t index);
		void addPendingDelete(Widget* widget, bool with_children);
		void addPendingSetParent(Widget* widget, Widget* new_parent, bool keep_pos = false, ptrdiff_t move_to_index = -1);
		Widget* operator[](size_t index) const;

	private:
		friend class Widget;
		friend class PendingMove;
		friend class PendingDelete;
		friend class PendingSetParent;
		Application& application;
		bool locked = false;
		bool click_blocked = false;
		bool release_blocked = false;
		fw::Font default_font;
		bool default_font_set = false;
		CompVectorUptr<Widget> widgets;
		EmptyWidget* root_widget = nullptr;
		Widget* focused_widget = nullptr;
		bool print_update_queue = false;
		bool is_being_destroyed = false;
		CompVectorUptr<PendingMove> pending_move;
		CompVectorUptr<PendingDelete> pending_delete;
		CompVectorUptr<PendingSetParent> pending_setparent;
		WidgetUpdateQueue update_queue = WidgetUpdateQueue(*this);
		WidgetRenderQueue render_queue = WidgetRenderQueue(*this);

		void removeWidget(Widget* widget, bool with_children);
	};

	template<typename T, typename... Args>
	requires std::derived_from<T, Widget>
	inline T* WidgetList::createWidget(Args&&... args) {
		wAssert(!isLocked());
		DataPointer<T> uptr = make_data_pointer<T>("Widget", *this, args...);
		Widget* widget = uptr.get();
		uptr.setName("Widget " + widget->full_name);
		T* ptr = uptr.get();
		if (root_widget) {
			ptr->setParentSilent(root_widget);
		}
		widgets.add(std::move(uptr));
		return ptr;
	}

}
