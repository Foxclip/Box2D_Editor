#include "widgets/shape_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	ShapeWidget::ShapeWidget(WidgetList& widget_list) : Widget(widget_list) { }

	sf::FloatRect ShapeWidget::getLocalBounds() const {
		return getShape().getLocalBounds();
	}

	const sf::Color& ShapeWidget::getFillColor() const {
		return getShape().getFillColor();
	}

	void ShapeWidget::setFillColor(const sf::Color& color) {
		getShape().setFillColor(color);
	}

	void ShapeWidget::setOutlineColor(const sf::Color& color) {
		getShape().setOutlineColor(color);
	}

	void ShapeWidget::setOutlineThickness(float thickness) {
		getShape().setOutlineThickness(thickness);
	}

}
