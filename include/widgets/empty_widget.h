#pragma once

#include "widget.h"

namespace fw {

	class WidgetList;

	class EmptyWidget : public Widget {
	public:
		EmptyWidget(WidgetList& widget_list);
		sf::FloatRect getLocalBounds() const override;

	protected:
		sf::Drawable* getDrawable() override;
		const sf::Drawable* getDrawable() const override;
		sf::Transformable* getTransformable() override;
		const sf::Transformable* getTransformable() const override;

	};

}
