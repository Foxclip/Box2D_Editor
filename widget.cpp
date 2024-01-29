#include "widget.h"

std::unique_ptr<sf::RenderWindow> window;

bool Widget::isMouseOver() {
	sf::FloatRect bounds = getLocalBounds();
	return utils::contains_point(bounds, utils::to2f(sf::Mouse::getPosition(*window)));
}

float Widget::getWidth() {
	return getLocalBounds().width;
}

float Widget::getHeight() {
	return getLocalBounds().height;
}

const sf::Vector2f& Widget::getPosition() {
	return getTransformable()->getPosition();
}

const sf::Vector2f Widget::getTopLeft() {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top);
}

const sf::Vector2f Widget::getTopRight() {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top);
}

const sf::Vector2f Widget::getBottomLeft() {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top + bounds.height);
}

const sf::Vector2f Widget::getBottomRight() {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height);
}

void Widget::setOrigin(Anchor anchor) {
	switch (anchor) {
		case Widget::TOP_LEFT: setOrigin(0.0f, 0.0f); break;
		case Widget::TOP_CENTER: setOrigin(getWidth() / 2.0f, 0.0f); break;
		case Widget::TOP_RIGHT: setOrigin(getWidth(), 0.0f); break;
		case Widget::LEFT_CENTER: setOrigin(0.0f, getHeight() / 2.0f); break;
		case Widget::CENTER: setOrigin(getWidth() / 2.0f, getHeight() / 2.0f); break;
		case Widget::RIGHT_CENTER: setOrigin(getWidth(), getHeight() / 2.0f); break;
		case Widget::BOTTOM_LEFT: setOrigin(0.0f, getHeight()); break;
		case Widget::BOTTOM_CENTER: setOrigin(getWidth() / 2.0f, getHeight()); break;
		case Widget::BOTTOM_RIGHT: setOrigin(getWidth(), getHeight()); break;
	}
}

void Widget::setOrigin(float x, float y) {
	getTransformable()->setOrigin(x, y);
}

void Widget::setOrigin(const sf::Vector2f& origin) {
	getTransformable()->setOrigin(origin);
}

void Widget::setPosition(float x, float y) {
	getTransformable()->setPosition(x, y);
}

void Widget::setPosition(const sf::Vector2f& position) {
	getTransformable()->setPosition(position);
}

void Widget::setRotation(float angle) {
	getTransformable()->setRotation(angle);
}

void Widget::render() {
	window->draw(*getDrawable());
}

void Widget::render(sf::RenderTarget& target) {
	target.draw(*getDrawable());
}

RectangleWidget::RectangleWidget() { }

sf::FloatRect RectangleWidget::getLocalBounds() {
	return rect.getLocalBounds();
}

sf::FloatRect RectangleWidget::getGlobalBounds() {
	return rect.getGlobalBounds();
}

void RectangleWidget::setFillColor(const sf::Color& color) {
	rect.setFillColor(color);
}

void RectangleWidget::setSize(const sf::Vector2f& size) {
	rect.setSize(size);
}

sf::Drawable* RectangleWidget::getDrawable() {
	return &rect;
}

sf::Transformable* RectangleWidget::getTransformable() {
	return &rect;
}

sf::FloatRect TextWidget::getLocalBounds() {
	return text.getLocalBounds();
}

sf::FloatRect TextWidget::getGlobalBounds() {
	return text.getGlobalBounds();
}

void TextWidget::setFont(const sf::Font& font) {
	text.setFont(font);
}

void TextWidget::setString(const std::string& string) {
	text.setString(string);
}

void TextWidget::setCharacterSize(unsigned int size) {
	text.setCharacterSize(size);
}

void TextWidget::setFillColor(const sf::Color& color) {
	text.setFillColor(color);
}

sf::Drawable* TextWidget::getDrawable() {
	return &text;
}

sf::Transformable* TextWidget::getTransformable() {
	return &text;
}
