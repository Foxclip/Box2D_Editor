#pragma once

#include "rectangle_widget.h"

namespace fw {

	class WidgetList;

	class CanvasWidget : public RectangleWidget {
	public:
		CanvasWidget(WidgetList& widget_list);
		void setTextureSize(unsigned int width, unsigned int height);
		void clear();
		void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default);
		void display();

	protected:
		sf::RenderTexture texture;

	private:

	};

}
