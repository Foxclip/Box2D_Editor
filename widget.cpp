#include "widget.h"

bool Widget::isMouseOver() const {
	return mouseIn;
}

void Widget::updateMouseState() {
	sf::FloatRect bounds = getGlobalBounds();
	sf::Vector2f mouse_pos = utils::to2f(sf::Mouse::getPosition(window));
	bool is_over = utils::contains_point(bounds, mouse_pos);
	if (is_over && !mouseIn) {
		internalOnMouseEnter(mouse_pos);
		OnMouseEnter(mouse_pos);
	} else if (!is_over && mouseIn) {
		internalOnMouseExit(mouse_pos);
		OnMouseExit(mouse_pos);
	}
	mouseIn = is_over;
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->updateMouseState();
	}
}

WidgetVisibility Widget::checkVisibility() const {
	WidgetVisibility v;
	sf::FloatRect global_bounds = getGlobalBounds();
	const Widget* current = this;
	while (current) {
		if (current == widget_list->getRootWidget()) {
			v.addedToRoot = true;
			break;
		}
		current = parent;
	}
	v.visibleSetting = visible;
	v.onScreen = widget_list->getRootWidget()->getGlobalBounds().intersects(global_bounds);
	v.nonZeroSize = global_bounds.width > 0 && global_bounds.height > 0;
	v.opaque = getFillColor().a > 0;
	return v;
}

void Widget::processClick(const sf::Vector2f& pos) {
	if (!visible) {
		return;
	}
	if (mouseIn) {
		if (!click_through) {
			widget_list->click_blocked = true;
		}
		internalOnClick(pos);
		OnClick(pos);
	}
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->processClick(pos);
	}
}

void Widget::processRelease(const sf::Vector2f& pos) {
	if (!visible) {
		return;
	}
	if (mouseIn) {
		if (!click_through) {
			widget_list->release_blocked = true;
		}
		OnRelease(pos);
	}
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->processRelease(pos);
	}
}

const std::string& Widget::getName() const {
	return name;
}

const std::string& Widget::getFullName() const {
	return full_name;
}

Widget* Widget::getParent() const {
	return parent;
}

std::vector<Widget*> Widget::getParentChain() const {
	const Widget* cur_obj = this;
	std::vector<Widget*> result;
	while (cur_obj) {
		if (cur_obj->parent) {
			result.push_back(cur_obj->parent);
			cur_obj = cur_obj->parent;
		} else {
			break;
		}
	}
	return result;
}

const CompoundVector<Widget*>& Widget::getChildren() const {
	return children;
}

const sf::Vector2f& Widget::toGlobal(const sf::Vector2f& pos) const {
	return getGlobalTransform().transformPoint(pos);
}

const sf::Vector2f& Widget::toLocal(const sf::Vector2f& pos) const {
	return getInverseGlobalTransform().transformPoint(pos);
}

sf::Vector2f Widget::getSize() const {
	return getLocalBounds().getSize();
}

float Widget::getWidth() const {
	return getLocalBounds().width;
}

float Widget::getHeight() const {
	return getLocalBounds().height;
}

const sf::Vector2f& Widget::getOrigin() const {
	return getTransformable().getOrigin();
}

const sf::Vector2f& Widget::getPosition() const {
	return transforms.getPosition();
}

const sf::Vector2f& Widget::getGlobalPosition() const {
	return toGlobal(sf::Vector2f());
}

const sf::Vector2f Widget::getTopLeft() const {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top);
}

const sf::Vector2f Widget::getTopRight() const {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top);
}

const sf::Vector2f Widget::getBottomLeft() const {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top + bounds.height);
}

const sf::Vector2f Widget::getBottomRight() const {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height);
}

void Widget::setOrigin(Anchor anchor) {
	sf::Vector2f origin_pos = anchorToPos(anchor, getOrigin(), getSize());
	setOrigin(origin_pos);
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
	transforms.setPosition(sf::Vector2f(x, y));
}

void Widget::setPosition(const sf::Vector2f& position) {
	transforms.setPosition(position);
}

void Widget::setRotation(float angle) {
	transforms.setRotation(angle);
}

void Widget::setVisible(bool value) {
	this->visible = value;
}

void Widget::setClickThrough(bool value) {
	this->click_through = value;
}

void Widget::setParent(Widget* new_parent) {
	if (!new_parent) {
		return setParent(widget_list->root_widget);
	}
	Widget* old_parent = this->parent;
	if (old_parent) {
		old_parent->children.remove(this);
	}
	new_parent->children.add(this);
	this->parent = new_parent;
	transforms.invalidateGlobalTransform();
	updateFullName();
}

