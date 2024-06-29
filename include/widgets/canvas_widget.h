#pragma once

#include "rectangle_widget.h"
#include <filesystem>

namespace fw {

	class WidgetList;

	class CanvasWidget : public RectangleWidget {
	public:
		CanvasWidget(WidgetList& widget_list, float width, float height, unsigned int texture_width, unsigned int texture_height);
		CanvasWidget(WidgetList& widget_list, const sf::Vector2f& size, const sf::Vector2u& texture_size);
		sf::RenderTexture& getRenderTexture();
		sf::Vector2f getTextureSize() const;
		const sf::View& getView() const;
		void setTextureSize(unsigned int width, unsigned int height);
		void setView(const sf::View& view);
		void setViewCenter(float x, float y);
		void setViewCenter(const sf::Vector2f& center);
		void setViewSize(float width, float height);
		void setViewSize(const sf::Vector2f& size);
		void resetView();
		void clear(const sf::Color& color = sf::Color::Black);
		void draw(const sf::Drawable& drawable, ColorType color_type, const sf::RenderStates& states = sf::RenderStates::Default);
		void draw(const sf::VertexArray& varray, const sf::RenderStates& states = sf::RenderStates::Default);
		void draw(const sf::Shape& shape, const sf::RenderStates& states = sf::RenderStates::Default);
		void draw(const sf::Text& text, const sf::RenderStates& states = sf::RenderStates::Default);
		void display();
		void saveToFile(std::filesystem::path path);

	protected:
		sf::RenderTexture texture;
		sf::View view;

	private:
		void intenalDraw(const sf::Drawable& drawable, ColorType color_type, const sf::RenderStates& states);

	};

}
