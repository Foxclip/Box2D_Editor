#include "widgets/polygon_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	PolygonWidget::PolygonWidget(WidgetList& widget_list, const std::vector<sf::Vector2f>& vertices) : ShapeWidget(widget_list) {
		type = WidgetType::Polygon;
		setName("polygon");
		setVertices(vertices);
	}

	PolygonWidget::PolygonWidget(WidgetList& widget_list, size_t vertex_count, float radius, float angle_offset) : ShapeWidget(widget_list) {
		type = WidgetType::Polygon;
		setName("polygon");
		std::vector<sf::Vector2f> vertices = get_regular_polygon<sf::Vector2f>(vertex_count, radius, angle_offset);
		setVertices(vertices);
	}

	const std::vector<sf::Vector2f>& PolygonWidget::getVertices() const {
		return vertices;
	}

	void PolygonWidget::setVertices(const std::vector<sf::Vector2f>& vertices) {
		this->vertices = vertices;
		syncVertices();
	}

	PolygonWidget* PolygonWidget::clone(bool with_children) {
		return widget_list.duplicateWidget(this, with_children);
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

	void PolygonWidget::setSizeInternal(float width, float height) {
		// nothing
	}

	void PolygonWidget::syncVertices() {
		polygon.setPointCount(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			polygon.setPoint(i, vertices[i]);
		}
	}

}
