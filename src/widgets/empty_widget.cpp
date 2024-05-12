#include "widgets/empty_widget.h"

fw::EmptyWidget::EmptyWidget(WidgetList& widget_list) : Widget(widget_list) { }

sf::FloatRect fw::EmptyWidget::getLocalBounds() const {
	return sf::FloatRect();
}

sf::Drawable* fw::EmptyWidget::getDrawable() {
	return nullptr;
}

const sf::Drawable* fw::EmptyWidget::getDrawable() const {
	return nullptr;
}

sf::Transformable* fw::EmptyWidget::getTransformable() {
	return nullptr;
}

const sf::Transformable* fw::EmptyWidget::getTransformable() const {
	return nullptr;
}
