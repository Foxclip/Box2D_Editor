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
	CompoundVector<Widget*> parents = getParentChain();
	v.addedToRoot = parents.contains(widget_list->root_widget);
	v.allParentsVisible = true;
	for (size_t i = 0; i < parents.size(); i++) {
		if (!parents[i]->visible) {
			v.allParentsVisible = false;
			break;
		}
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
		if (isFocusable() && !widget_list->focused_widget) {
			widget_list->focused_widget = this;
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

bool Widget::isFocusable() const {
	return false;
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

CompoundVector<Widget*> Widget::getParentChain() const {
	const Widget* cur_obj = this;
	CompoundVector<Widget*> result;
	while (cur_obj) {
		if (cur_obj->parent) {
			result.add(cur_obj->parent);
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

Widget* Widget::find(const std::string& name) const {
	for (size_t i = 0; i < children.size(); i++) {
		if (children[i]->name == name) {
			return children[i];
		}
	}
	for (size_t i = 0; i < children.size(); i++) {
		Widget* result = children[i]->find(name);
		if (result) {
			return result;
		}
	}
	return nullptr;
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
	sf::FloatRect bounds = getParentLocalBounds();
	return sf::Vector2f(bounds.left, bounds.top);
}

const sf::Vector2f Widget::getTopRight() const {
	sf::FloatRect bounds = getParentLocalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top);
}

const sf::Vector2f Widget::getBottomLeft() const {
	sf::FloatRect bounds = getParentLocalBounds();
	return sf::Vector2f(bounds.left, bounds.top + bounds.height);
}

const sf::Vector2f Widget::getBottomRight() const {
	sf::FloatRect bounds = getParentLocalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height);
}

const sf::Vector2f Widget::getGlobalTopLeft() const {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top);
}

const sf::Vector2f Widget::getGlobalTopRight() const {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top);
}

const sf::Vector2f Widget::getGlobalBottomLeft() const {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top + bounds.height);
}

const sf::Vector2f Widget::getGlobalBottomRight() const {
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

void Widget::setParentSilent(Widget* new_parent) {
	if (!new_parent) {
		return setParentSilent(widget_list->root_widget);
	}
	if (new_parent == this) {
		updateFullName();
		throw std::runtime_error("Cannot parent object to itself: " + full_name);
	}
	if (new_parent) {
		CompoundVector<Widget*> parent_chain = new_parent->getParentChain();
		if (parent_chain.contains(this)) {
			std::string chain_str;
			chain_str += name;
			chain_str += " -> " + new_parent->name;
			for (size_t i = 0; i < parent_chain.size(); i++) {
				chain_str += " -> " + parent_chain[i]->name;
				if (parent_chain[i] == this) {
					break;
				}
			}
			throw std::runtime_error("Loop in widget parent hierarchy: " + chain_str);
		}
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

void Widget::setParent(Widget* new_parent) {
	setParentSilent(new_parent);
	internalOnSetParent(new_parent);
}

void Widget::setName(const std::string& name) {
	this->name = name;
	updateFullName();
}

sf::Transform Widget::getRenderTransform() const {
	return sf::Transform::Identity;
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
	setOrigin(origin_anchor);
}

void Widget::internalOnSetParent(Widget* parent) { }

void Widget::internalOnClick(const sf::Vector2f& pos) { }

void Widget::internalOnMouseEnter(const sf::Vector2f& pos) { }

void Widget::internalOnMouseExit(const sf::Vector2f& pos) { }

std::string Widget::calcFullName() const {
	CompoundVector<Widget*> parents = getParentChain();
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

void Widget::updateVisibility() {
	visibility = checkVisibility();
}

void Widget::render(sf::RenderTarget& target) {
	if (!visible) {
		return;
	}
	update();
	sf::Transform render_transform = getRenderTransform();
	sf::Transform parent_transform = getParentGlobalTransform();
	sf::Transform combined = render_transform * parent_transform;
	target.draw(getDrawable(), combined);
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->render(target);
	}
}

void Widget::renderBounds(sf::RenderTarget& target, const sf::Color& color, bool include_children) {
	if (!visible) {
		return;
	}
	draw_wire_rect(target, getGlobalBounds(), color);
	if (include_children) {
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->renderBounds(target, color, true);
		}
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
	draw_line(target, pos - hoffset, pos + hoffset, origin_color);
	draw_line(target, pos - voffset, pos + voffset, origin_color);
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
	sf::FloatRect result;
	sf::FloatRect local_bounds = getExactLocalBounds();
	if (adjust_local_bounds) {
		float width = local_bounds.width;
		float height = text.getCharacterSize();
		result = sf::FloatRect(sf::Vector2f(), sf::Vector2f(width, height));
	} else {
		result = local_bounds;
		sf::Vector2f pos = result.getPosition() - calcPositionOffset();
		result.left = pos.x;
		result.top = pos.y;
	}
	return result;
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

const sf::Font* TextWidget::getFont() const {
	return text.getFont();
}

const std::string& TextWidget::getString() const {
	return text.getString();
}

unsigned int TextWidget::getCharacterSize() const {
	return text.getCharacterSize();
}

const sf::Color& TextWidget::getFillColor() const {
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

sf::Transform TextWidget::getRenderTransform() const {
	// local bounds can be offset from origin
	sf::Vector2f offset = calcPositionOffset();
	sf::Transform result = sf::Transform::Identity;
	result.translate(-offset);
	// quantize pixel coordinates so letters are not dancing even without smoothiing
	sf::Transform global_tr = getGlobalTransform();
	float x_pos = global_tr.getMatrix()[12];
	float y_pos = global_tr.getMatrix()[13];
	float x_offset = x_pos - floor(x_pos);
	float y_offset = y_pos - floor(y_pos);
	sf::Vector2f subpixel_offset = sf::Vector2f(x_offset, y_offset);
	result.translate(-subpixel_offset);
	return result;
}

sf::Vector2f TextWidget::calcPositionOffset() const {
	sf::FloatRect bounds = getExactLocalBounds();
	if (adjust_local_bounds) {
		return sf::Vector2f(bounds.getPosition().x, 0.0f);
	} else {
		return bounds.getPosition();
	}
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

void ContainerWidget::setVerticalAlignment(Alignment alignment) {
	this->vertical_alignment = alignment;
}

void ContainerWidget::setHorizontalAlignment(Alignment alignment) {
	this->horizontal_alignment = alignment;
}

void ContainerWidget::update() {
	Widget::update();
	sf::FloatRect container_bounds = sf::FloatRect();
	float max_width = 0.0f, max_height = 0.0f;
	for (size_t i = 0; i < children.size(); i++) {
		if (children[i]->getWidth() > max_width) {
			max_width = children[i]->getWidth();
		}
		if (children[i]->getHeight() > max_height) {
			max_height = children[i]->getHeight();
		}
	}
	float next_x = horizontal_padding, next_y = vertical_padding;
	if (horizontal) {
		next_y += alignmentToOffset(vertical_alignment, max_height);
	} else {
		next_x += alignmentToOffset(horizontal_alignment, max_width);
	}
	for (size_t i = 0; i < children.size(); i++) {
		Widget* child = children[i];
		child->setPosition(next_x, next_y);
		sf::FloatRect child_bounds = child->getParentLocalBounds();
		utils::extend_bounds(container_bounds, child_bounds);
		if (horizontal) {
			child->setOrigin(alignmentToAnchor(vertical_alignment));
			next_x += child->getWidth() + horizontal_padding;
		} else {
			child->setOrigin(alignmentToAnchor(horizontal_alignment));
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

Widget::Anchor ContainerWidget::alignmentToAnchor(Alignment alignment) const {
	switch (alignment) {
		case Widget::ALIGN_TOP: return TOP_LEFT;
		case Widget::ALIGN_CENTER:
			if (horizontal) {
				return CENTER_LEFT;
			} else {
				return TOP_CENTER;
			}
			break;
		case Widget::ALIGN_BOTTOM: return BOTTOM_LEFT;
		case Widget::ALIGN_LEFT: return TOP_LEFT;
		case Widget::ALIGN_RIGHT: return TOP_RIGHT;
		default: assert(false, "Unknown alignment: " + std::to_string(alignment));
	}
}

float ContainerWidget::alignmentToOffset(Alignment alignment, float max_size) const {
	if (horizontal) {
		switch (vertical_alignment) {
			case ALIGN_TOP: return 0.0f;
			case ALIGN_CENTER: return max_size / 2.0f;
			case ALIGN_BOTTOM: return max_size;
		}
	} else {
		switch (horizontal_alignment) {
			case ALIGN_LEFT: return 0.0f;
			case ALIGN_CENTER: return max_size / 2.0f;
			case ALIGN_RIGHT: return max_size;
		}
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

bool CheckboxWidget::isFocusable() const {
	return true;
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

void CheckboxWidget::setCheckedSilent(bool value) {
	checked = value;
	check_widget->setVisible(value);
}

void CheckboxWidget::setChecked(bool value) {
	setCheckedSilent(value);
	OnToggle(value);
}

void CheckboxWidget::toggleChecked() {
	setCheckedSilent(!checked);
	OnToggle(checked);
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

WidgetList::WidgetList() {
	root_widget = createWidget<RectangleWidget>();
	root_widget->setFillColor(sf::Color::Transparent);
	root_widget->setName("root");
}

bool WidgetList::contains(const Widget* widget) {
	return widgets.contains(widget);
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
	focused_widget = nullptr;
	root_widget->processClick(pos);
}

void WidgetList::processRelease(const sf::Vector2f pos) {
	root_widget->processRelease(pos);
}

void WidgetList::render(sf::RenderTarget& target) {
	root_widget->render(target);
	if (debug_render) {
		root_widget->renderBounds(target, sf::Color::Green, true);
		root_widget->renderOrigin(target);
	}
	if (focused_widget) {
		focused_widget->renderBounds(target, sf::Color(0, 200, 255), false);
	}
#ifndef NDEBUG
	for (size_t i = 0; i < widgets.size(); i++) {
		widgets[i]->updateVisibility();
	}
#endif
}

void WidgetList::reset(const sf::Vector2f& root_size) {
	root_widget->setSize(root_size);
	root_widget->updateMouseState();
	click_blocked = false;
	release_blocked = false;
}

TextBoxWidget::TextBoxWidget() { }

TextBoxWidget::TextBoxWidget(WidgetList* widget_list) {
	this->widget_list = widget_list;
	setSize(DEFAULT_SIZE);
	setName("textbox");
	text_widget = widget_list->createWidget<TextWidget>();
	text_widget->setFillColor(text_color);
	text_widget->setParentAnchor(CENTER_LEFT);
	text_widget->setOrigin(CENTER_LEFT);
	text_widget->setParent(this);
	cursor_widget = widget_list->createWidget<RectangleWidget>();
	cursor_widget->setFillColor(editor_text_color);
	cursor_widget->setSize(sf::Vector2f(1.0f, text_widget->getCharacterSize()));
	cursor_widget->setParent(this);
	cursor_widget->setName("cursor");
	setValueSilent("Text");
	updateColors();
}

bool TextBoxWidget::isFocusable() const {
	return true;
}

const sf::Color& TextBoxWidget::getFillColor() const {
	return background_color;
}

const sf::Color& TextBoxWidget::getHighlightColor() const {
	return highlight_color;
}

const sf::Color& TextBoxWidget::getTextColor() const {
	return text_color;
}

const sf::Color& TextBoxWidget::getEditorColor() const {
	return editor_color;
}

const sf::Color& TextBoxWidget::getEditorTextColor() const {
	return editor_text_color;
}

const sf::Font* TextBoxWidget::getFont() const {
	return text_widget->getFont();
}

unsigned int TextBoxWidget::getCharacterSize() const {
	return text_widget->getCharacterSize();
}

const std::string& TextBoxWidget::getValue() const {
	return text_widget->getString();
}

void TextBoxWidget::setFillColor(const sf::Color& color) {
	this->background_color = color;
	updateColors();
}

void TextBoxWidget::setHighlightColor(const sf::Color& color) {
	this->highlight_color = color;
	updateColors();
}

void TextBoxWidget::setTextColor(const sf::Color& color) {
	this->text_color = color;
	updateColors();
}

void TextBoxWidget::setEditorColor(const sf::Color& color) {
	this->editor_color = color;
	updateColors();
}

void TextBoxWidget::setEditorTextColor(const sf::Color& color) {
	this->editor_text_color = color;
	updateColors();
}

void TextBoxWidget::setValueSilent(const std::string& value) {
	text_widget->setString(value);
}

void TextBoxWidget::setValue(const std::string& value) {
	setValueSilent(value);
	OnTextChanged(value);
}

void TextBoxWidget::setFont(const sf::Font& font) {
	text_widget->setFont(font);
}

void TextBoxWidget::setCharacterSize(unsigned int size) {
	text_widget->setCharacterSize(size);
	cursor_widget->setSize(sf::Vector2f(1.0f, size));
}

void TextBoxWidget::processKeyboardEvent(const sf::Event& event) {
}

void TextBoxWidget::update() {
	Widget::update();
	sf::Vector2f cursor_pos = text_widget->getTopRight() + cursor_offset;
	cursor_widget->setPosition(cursor_pos);
}

void TextBoxWidget::updateColors() {
	sf::Color rect_col;
	sf::Color text_col;
	if (edit_mode) {
		rect_col = editor_color;
		text_col = editor_text_color;
	} else {
		if (highlighted) {
			rect_col = highlight_color;
		} else {
			rect_col = background_color;
		}
		text_col = text_color;
	}
	RectangleWidget::setFillColor(rect_col);
	text_widget->setFillColor(text_col);
	cursor_widget->setFillColor(text_col);
}

void TextBoxWidget::internalOnSetParent(Widget* parent) {
	if (!getFont()) {
		logger << "WARNING: font is not set for " + full_name + "\n";
	}
}

void TextBoxWidget::internalOnEditModeToggle(bool value) {
	updateColors();
}

void TextBoxWidget::internalOnClick(const sf::Vector2f& pos) {
	if (!edit_mode) {
		edit_mode = true;
		internalOnEditModeToggle(true);
		OnEditModeToggle(true);
	}
}

void TextBoxWidget::internalOnMouseEnter(const sf::Vector2f& pos) {
	highlighted = true;
	if (!edit_mode) {
		updateColors();
	}
}

void TextBoxWidget::internalOnMouseExit(const sf::Vector2f& pos) {
	highlighted = false;
	if (!edit_mode) {
		updateColors();
	}
}
