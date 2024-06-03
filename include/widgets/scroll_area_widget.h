#pragma once

#include "empty_widget.h"
#include "container_widget.h"

namespace fw {

	const float SCROLL_AREA_SCROLLBAR_DEFAULT_WIDTH = 20.0f;
	const sf::Color SCROLL_AREA_DEFAULT_BACKGROUND_COLOR = sf::Color(100, 100, 100);
	const sf::Color SCROLL_AREA_DEFAULT_SCROLL_BACKGROUND_COLOR = sf::Color(128, 128, 128);
	const sf::Color SCROLL_AREA_DEFAULT_SLIDER_COLOR = sf::Color(150, 150, 150);
	const sf::Color SCROLL_AREA_DEFAULT_SLIDER_HOVER_COLOR = sf::Color(175, 175, 175);
	const sf::Color SCROLL_AREA_DEFAULT_SLIDER_PRESSED_COLOR = sf::Color(200, 200, 200);
	const sf::Color SCROLL_AREA_DEFAULT_CORNER_COLOR = sf::Color(140, 140, 140);

	class WidgetList;

	class ScrollAreaWidget : public EmptyWidget {
	public:
		ScrollAreaWidget(WidgetList& widget_list, float width, float height);
		ScrollAreaWidget(WidgetList& widget_list, const sf::Vector2f& size);
		Widget* getWidget() const;
		void setWidget(Widget* widget);

	protected:
		RectangleWidget* area_widget = nullptr;
		Widget* scrolled_widget = nullptr;
		RectangleWidget* slider_background_x_widget = nullptr;
		RectangleWidget* slider_x_widget = nullptr;
		RectangleWidget* slider_background_y_widget = nullptr;
		RectangleWidget* slider_y_widget = nullptr;
		RectangleWidget* corner_widget = nullptr;
		bool slider_x_hover = false;
		bool slider_y_hover = false;
		bool slider_x_grabbed = false;
		bool slider_y_grabbed = false;
		float mouse_grab_pos;
		float slider_grab_pos;

		void setSize(float width, float height) override;
		void internalUpdate() override;

	private:
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
		void setSliderX(float x);
		void setSliderY(float y);

	};

}
