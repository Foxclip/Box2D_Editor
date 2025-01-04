#include "widgets/canvas_widget.h"
#include "widgets/widget_list.h"
#include "widgets/application.h"

namespace fw {

	CanvasWidget::CanvasWidget(WidgetList& widget_list, float width, float height, unsigned int texture_width, unsigned int texture_height)
		: RectangleWidget(widget_list, width, height) {
		type = WidgetType::Canvas;
		setName("canvas");
		setTextureSize(texture_width, texture_height);
		rect.setTexture(&texture.getTexture(), true);
	}

	CanvasWidget::CanvasWidget(WidgetList& widget_list, const sf::Vector2f& size, const sf::Vector2u& texture_size)
		: CanvasWidget(widget_list, size.x, size.y, texture_size.x, texture_size.y) {
	}

	CanvasWidget::CanvasWidget(const CanvasWidget& other) : RectangleWidget(other) {
		type = WidgetType::Canvas;
		setName(other.name);
		setTextureSize(other.texture.getSize().x, other.texture.getSize().y);
		sf::Sprite sprite(other.texture.getTexture());
		texture.clear(sf::Color::Transparent);
		texture.draw(sprite);
		texture.display();
		rect.setTexture(&texture.getTexture(), true);
		this->view = other.view;
	}

	sf::RenderTexture& CanvasWidget::getRenderTexture() {
		return texture;
	}

	sf::Vector2f CanvasWidget::getTextureSize() const {
		return sf::Vector2f((float)texture.getSize().x, (float)texture.getSize().y);
	}

	const sf::View& CanvasWidget::getView() const {
		return view;
	}

	void CanvasWidget::setTextureSize(unsigned int width, unsigned int height) {
		if (width == texture.getSize().x && height == texture.getSize().y) {
			return;
		}
		texture.create(width, height);
		rect.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(texture.getSize().x, texture.getSize().y)));
	}

	void CanvasWidget::setView(const sf::View& view) {
		this->view = view;
		texture.setView(view);
	}

	void CanvasWidget::setViewCenter(float x, float y) {
		view.setCenter(x, y);
		texture.setView(view);
	}

	void CanvasWidget::setViewCenter(const sf::Vector2f& center) {
		setViewCenter(center.x, center.y);
	}

	void CanvasWidget::setViewSize(float width, float height) {
		view.setSize(width, height);
		texture.setView(view);
	}

	void CanvasWidget::setViewSize(const sf::Vector2f& size) {
		setViewSize(size.x, size.y);
	}

	void CanvasWidget::resetView() {
		view.setCenter(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
		view.setSize((float)texture.getSize().x, (float)texture.getSize().y);
		texture.setView(view);
	}

	void CanvasWidget::clear(const sf::Color& color) {
		texture.clear(color);
	}

	void CanvasWidget::draw(const sf::Drawable& drawable, ColorType color_type, const sf::RenderStates& states) {
		intenalDraw(drawable, color_type, states);
	}

	void CanvasWidget::display() {
		texture.display();
	}

	void CanvasWidget::saveToFile(std::filesystem::path path) {
		texture.getTexture().copyToImage().saveToFile(path.string());
	}

	CanvasWidget* CanvasWidget::clone(bool with_children) {
		return widget_list.duplicateWidget(this, with_children);
	}

	void CanvasWidget::draw(const sf::VertexArray& varray, const sf::RenderStates& states) {
		intenalDraw(varray, ColorType::VERTEX, states);
	}

	void CanvasWidget::draw(const sf::Shape& shape, const sf::RenderStates& states) {
		if (shape.getTexture()) {
			intenalDraw(shape, ColorType::TEXTURE, states);
		} else {
			intenalDraw(shape, ColorType::VERTEX, states);
		}
	}

	void CanvasWidget::draw(const sf::Text& text, const sf::RenderStates& states) {
		// SFML text rendering multiples texture mask by vertex color,
		// and so it must be recreated in premultiplying shader
		intenalDraw(text, ColorType::MULTIPLIED, states);
	}

	void CanvasWidget::intenalDraw(const sf::Drawable& drawable, ColorType color_type, const sf::RenderStates& states) {
		sf::RenderStates states_copy(states);
		states_copy.blendMode = sf::BlendMode(sf::BlendMode::One, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add);
		sf::Shader* premultiply = &widget_list.getApplication().premultiply;
		premultiply->setUniform("type", static_cast<int>(color_type));
		if (color_type == ColorType::TEXTURE || color_type == ColorType::MULTIPLIED) {
			premultiply->setUniform("src_texture", sf::Shader::CurrentTexture);
		}
		states_copy.shader = premultiply;
		texture.draw(drawable, states_copy);
	}

}