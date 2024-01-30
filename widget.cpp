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
	return getTransformable().getPosition();
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
	this->origin_anchor = anchor;
}

void Widget::setOrigin(float x, float y) {
	getTransformable().setOrigin(x, y);
	this->origin_anchor = CUSTOM;
}

void Widget::setOrigin(const sf::Vector2f& origin) {
	getTransformable().setOrigin(origin);
	this->origin_anchor = CUSTOM;
}

void Widget::setParentAnchor(Anchor anchor) {
	this->parent_anchor = anchor;
}

void Widget::setPosition(float x, float y) {
	getTransformable().setPosition(x, y);
}

void Widget::setPosition(const sf::Vector2f& position) {
	getTransformable().setPosition(position);
}

void Widget::setAdjustedPosition(float x, float y) {
	sf::FloatRect bounds = getLocalBounds();
	getTransformable().setPosition(x - bounds.left, y - bounds.top);
}

void Widget::setAdjustedPosition(const sf::Vector2f& position) {
	sf::FloatRect bounds = getLocalBounds();
	getTransformable().setPosition(position - bounds.getPosition());
}

void Widget::setRotation(float angle) {
	getTransformable().setRotation(angle);
}

void Widget::update() {
	float parent_width, parent_height;
	if (parent) {
		parent_width = parent->getWidth();
		parent_height = parent->getHeight();
	} else {
		parent_width = window->getSize().x;
		parent_height = window->getSize().y;
	}
	switch (parent_anchor) {
		case Widget::TOP_LEFT: setPosition(0.0f, 0.0f); break;
		case Widget::TOP_CENTER: setPosition(parent_width / 2.0f, 0.0f); break;
		case Widget::TOP_RIGHT: setPosition(parent_width, 0.0f); break;
		case Widget::LEFT_CENTER: setPosition(0.0f, parent_height / 2.0f); break;
		case Widget::CENTER: setPosition(parent_width / 2.0f, parent_height / 2.0f); break;
		case Widget::RIGHT_CENTER: setPosition(parent_width, parent_height / 2.0f); break;
		case Widget::BOTTOM_LEFT: setPosition(0.0f, parent_height); break;
		case Widget::BOTTOM_CENTER: setPosition(parent_width / 2.0f, parent_height); break;
		case Widget::BOTTOM_RIGHT: setPosition(parent_width, parent_height); break;
	}
}

void Widget::render() {
	render(*window);
}

void Widget::render(sf::RenderTarget& target) {
	update();
	target.draw(getDrawable(), getParentTransform());
	for (int i = 0; i < children.size(); i++) {
		children[i]->render(target);
	}
}

void Widget::addChild(std::unique_ptr<Widget> child) {
	child->parent = this;
	children.push_back(std::move(child));
}

sf::Transform Widget::getTransform() {
	// TODO: cache transform
	return getParentTransform() * getTransformable().getTransform();
}

sf::Transform Widget::getParentTransform() {
	if (parent) {
		return parent->getTransform();
	}
	return sf::Transform::Identity;
}

sf::FloatRect ShapeWidget::getLocalBounds() {
	return getShape().getLocalBounds();
}

sf::FloatRect ShapeWidget::getGlobalBounds() {
	return getShape().getGlobalBounds();
}

void ShapeWidget::setFillColor(const sf::Color& color) {
	getShape().setFillColor(color);
}

void ShapeWidget::setOutlineColor(const sf::Color& color) {
	getShape().setOutlineColor(color);
}

void ShapeWidget::setOutlineThickness(float thickness) {
	getShape().setOutlineThickness(thickness);
}

RectangleWidget::RectangleWidget() { }

void RectangleWidget::setSize(const sf::Vector2f& size) {
	rect.setSize(size);
	setOrigin(origin_anchor);
}

sf::Drawable& RectangleWidget::getDrawable() {
	return rect;
}

sf::Transformable& RectangleWidget::getTransformable() {
	return rect;
}

sf::Shape& RectangleWidget::getShape() {
	return rect;
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

void TextWidget::setOriginToTextCenter() {
	float x = text.getLocalBounds().width / 2.0f;
	float y = text.getCharacterSize() / 2.0f;
	text.setOrigin(x, y);
}

sf::Drawable& TextWidget::getDrawable() {
	return text;
}

sf::Transformable& TextWidget::getTransformable() {
	return text;
}

ContainerWidget::ContainerWidget() : RectangleWidget() { }

void ContainerWidget::setHorizontal(bool value) {
	this->horizontal = value;
}

void ContainerWidget::setPadding(float padding) {
	this->padding = padding;
}

void ContainerWidget::update() {
	Widget::update();
	sf::FloatRect container_bounds = sf::FloatRect();
	float next_x = padding, next_y = padding;
	for (int i = 0; i < children.size(); i++) {
		Widget* child = children[i].get();
		child->setAdjustedPosition(next_x, next_y);
		sf::FloatRect child_bounds = sf::FloatRect(next_x, next_y, child->getWidth(), child->getHeight());
		utils::extend_bounds(container_bounds, child_bounds);
		if (horizontal) {
			next_x += child->getWidth() + padding;
		} else {
			next_y += child->getHeight() + padding;
		}
	}
	setSize(sf::Vector2f(container_bounds.width + padding, container_bounds.height + padding));
}
