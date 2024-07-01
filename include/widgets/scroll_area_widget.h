#pragma once

#include "empty_widget.h"
#include "container_widget.h"

namespace fw {

	const float SCROLL_AREA_SCROLLBAR_DEFAULT_WIDTH = 20.0f;
	const float SCROLL_AREA_DEFAULT_SCROLL_X_DELTA = 40.0f;
	const float SCROLL_AREA_DEFAULT_SCROLL_Y_DELTA = 20.0f;
	const sf::Color SCROLL_AREA_DEFAULT_BACKGROUND_COLOR = sf::Color(100, 100, 100);
	const sf::Color SCROLL_AREA_DEFAULT_SCROLL_BACKGROUND_COLOR = sf::Color(128, 128, 128);
	const sf::Color SCROLL_AREA_DEFAULT_SLIDER_COLOR = sf::Color(150, 150, 150);
	const sf::Color SCROLL_AREA_DEFAULT_SLIDER_HOVER_COLOR = sf::Color(175, 175, 175);
	const sf::Color SCROLL_AREA_DEFAULT_SLIDER_PRESSED_COLOR = sf::Color(200, 200, 200);
	const sf::Color SCROLL_AREA_DEFAULT_CORNER_COLOR = sf::Color(140, 140, 140);

	class WidgetList;

	class ScrollAreaWidget : public EmptyWidget {
	public:
		enum class ScrollbarPolicy {
			OFF,
			SIZE,
			ON,
		};
		ScrollAreaWidget(WidgetList& widget_list, float width, float height);
		ScrollAreaWidget(WidgetList& widget_list, const sf::Vector2f& size);
		Widget* getScrolledWidget() const;
		Widget* getAreaWidget() const;
		Widget* getScrollbarXWidget() const;
		Widget* getScrollbarYWidget() const;
		Widget* getSliderXWidget() const;
		Widget* getSliderYWidget() const;
		Widget* getCornerWidget() const;
		void setScrolledWidget(Widget* widget);
		Widget* takeScrolledWidget();
		void setBackgroundColor(const sf::Color& color);
		void setScrollbarColor(const sf::Color& color);
		void setSliderColor(const sf::Color& color);
		void setDeltaX(float delta);
		void setDeltaY(float delta);
		void setScrollbarXPolicy(ScrollbarPolicy policy);
		void setScrollbarYPolicy(ScrollbarPolicy policy);

	protected:
		RectangleWidget* area_widget = nullptr;
		Widget* scrolled_widget = nullptr;
		RectangleWidget* scrollbar_x_widget = nullptr;
		RectangleWidget* slider_x_widget = nullptr;
		RectangleWidget* scrollbar_y_widget = nullptr;
		RectangleWidget* slider_y_widget = nullptr;
		RectangleWidget* corner_widget = nullptr;
		bool slider_x_hover = false;
		bool slider_y_hover = false;
		bool slider_x_grabbed = false;
		bool slider_y_grabbed = false;
		float mouse_grab_pos;
		float slider_grab_pos;
		float delta_x = SCROLL_AREA_DEFAULT_SCROLL_X_DELTA;
		float delta_y = SCROLL_AREA_DEFAULT_SCROLL_Y_DELTA;
		ScrollbarPolicy scrollbar_x_policy = ScrollbarPolicy::SIZE;
		ScrollbarPolicy scrollbar_y_policy = ScrollbarPolicy::SIZE;

		void internalPostUpdate() override;
		void internalOnScrollX(const sf::Vector2f& pos, float delta) override;
		void internalOnScrollY(const sf::Vector2f& pos, float delta) override;

	private:
		bool getScrollbarXVisibleIndividual();
		bool getScrollbarYVisibleIndividual();
		bool getScrollbarXVisible();
		bool getScrollbarYVisible();
		void updateScrollbarXVisibility();
		void updateScrollbarYVisibility();
		void updateCornerVisibility();
		void updateScroll();
		void updateColors();
		float getSizeXFactor() const;
		float getSizeYFactor() const;
		float getXFactorFromArea() const;
		float getYFactorFromArea() const;
		float getXRange() const;
		float getYRange() const;
		float getScrollXRange() const;
		float getScrollYRange() const;
		float getSliderXFromArea() const;
		float getSliderYFromArea() const;
		float getSliderBgYEffectiveWidth() const;
		float getSliderBgXEffectiveHeight() const;
		void setSliderX(float x);
		void setSliderY(float y);
		void moveSliderX(float x_offset);
		void moveSliderY(float y_offset);
		void scrollX(float x_offset);
		void scrollY(float y_offset);

	};

}
