#pragma once

#include "rectangle_widget.h"

namespace fw {

	class WidgetList;

	class CanvasWidget : public RectangleWidget {
	public:
		CanvasWidget(WidgetList& widget_list);
		sf::RenderTexture& getRenderTexture();
		const sf::View& getView() const;
		void setTextureSize(unsigned int width, unsigned int height);
		void setViewCenter(const sf::Vector2f& center);
		void resetView();
		void setViewSize(const sf::Vector2f& size);
		void clear(const sf::Color& color = sf::Color::Black);
		void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default);
		void display();

	protected:
		sf::RenderTexture texture;
		sf::View view;

	private:

	};

}
