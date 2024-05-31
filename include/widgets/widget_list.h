#pragma once

#include "widget.h"
#include "empty_widget.h"
#include "widget_update_queue.h"
#include "widget_render_queue.h"

namespace fw {

	// Adding methods:
	// If method changes widgets, check this:
	// wAssert(!isLocked());
	// And if if changes render queue:
	// render_queue.invalidate();

	class Application;

	class WidgetList {
	public:
		bool debug_render = false;

		WidgetList(Application& application);
		size_t getSize() const;
		bool contains(const Widget* widget);
		bool isClickBlocked() const;
		bool isReleaseBlocked() const;
		Widget* getRootWidget() const;
		Widget* getFocusedWidget() const;
		Widget* getTopWidgetUnderCursor() const;
		CompVector<Widget*> getWidgetsUnderCursor(bool can_block, bool& blocked) const;
		bool getCurrentCursorType(sf::Cursor::Type& result) const;
		sf::Vector2f getMousePosf() const;
		sf::Vector2u getWindowSize() const;
		bool isLCtrlPressed() const;
		bool isLAltPressed() const;
		bool isLShiftPressed() const;
		Widget* find(const std::string& name) const;
		const sf::Font* getDefaultFont() const;
		void setDefaultFont(const sf::Font& font);
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
		void processMouse(const sf::Vector2f pos);
		void processWindowEvent(const sf::Event& event);
		void processKeyboardEvent(const sf::Event& event);
		void updateRenderQueue();
		void updateWidgets();
		void render(sf::RenderTarget& target);
		void reset(const sf::Vector2f& root_size, const sf::Vector2f& mouse_pos);
		void setFocusedWidget(Widget* widget);
		void removeWidget(Widget* widget, bool with_children = true);

	private:
		friend class Widget;
		Application& application;
		bool locked = false;
		bool click_blocked = false;
		bool release_blocked = false;
		float render_origin_size = 10.0f;
		sf::Color render_bounds_color = sf::Color::Green;
		sf::Color render_origin_color = sf::Color::Red;
		sf::Color render_transform_origin_color = sf::Color::Blue;
		sf::Color focused_widget_bounds_color = sf::Color(0, 200, 255);
		sf::Font default_font;
		bool default_font_set = false;
		CompVectorUptr<Widget> widgets;
		EmptyWidget* root_widget = nullptr;
		Widget* focused_widget = nullptr;
		WidgetUpdateQueue update_queue = WidgetUpdateQueue(*this);
		WidgetRenderQueue render_queue = WidgetRenderQueue(*this);

	};

	template<typename T, typename... Args>
	requires std::derived_from<T, Widget>
	inline T* WidgetList::createWidget(Args&&... args) {
		wAssert(!isLocked());
		std::unique_ptr<T> uptr = std::make_unique<T>(*this, args...);
		T* ptr = uptr.get();
		if (root_widget) {
			ptr->setParentSilent(root_widget);
		}
		widgets.add(std::move(uptr));
		return ptr;
	}

}
