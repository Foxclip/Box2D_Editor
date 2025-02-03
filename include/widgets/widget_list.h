#pragma once

#include "widget.h"
#include "empty_widget.h"
#include "widget_update_queue.h"
#include "widget_render_queue.h"

namespace fw {

	const float DEBUG_RENDER_ORIGIN_SIZE = 10.0f;
	const float DEBUG_RENDER_MOUSE_SIZE = 11.0f;
	const int DEBUG_RENDER_MOUSE_TRACE_MAX_LENGTH = 10;
	const sf::Color DEBUG_RENDER_TRANSFORMED_BOUNDS_COLOR = sf::Color(0, 255, 0);
	const sf::Color DEBUG_RENDER_BOUNDS_COLOR = sf::Color(0, 128, 0);
	const sf::Color DEBUG_RENDER_POSITION_COLOR = sf::Color(255, 0, 0);
	const sf::Color DEBUG_RENDER_TRANSFORM_POSITION_COLOR = sf::Color(0, 0, 255);
	const sf::Color DEBUG_RENDER_MOUSE_POSITION_COLOR = sf::Color(255, 255, 255, 128);
	const sf::Color DEBUG_RENDER_MOUSE_LEFT_RECT_COLOR = sf::Color(255, 64, 0, 64);
	const sf::Color DEBUG_RENDER_MOUSE_RIGHT_RECT_COLOR = sf::Color(0, 255, 0, 64);
	const sf::Color DEBUG_RENDER_MOUSE_MIDDLE_RECT_COLOR = sf::Color(0, 200, 255, 64);
	const sf::Color DEBUG_RENDER_MOUSE_TRACE_COLOR = sf::Color(255, 255, 255);
	const sf::Color DEBUG_RENDER_FOCUSED_WIDGET_BOUNDS_COLOR = sf::Color(0, 200, 255);

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

	enum class PostActionStage {
		FIRST,
		MOVE,
		DUPLICATE,
		SET_PARENT,
		REMOVE,
		LAST,
	};
	const int STAGE_COUNT = static_cast<int>(PostActionStage::LAST) + 1;
	using PostActionFuncType = std::function<void(void)>;
	class PostAction {
	public:
		PostAction(WidgetList& widget_list, const PostActionFuncType& action);
		void execute() const;

	private:
		WidgetList& widget_list;
		PostActionFuncType action;
	};

	// Adding methods:
	// If method changes widgets, check this:
	// wAssert(!isLocked());
	class WidgetList {
	public:
		static bool debug_render;
		static bool debug_mouse;
		Event<sf::Keyboard::Key> OnKeyPressed;
		Event<sf::Keyboard::Key> OnKeyReleased;
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
		bool isLeftButtonPressed() const;
		bool isRightButtonPressed() const;
		bool isMiddleButtonPressed() const;
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
		PolygonWidget* createPolygonWidget(size_t vertex_count, float radius = 10.0f, float angle_offset = 0.0f);
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
		template<typename T>
		requires std::derived_from<T, Widget>
		T* duplicateWidget(T* widget, bool with_children = true);
		bool isLocked() const;
		void lock();
		void unlock();
		void processBeforeInput();
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
		void addPostAction(const PostActionFuncType& func, PostActionStage stage = PostActionStage::FIRST);
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
		std::vector<PostAction> post_actions[STAGE_COUNT];
		WidgetUpdateQueue update_queue = WidgetUpdateQueue(*this);
		WidgetRenderQueue render_queue = WidgetRenderQueue(*this);
		sf::VertexArray mouse_debug_trace = sf::VertexArray(sf::PrimitiveType::LineStrip);

		void removeWidget(Widget* widget, bool with_children);
	};

	template<typename T, typename... Args>
	requires std::derived_from<T, Widget>
	inline T* WidgetList::createWidget(Args&&... args) {
		wAssert(!isLocked());
		dp::DataPointerUnique<T> uptr = dp::make_data_pointer<T>("Widget", *this, args...);
		T* ptr = uptr.get();
		uptr.setName("Widget " + ptr->full_name);
		if (root_widget) {
			ptr->setParentSilent(root_widget);
		}
		widgets.add(std::move(uptr));
		return ptr;
	}

	template<typename T>
	requires std::derived_from<T, Widget>
	inline T* WidgetList::duplicateWidget(T* widget, bool with_children) {
		wAssert(!isLocked());
		std::string name = widget->getName() + " copy";
		dp::DataPointerUnique<T> uptr = dp::make_data_pointer<T>(name, *widget);
		uptr.setName(name);
		T* ptr = uptr.get();
		widgets.add(std::move(uptr));
		ptr->setParent(widget->parent);
		if (with_children) {
			for (size_t i = 0; i < widget->children.size(); i++) {
				Widget* child = widget->children[i];
				Widget* child_copy = child->clone(true);
				child_copy->setParent(ptr);
			}
		}
		return ptr;
	}

}
