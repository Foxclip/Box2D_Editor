#pragma once

#include "widget.h"

namespace fw {

	class WidgetList;

	class EmptyWidget : public Widget {
	public:
		EmptyWidget(WidgetList& widget_list);
		sf::FloatRect getLocalBounds() const override;
		const sf::Color& getFillColor() const override;
		void setFillColor(const sf::Color& color) override;
		void setSize(float width, float height);
		void setSize(const sf::Vector2f& p_size);
		void setRenderable(bool value) override;

	protected:
		sf::Drawable* getDrawable() override;
		const sf::Drawable* getDrawable() const override;
		sf::Transformable* getTransformable() override;
		const sf::Transformable* getTransformable() const override;

	private:
		sf::Vector2f size;

	};

}
