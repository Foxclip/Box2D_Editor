#pragma once

#include "widget.h"
#include "rectangle_widget.h"

struct RenderQueueLayer {
	Widget::RenderLayer layer;
	CompoundVector<Widget*> widgets;
	RenderQueueLayer(Widget::RenderLayer layer);
	bool operator<(const RenderQueueLayer& other) const;
};

class WidgetList {
public:
	bool debug_render = false;

	WidgetList();
	bool contains(const Widget* widget);
	bool isClickBlocked() const;
	bool isReleaseBlocked() const;
	Widget* getRootWidget() const;
	Widget* getFocusedWidget() const;
	Widget* find(const std::string& name) const;
	template<typename T, typename... Args>
	requires std::derived_from<T, Widget>
	T* createWidget(Args&&... args);
	void processClick(const sf::Vector2f pos);
	void processRelease(const sf::Vector2f pos);
	void processMouse(const sf::Vector2f pos);
	void processKeyboardEvent(const sf::Event& event);
	void render(sf::RenderTarget& target);
	void reset(const sf::Vector2f& root_size);
	void setFocusedWidget(Widget* widget);

private:
	friend class Widget;
	bool click_blocked = false;
	bool release_blocked = false;
	float render_origin_size = 10.0f;
	sf::Color render_bounds_color = sf::Color::Green;
	sf::Color render_origin_color = sf::Color::Red;
	sf::Color focused_widget_bounds_color = sf::Color(0, 200, 255);
	CompoundVectorUptr<Widget> widgets;
	RectangleWidget* root_widget = nullptr;
	Widget* focused_widget_temp = nullptr;
	Widget* focused_widget = nullptr;
	std::set<RenderQueueLayer> render_queue;
	
	void updateRenderQueue();

};

template<typename T, typename... Args>
	requires std::derived_from<T, Widget>
inline T* WidgetList::createWidget(Args&&... args) {
	std::unique_ptr<T> uptr = std::make_unique<T>(*this, args...);
	T* ptr = uptr.get();
	if (root_widget) {
		ptr->setParentSilent(root_widget);
	}
	widgets.add(std::move(uptr));
	return ptr;
}