void Widget::setName(const std::string& name) {
	this->name = name;
	updateFullName();
}

void Widget::update() {
	sf::Vector2f parent_size;
	if (parent) {
		parent_size = parent->getLocalBounds().getSize();
	} else {
		parent_size = sf::Vector2f(window.getSize());
	}
	sf::Vector2f anchored_pos = anchorToPos(parent_anchor, getPosition(), parent_size);
	setPosition(anchored_pos + anchor_offset);
}

void Widget::internalOnClick(const sf::Vector2f& pos) { }

void Widget::internalOnMouseEnter(const sf::Vector2f& pos) { }

void Widget::internalOnMouseExit(const sf::Vector2f& pos) { }

std::string Widget::calcFullName() const {
	std::vector<Widget*> parents = getParentChain();
	std::string result;
	for (ptrdiff_t i = parents.size() - 1; i >= 0; i--) {
		result += parents[i]->name;
		result += "|";
	}
	result += getName();
	return result;
}

void Widget::updateFullName() {
	full_name = calcFullName();
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->updateFullName();
	}
}

void Widget::render(sf::RenderTarget& target) {
	if (!visible) {
		return;
	}
	update();
	target.draw(getDrawable(), getParentGlobalTransform());
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->render(target);
	}
}

void Widget::renderBounds(sf::RenderTarget& target) {
	if (!visible) {
		return;
	}
	sf::Color bounds_color = widget_list->render_bounds_color;
	drawLine(target, getTopRight(), getTopLeft(), bounds_color);
	drawLine(target, getTopLeft(), getBottomLeft(), bounds_color);
	drawLine(target, getBottomLeft(), getBottomRight(), bounds_color);
	drawLine(target, getBottomRight(), getTopRight(), bounds_color);
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->renderBounds(target);
	}
}

void Widget::renderOrigin(sf::RenderTarget& target) {
	if (!visible) {
		return;
	}
	sf::Color origin_color = widget_list->render_origin_color;
	float offset = widget_list->render_origin_size;
	sf::Vector2f hoffset = sf::Vector2f(offset, 0.0f);
	sf::Vector2f voffset = sf::Vector2f(0.0f, offset);
	sf::Vector2f pos = getGlobalPosition();
	drawLine(target, pos - hoffset, pos + hoffset, origin_color);
	drawLine(target, pos - voffset, pos + voffset, origin_color);
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->renderOrigin(target);
	}
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

const sf::Transform& Widget::getGlobalTransform() const {
	return transforms.getGlobalTransform();
}

const sf::Transform& Widget::getParentGlobalTransform() const {
	if (parent) {
		return parent->getGlobalTransform();
	} else {
		return sf::Transform::Identity;
	}
}

const sf::Transform& Widget::getInverseGlobalTransform() const {
	return transforms.getInverseGlobalTransform();
}

const sf::Transform& Widget::getInverseParentGlobalTransform() const {
	if (parent) {
		return parent->getInverseGlobalTransform();
	} else {
		return sf::Transform::Identity;
	}
}

sf::FloatRect ShapeWidget::getLocalBounds() const {
	return getShape().getLocalBounds();
}

sf::FloatRect ShapeWidget::getParentLocalBounds() const {
	return getShape().getGlobalBounds();
}

sf::FloatRect ShapeWidget::getGlobalBounds() const {
	return getParentGlobalTransform().transformRect(getShape().getGlobalBounds());
}

