#pragma once

#include "widget.h"

namespace fw {

	class WidgetList;

	class ShapeWidget : public Widget {
	public:
		ShapeWidget(WidgetList& widget_list);
		sf::FloatRect getLocalBounds() const override;
		sf::FloatRect getParentLocalBounds() const override;
		sf::FloatRect getGlobalBounds() const override;
		virtual const sf::Color& getFillColor() const override;
		virtual void setFillColor(const sf::Color& color) override;
		void setOutlineColor(const sf::Color& color);
		void setOutlineThickness(float thickness);

	protected:
		virtual sf::Shape& getShape() = 0;
		virtual const sf::Shape& getShape() const = 0;
	};

}
