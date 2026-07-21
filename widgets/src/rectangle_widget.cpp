#include "widgets/rectangle_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	RectangleWidget::RectangleWidget(WidgetList& widget_list, float width, float height) : ShapeWidget(widget_list) {
		type = WidgetType::Rectangle;
		setName("rectangle");
		setSize(width, height);
	}

	RectangleWidget::RectangleWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: RectangleWidget(widget_list, size.x, size.y) { }

	sf::Drawable* RectangleWidget::getDrawable() {
		return &rect;
	}

	const sf::Drawable* RectangleWidget::getDrawable() const {
		return &rect;
	}

	sf::Transformable* RectangleWidget::getTransformable() {
		return &rect;
	}

	const sf::Transformable* RectangleWidget::getTransformable() const {
		return &rect;
	}

	sf::Shape& RectangleWidget::getShape() {
		return rect;
	}

	const sf::Shape& RectangleWidget::getShape() const {
		return rect;
	}

	void RectangleWidget::setSizeInternal(float width, float height) {
		rect.setSize(sf::Vector2f(width, height));
		updateOrigin();
	}

	RectangleWidget* RectangleWidget::clone(bool with_children) {
		return widget_list.duplicateWidget(this, with_children);
	}

}
