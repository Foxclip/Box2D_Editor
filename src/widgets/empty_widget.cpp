#include "widgets/empty_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	EmptyWidget::EmptyWidget(WidgetList& widget_list) : Widget(widget_list) {
		type = WidgetType::Empty;
		setName("empty");
		setRenderable(false);
	}

	sf::FloatRect EmptyWidget::getLocalBounds() const {
		return sf::FloatRect(sf::Vector2f(), size);
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

	EmptyWidget* EmptyWidget::clone(bool with_children) {
		return widget_list.duplicateWidget(this, with_children);
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

	void EmptyWidget::setSizeInternal(float width, float height) {
		this->size.x = width;
		this->size.y = height;
		updateOrigin();
	}

}
