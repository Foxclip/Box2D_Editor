#include "widgets/polygon_widget.h"

namespace fw {

	PolygonWidget::PolygonWidget(WidgetList& widget_list, const std::vector<sf::Vector2f>& vertices) : ShapeWidget(widget_list) {
		type = WidgetType::Polygon;
		setName("polygon");
		setVertices(vertices);
	}

	sf::Drawable* PolygonWidget::getDrawable() {
		return &polygon;
	}

	const sf::Drawable* PolygonWidget::getDrawable() const {
		return &polygon;
	}

	sf::Transformable* PolygonWidget::getTransformable() {
		return &polygon;
	}

	const sf::Transformable* PolygonWidget::getTransformable() const {
		return &polygon;
	}

	sf::Shape& PolygonWidget::getShape() {
		return polygon;
	}

	const sf::Shape& PolygonWidget::getShape() const {
		return polygon;
	}

	void PolygonWidget::setVertices(const std::vector<sf::Vector2f>& vertices) {
		polygon.setPointCount(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			polygon.setPoint(i, vertices[i]);
		}
	}

	void PolygonWidget::setSizeInternal(float width, float height) {
		// nothing
	}

}
