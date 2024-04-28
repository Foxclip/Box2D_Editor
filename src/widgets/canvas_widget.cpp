#include "widgets/canvas_widget.h"

fw::CanvasWidget::CanvasWidget(WidgetList& widget_list) : RectangleWidget(widget_list) {
	rect.setTexture(&texture.getTexture(), true);
}

void fw::CanvasWidget::setTextureSize(unsigned int width, unsigned int height) {
	texture.create(width, height);
	rect.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(texture.getSize().x, texture.getSize().y)));
	sf::IntRect texture_rect = rect.getTextureRect();
}

void fw::CanvasWidget::clear() {
	texture.clear();
}

void fw::CanvasWidget::draw(const sf::Drawable& drawable, const sf::RenderStates& states) {
	texture.draw(drawable, states);
}

void fw::CanvasWidget::display() {
	texture.display();
}
