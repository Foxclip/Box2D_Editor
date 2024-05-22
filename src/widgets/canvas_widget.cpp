#include "widgets/canvas_widget.h"

fw::CanvasWidget::CanvasWidget(WidgetList& widget_list, float width, float height, unsigned int texture_width, unsigned int texture_height)
	: RectangleWidget(widget_list, width, height) {
	type = WidgetType::Canvas;
	setName("canvas");
	setTextureSize(texture_width, texture_height);
	rect.setTexture(&texture.getTexture(), true);
}

fw::CanvasWidget::CanvasWidget(WidgetList& widget_list, const sf::Vector2f& size, const sf::Vector2u& texture_size)
	: CanvasWidget(widget_list, size.x, size.y, texture_size.x, texture_size.y) { }

sf::RenderTexture& fw::CanvasWidget::getRenderTexture() {
	return texture;
}

sf::Vector2f fw::CanvasWidget::getTextureSize() const {
	return sf::Vector2f((float)texture.getSize().x, (float)texture.getSize().y);
}

const sf::View& fw::CanvasWidget::getView() const {
	return view;
}

void fw::CanvasWidget::setTextureSize(unsigned int width, unsigned int height) {
	if (width == texture.getSize().x && height == texture.getSize().y) {
		return;
	}
	texture.create(width, height);
	rect.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(texture.getSize().x, texture.getSize().y)));
}

void fw::CanvasWidget::setView(const sf::View& view) {
	this->view = view;
	texture.setView(view);
}

void fw::CanvasWidget::setViewCenter(float x, float y) {
	view.setCenter(x, y);
	texture.setView(view);
}

void fw::CanvasWidget::setViewCenter(const sf::Vector2f& center) {
	setViewCenter(center.x, center.y);
}

void fw::CanvasWidget::setViewSize(float width, float height) {
	view.setSize(width, height);
	texture.setView(view);
}

void fw::CanvasWidget::setViewSize(const sf::Vector2f& size) {
	setViewSize(size.x, size.y);
}

void fw::CanvasWidget::resetView() {
	view.setCenter(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
	view.setSize((float)texture.getSize().x, (float)texture.getSize().y);
	texture.setView(view);
}

void fw::CanvasWidget::clear(const sf::Color& color) {
	texture.clear(color);
}

void fw::CanvasWidget::draw(const sf::Drawable& drawable, const sf::RenderStates& states) {
	texture.draw(drawable, states);
}

void fw::CanvasWidget::display() {
	texture.display();
}
