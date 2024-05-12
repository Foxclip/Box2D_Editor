#include "widgets/empty_widget.h"

namespace fw {

	EmptyWidget::EmptyWidget(WidgetList& widget_list) : Widget(widget_list) {
		setName("empty");
		setRenderable(false);
	}

	sf::FloatRect EmptyWidget::getLocalBounds() const {
		return sf::FloatRect();
	}

	const sf::Color& EmptyWidget::getFillColor() const {
		return sf::Color::Transparent;
	}

	void EmptyWidget::setFillColor(const sf::Color& color) { }

	void EmptyWidget::setRenderable(bool value) {
		if (value) {
			wAssert(false, "Cannot set EmptyWidget as renderable");
		} else {
			Widget::setRenderable(value);
		}

	}

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
