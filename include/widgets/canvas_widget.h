#pragma once

#include "rectangle_widget.h"

namespace fw {

	class WidgetList;

	class CanvasWidget : public RectangleWidget {
	public:
		CanvasWidget(WidgetList& widget_list);
		sf::RenderTexture& getRenderTexture();
		sf::Vector2f getTextureSize() const;
		const sf::View& getView() const;
		void setTextureSize(unsigned int width, unsigned int height);
		void setViewCenter(float x, float y);
		void setViewCenter(const sf::Vector2f& center);
		void setViewSize(float width, float height);
		void setViewSize(const sf::Vector2f& size);
		void resetView();
		void clear(const sf::Color& color = sf::Color::Black);
		void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default);
		void display();

	protected:
		sf::RenderTexture texture;
		sf::View view;

	private:

	};

}
