#pragma once

#include "widget.h"
#include "empty_widget.h"
#include "widget_update_queue.h"
#include "widget_render_queue.h"

namespace fw {

	const float DEBUG_RENDER_ORIGIN_SIZE = 10.0f;
	const sf::Color DEBUG_RENDER_TRANSFORMED_BOUNDS_COLOR = sf::Color(0, 255, 0);
	const sf::Color DEBUG_RENDER_BOUNDS_COLOR = sf::Color(0, 128, 0);
	const sf::Color DEBUG_RENDER_POSITION_COLOR = sf::Color(255, 0, 0);
	const sf::Color DEBUG_RENDER_TRANSFORM_POSITION_COLOR = sf::Color(0, 0, 255);
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

	class Application;

	// Adding methods:
	// If method changes widgets, check this:
	// wAssert(!isLocked());
	class WidgetList {
	public:
		bool debug_render = false;

		WidgetList(Application& application);
		Application& getApplication() const;
		size_t getSize() const;
		bool contains(const Widget* widget);
		bool isClickBlocked() const;
		bool isReleaseBlocked() const;
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

	private:
		friend class Widget;
		friend class PendingMove;
		friend class PendingDelete;
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
		CompVectorUptr<PendingMove> pending_move;
		CompVectorUptr<PendingDelete> pending_delete;
		WidgetUpdateQueue update_queue = WidgetUpdateQueue(*this);
		WidgetRenderQueue render_queue = WidgetRenderQueue(*this);

		void addPendingMove(Widget* widget, size_t index);
		void addPendingDelete(Widget* widget, bool with_children);
		void removeWidget(Widget* widget, bool with_children);

	};

	template<typename T, typename... Args>
	requires std::derived_from<T, Widget>
	inline T* WidgetList::createWidget(Args&&... args) {
		wAssert(!isLocked());
		DataPointer<T> uptr = make_data_pointer<T>("Widget", *this, args...);
		T* ptr = uptr.get();
		if (root_widget) {
			ptr->setParentSilent(root_widget);
		}
		widgets.add(std::move(uptr));
		return ptr;
	}

}
