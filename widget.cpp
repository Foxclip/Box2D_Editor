#include "widget.h"

std::unique_ptr<sf::RenderWindow> window;
RectangleWidget root_widget;
bool Widget::click_blocked = false;

bool Widget::isMouseOver() {
	return mouseIn;
}

WidgetVisibility Widget::checkVisibility() {
	WidgetVisibility v;
	sf::FloatRect global_bounds = getGlobalBounds();
	Widget* current = this;
	while (current) {
		if (current == &root_widget) {
			v.addedToRoot = true;
			break;
		}
		current = parent;
	}
	v.visibleSetting = visible;
	v.onScreen = root_widget.getGlobalBounds().intersects(global_bounds);
	v.nonZeroSize = global_bounds.width > 0 && global_bounds.height > 0;
	v.opaque = getFillColor().a > 0;
	return v;
}

void Widget::updateMouseState() {
	sf::FloatRect bounds = getGlobalBounds();
	bool is_over = utils::contains_point(bounds, utils::to2f(sf::Mouse::getPosition(*window)));
	if (is_over && !mouseIn) {
		OnMouseEnter();
	} else if (!is_over && mouseIn) {
		OnMouseExit();
	}
	mouseIn = is_over;
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->updateMouseState();
	}
}

void Widget::processClick(const sf::Vector2f& pos) {
	if (!visible) {
		return;
	}
	if (mouseIn) {
		if (!click_through) {
			click_blocked = true;
		}
		OnClick(pos);
	}
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->processClick(pos);
	}
}

std::vector<Widget*> Widget::getChildren() {
	std::vector<Widget*> result;
	for (size_t i = 0; i < children.size(); i++) {
		result.push_back(children[i].get());
	}
	return result;
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
		case Widget::CENTER_LEFT: setOrigin(0.0f, getHeight() / 2.0f); break;
		case Widget::CENTER: setOrigin(getWidth() / 2.0f, getHeight() / 2.0f); break;
		case Widget::CENTER_RIGHT: setOrigin(getWidth(), getHeight() / 2.0f); break;
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

void Widget::setAnchorOffset(float x, float y) {
	this->anchor_offset = sf::Vector2f(x, y);
}

void Widget::setAnchorOffset(const sf::Vector2f& offset) {
	this->anchor_offset = offset;
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

void Widget::setVisible(bool value) {
	this->visible = value;
}

void Widget::setClickThrough(bool value) {
	this->click_through = value;
}

void Widget::update() {
	sf::Vector2f parent_size;
	if (parent) {
		parent_size = parent->getLocalBounds().getSize();
	} else {
		parent_size = sf::Vector2f(window->getSize());
	}
	sf::Vector2f anchored_pos = anchorToPos(parent_anchor, getPosition(), parent_size);
	setPosition(anchored_pos + anchor_offset);
}

sf::Vector2f Widget::anchorToPos(Anchor p_anchor, const sf::Vector2f& orig, const sf::Vector2f& size) {
	float x = orig.x, y = orig.y;
	switch (p_anchor) {
		case Widget::TOP_LEFT: x = 0.0f; y = 0.0f; break;
		case Widget::TOP_CENTER: x = size.x / 2.0f; y = 0.0f; break;
		case Widget::TOP_RIGHT: x = size.x; y = 0.0f; break;
		case Widget::CENTER_LEFT: x = 0.0f; y = size.y / 2.0f; break;
		case Widget::CENTER: x = size.x / 2.0f; y = size.y / 2.0f; break;
		case Widget::CENTER_RIGHT: x = size.x; y = size.y / 2.0f; break;
		case Widget::BOTTOM_LEFT: x = 0.0f; y = size.y; break;
		case Widget::BOTTOM_CENTER: x = size.x / 2.0f; y = size.y; break;
		case Widget::BOTTOM_RIGHT: x = size.x; y = size.y; break;
	}
	return sf::Vector2f(x, y);
}

void Widget::render() {
	render(*window);
}

void Widget::render(sf::RenderTarget& target) {
	if (!visible) {
		return;
	}
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
	// cache this
	return getParentTransform().transformRect(getShape().getGlobalBounds());
}

const sf::Color& ShapeWidget::getFillColor() {
	return getShape().getFillColor();
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
	// cache this
	return getParentTransform().transformRect(text.getGlobalBounds());
}

const sf::Color& TextWidget::getFillColor() {
	return text.getFillColor();
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

void ContainerWidget::setAutoResize(bool value) {
	this->auto_resize = value;
}

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
	if (auto_resize) {
		setSize(sf::Vector2f(container_bounds.width + padding, container_bounds.height + padding));
	}
}
