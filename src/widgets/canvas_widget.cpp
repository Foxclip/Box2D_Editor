#include "widgets/canvas_widget.h"

fw::CanvasWidget::CanvasWidget(WidgetList& widget_list) : RectangleWidget(widget_list) {
	rect.setTexture(&texture.getTexture(), true);
}

sf::RenderTexture& fw::CanvasWidget::getRenderTexture() {
	return texture;
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

void fw::CanvasWidget::setViewCenter(const sf::Vector2f& center) {
	view.setCenter(center);
	texture.setView(view);
}

void fw::CanvasWidget::resetView() {
	view.setCenter(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
	view.setSize((float)texture.getSize().x, (float)texture.getSize().y);
	texture.setView(view);
}

void fw::CanvasWidget::setViewSize(const sf::Vector2f& size) {
	view.setSize(size);
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
