#include "shape_widget.h"
#include "widget_list.h"

ShapeWidget::ShapeWidget(WidgetList& widget_list) : Widget(widget_list) { }

sf::FloatRect ShapeWidget::getLocalBounds() const {
	return getShape().getLocalBounds();
}

sf::FloatRect ShapeWidget::getParentLocalBounds() const {
	return getShape().getGlobalBounds();
}

sf::FloatRect ShapeWidget::getGlobalBounds() const {
	return getParentGlobalTransform().transformRect(getShape().getGlobalBounds());
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
