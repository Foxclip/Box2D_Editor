#pragma once

#include "shape_widget.h"

namespace fw {

	class WidgetList;

	class RectangleWidget : public ShapeWidget {
	public:
		RectangleWidget(WidgetList& widget_list);
		void setSize(float width, float height);
		void setSize(const sf::Vector2f& size);

	protected:
		sf::RectangleShape rect;

		sf::Drawable& getDrawable() override;
		const sf::Drawable& getDrawable() const override;
		sf::Transformable& getTransformable() override;
		const sf::Transformable& getTransformable() const override;
		sf::Shape& getShape() override;
		const sf::Shape& getShape() const override;

	private:

	};

}
