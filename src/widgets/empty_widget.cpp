#include "widgets/empty_widget.h"

namespace fw {

	EmptyWidget::EmptyWidget(WidgetList& widget_list) : Widget(widget_list) {
		setName("empty");
	}

	sf::FloatRect EmptyWidget::getLocalBounds() const {
		return sf::FloatRect();
	}

	const sf::Color& EmptyWidget::getFillColor() const {
		return sf::Color::Transparent;
	}

	CompVector<Widget*> EmptyWidget::getRenderQueue() const {
		return CompVector<Widget*>();
	}

	void EmptyWidget::setFillColor(const sf::Color& color) { }

	sf::Drawable* EmptyWidget::getDrawable() {
		return nullptr;
	}

	const sf::Drawable* EmptyWidget::getDrawable() const {
		return nullptr;
	}

	sf::Transformable* EmptyWidget::getTransformable() {
		return nullptr;
	}

	const sf::Transformable* EmptyWidget::getTransformable() const {
		return nullptr;
	}

}
