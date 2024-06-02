#pragma once

#include "empty_widget.h"
#include "container_widget.h"

namespace fw {

	const float SCROLL_AREA_SCROLLBAR_DEFAULT_WIDTH = 20.0f;
	const sf::Color SCROLL_AREA_DEFAULT_BACKGROUND_COLOR = sf::Color(100, 100, 100);
	const sf::Color SCROLL_AREA_DEFAULT_SCROLL_BACKGROUND_COLOR = sf::Color(128, 128, 128);
	const sf::Color SCROLL_AREA_DEFAULT_SLIDER_COLOR = sf::Color(150, 150, 150);
	const sf::Color SCROLL_AREA_DEFAULT_CORNER_COLOR = sf::Color(140, 140, 140);

	class WidgetList;

	class ScrollAreaWidget : public EmptyWidget {
	public:
		ScrollAreaWidget(WidgetList& widget_list, float width, float height);
		ScrollAreaWidget(WidgetList& widget_list, const sf::Vector2f& size);

	protected:
		ContainerWidget* container_widget = nullptr;
		RectangleWidget* slider_background_x_widget = nullptr;
		RectangleWidget* slider_x_widget = nullptr;
		RectangleWidget* slider_background_y_widget = nullptr;
		RectangleWidget* slider_y_widget = nullptr;
		RectangleWidget* corner_widget = nullptr;

		void setSize(float width, float height) override;

	private:

	};

}
