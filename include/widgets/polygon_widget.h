#pragma once

#include "shape_widget.h"

namespace fw {

	class WidgetList;

	class PolygonWidget : public ShapeWidget {
	public:
		PolygonWidget(WidgetList& widget_list, const std::vector<sf::Vector2f>& vertices);
		PolygonWidget(WidgetList& widget_list, size_t vertex_count, float radius = 10.0f, float angle_offset = 0.0f);
		const std::vector<sf::Vector2f>& getVertices() const;
		void setVertices(const std::vector<sf::Vector2f>& vertices);
		PolygonWidget* clone(bool with_children = true) override;

	protected:
		sf::ConvexShape polygon;
		std::vector<sf::Vector2f> vertices;

		sf::Drawable* getDrawable() override;
		const sf::Drawable* getDrawable() const override;
		sf::Transformable* getTransformable() override;
		const sf::Transformable* getTransformable() const override;
		sf::Shape& getShape() override;
		const sf::Shape& getShape() const override;
		void setSizeInternal(float width, float height) override;

	private:
		friend class WindowWidget;

		void syncVertices();

	};

}