const sf::Color& ShapeWidget::getFillColor() const {
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

RectangleWidget::RectangleWidget(WidgetList* widget_list) {
	this->widget_list = widget_list;
	setName("rectangle");
}

void RectangleWidget::setSize(const sf::Vector2f& size) {
	rect.setSize(size);
	setOrigin(origin_anchor);
}

sf::Drawable& RectangleWidget::getDrawable() {
	return rect;
}

const sf::Drawable& RectangleWidget::getDrawable() const {
	return rect;
}

sf::Transformable& RectangleWidget::getTransformable() {
	return rect;
}

const sf::Transformable& RectangleWidget::getTransformable() const {
	return rect;
}

sf::Shape& RectangleWidget::getShape() {
	return rect;
}

const sf::Shape& RectangleWidget::getShape() const {
	return rect;
}

TextWidget::TextWidget() { }

TextWidget::TextWidget(WidgetList* widget_list) {
	this->widget_list = widget_list;
	setName("text");
}

sf::FloatRect TextWidget::getLocalBounds() const {
	if (adjust_local_bounds) {
		float width = text.getLocalBounds().width;
		float height = text.getCharacterSize();
		sf::FloatRect bounds(sf::Vector2f(), sf::Vector2f(width, height));
		return bounds;
	} else {
		return getExactLocalBounds();
	}
}

sf::FloatRect TextWidget::getParentLocalBounds() const {
	sf::FloatRect local_bounds = getLocalBounds();
	return getTransformable().getTransform().transformRect(local_bounds);
}

sf::FloatRect TextWidget::getGlobalBounds() const {
	return getParentGlobalTransform().transformRect(getParentLocalBounds());
}

sf::FloatRect TextWidget::getExactLocalBounds() const {
	return text.getLocalBounds();
}

const sf::Vector2f& TextWidget::getPosition() const {
	if (adjust_local_bounds) {
		return transforms.getPosition();
	} else {
		sf::Vector2f shape_pos = transforms.getPosition();
		sf::FloatRect bounds = getExactLocalBounds();
		sf::Vector2f transform_pos = shape_pos + bounds.getPosition();
		return transform_pos;
	}
}

const sf::Color& TextWidget::getFillColor() const {
	return text.getFillColor();
}

void TextWidget::setPosition(float x, float y) {
	sf::Vector2f position(x, y);
	setPosition(position);
}

void TextWidget::setPosition(const sf::Vector2f& position) {
	if (adjust_local_bounds) {
		transforms.setPosition(position);
	} else {
		sf::FloatRect bounds = getExactLocalBounds();
		sf::Vector2f new_pos = position - bounds.getPosition();
		transforms.setPosition(new_pos);
	}
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

void TextWidget::setAdjustLocalBounds(bool value) {
	adjust_local_bounds = value;
}

sf::Drawable& TextWidget::getDrawable() {
	return text;
}

const sf::Drawable& TextWidget::getDrawable() const {
	return text;
}

sf::Transformable& TextWidget::getTransformable() {
	return text;
}

const sf::Transformable& TextWidget::getTransformable() const {
	return text;
}

ContainerWidget::ContainerWidget() { }

ContainerWidget::ContainerWidget(WidgetList* widget_list) {
	this->widget_list = widget_list;
	setName("container");
}

void ContainerWidget::setAutoResize(bool value) {
	this->auto_resize = value;
}

void ContainerWidget::setHorizontal(bool value) {
	this->horizontal = value;
}

void ContainerWidget::setPadding(float padding) {
	this->horizontal_padding = padding;
	this->vertical_padding = padding;
}

void ContainerWidget::setHorizontalPadding(float padding) {
	this->horizontal_padding = padding;
}

void ContainerWidget::setVerticalPadding(float padding) {
	this->vertical_padding = padding;
}

void ContainerWidget::update() {
	Widget::update();
	sf::FloatRect container_bounds = sf::FloatRect();
	float next_x = horizontal_padding, next_y = vertical_padding;
	for (size_t i = 0; i < children.size(); i++) {
		Widget* child = children[i];
		child->setPosition(next_x, next_y);
		sf::FloatRect child_bounds = sf::FloatRect(
			next_x, next_y, child->getWidth(), child->getHeight()
		);
		utils::extend_bounds(container_bounds, child_bounds);
		if (horizontal) {
			next_x += child->getWidth() + horizontal_padding;
		} else {
			next_y += child->getHeight() + vertical_padding;
		}
	}
	if (auto_resize) {
		setSize(sf::Vector2f(
			container_bounds.width + horizontal_padding,
			container_bounds.height + vertical_padding
		));
	}
}

WidgetTransform::WidgetTransform(Widget* widget) {
	this->widget = widget;
	global_transform = sf::Transform::Identity;
	global_transform_valid = false;
}

const sf::Transform& WidgetTransform::getTransform() const {
	return widget->getTransformable().getTransform();
}

const sf::Transform& WidgetTransform::getGlobalTransform() const {
	if (!global_transform_valid) {
		recalcGlobalTransform();
	}
	return global_transform;
}

const sf::Transform& WidgetTransform::getInverseGlobalTransform() const {
	if (!inv_global_transform_valid) {
		recalcInverseGlobalTransform();
	}
	return inv_global_transform;
}

const sf::Vector2f& WidgetTransform::getPosition() const {
	return widget->getTransformable().getPosition();
}

float WidgetTransform::getRotation() const {
	return widget->getTransformable().getRotation();
}

const sf::Vector2f& WidgetTransform::getScale() const {
	return widget->getTransformable().getScale();
}

void WidgetTransform::invalidateGlobalTransform() {
	global_transform_valid = false;
	inv_global_transform_valid = false;
	for (size_t i = 0; i < widget->getChildren().size(); i++) {
		Widget* child = widget->getChildren()[i];
		child->transforms.invalidateGlobalTransform();
	}
}

void WidgetTransform::setPosition(const sf::Vector2f& position) {
	widget->getTransformable().setPosition(position);
	invalidateGlobalTransform();
}

void WidgetTransform::setRotation(float angle) {
	widget->getTransformable().setRotation(angle);
	invalidateGlobalTransform();
}

void WidgetTransform::setScale(const sf::Vector2f& scale) {
	widget->getTransformable().setScale(scale);
	invalidateGlobalTransform();
}

void WidgetTransform::recalcGlobalTransform() const {
	const sf::Transform& transform = getTransform();
	const sf::Transform& parent_transform = widget->getParentGlobalTransform();
	global_transform = parent_transform * transform;
	global_transform_valid = true;
}

void WidgetTransform::recalcInverseGlobalTransform() const {
	const sf::Transform& transform = getTransform();
	const sf::Transform& global_transform = getGlobalTransform();
	inv_global_transform = global_transform.getInverse();
	inv_global_transform_valid = true;
}

CheckboxWidget::CheckboxWidget() { }

CheckboxWidget::CheckboxWidget(WidgetList* widget_list) {
	this->widget_list = widget_list;
	setSize(DEFAULT_SIZE);
	RectangleWidget::setFillColor(background_fill_color);
	setName("checkbox");
	check_widget = widget_list->createWidget<RectangleWidget>();
	check_widget->setVisible(checked);
	check_widget->setSize(rect.getSize() * check_size);
	check_widget->setFillColor(check_fill_color);
	check_widget->setOrigin(CENTER);
	check_widget->setParentAnchor(CENTER);
	check_widget->setParent(this);
	check_widget->setName("check");
}

bool CheckboxWidget::isChecked() const {
	return checked;
}

const sf::Color& CheckboxWidget::getFillColor() const {
	return background_fill_color;
}

const sf::Color& CheckboxWidget::getHighlightFillColor() const {
	return highlight_fill_color;
}

const sf::Color& CheckboxWidget::getCheckFillColor() const {
	return check_fill_color;
}

void CheckboxWidget::setFillColor(const sf::Color& color) {
	background_fill_color = color;
}

void CheckboxWidget::setHighlightFillColor(const sf::Color& color) {
	highlight_fill_color = color;
}

void CheckboxWidget::setCheckFillColor(const sf::Color& color) {
	check_fill_color = color;
}

void CheckboxWidget::setChecked(bool value) {
	checked = value;
	check_widget->setVisible(value);
}

void CheckboxWidget::update() {
	Widget::update();
	check_widget->setSize(rect.getSize() * check_size);
}

void CheckboxWidget::internalOnClick(const sf::Vector2f& pos) {
	toggleChecked();
}

void CheckboxWidget::internalOnMouseEnter(const sf::Vector2f& pos) {
	RectangleWidget::setFillColor(highlight_fill_color);
}

void CheckboxWidget::internalOnMouseExit(const sf::Vector2f& pos) {
	RectangleWidget::setFillColor(background_fill_color);
}

void CheckboxWidget::toggleChecked() {
	setChecked(!checked);
}

WidgetList::WidgetList() {
	root_widget = createWidget<RectangleWidget>();
	root_widget->setFillColor(sf::Color::Transparent);
	root_widget->setName("root");
}

bool WidgetList::isClickBlocked() const {
	return click_blocked;
}

bool WidgetList::isReleaseBlocked() const {
	return release_blocked;
}

Widget* WidgetList::getRootWidget() {
	return root_widget;
}

void WidgetList::processClick(const sf::Vector2f pos) {
	root_widget->processClick(pos);
}

void WidgetList::processRelease(const sf::Vector2f pos) {
	root_widget->processRelease(pos);
}

void WidgetList::render(sf::RenderTarget& target) {
	root_widget->render(target);
	if (debug_render) {
		root_widget->renderBounds(target);
		root_widget->renderOrigin(target);
	}
}

void WidgetList::reset(const sf::Vector2f& root_size) {
	root_widget->setSize(root_size);
	root_widget->updateMouseState();
	click_blocked = false;
	release_blocked = false;
}

