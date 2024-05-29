#pragma once

#include "shape_widget.h"

namespace fw {

	class WidgetList;

	class PolygonWidget : public ShapeWidget {
	public:
		PolygonWidget(WidgetList& widget_list, const std::vector<sf::Vector2f>& vertices);

	protected:
		sf::ConvexShape polygon;

		sf::Drawable* getDrawable() override;
		const sf::Drawable* getDrawable() const override;
		sf::Transformable* getTransformable() override;
		const sf::Transformable* getTransformable() const override;
		sf::Shape& getShape() override;
		const sf::Shape& getShape() const override;
		void setVertices(const std::vector<sf::Vector2f>& vertices);
		void setSizeInternal(float width, float height) override;

	private:
		friend class WindowWidget;

	};

}
