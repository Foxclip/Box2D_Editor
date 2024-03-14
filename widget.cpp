#include "widget.h"
#include "application.h"

Widget::Widget(WidgetList& widget_list) : widget_list(widget_list) { }

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

bool Widget::isVisualPositionQuantized() const {
	return false;
}

WidgetVisibility Widget::checkVisibility() const {
	WidgetVisibility v;
	sf::FloatRect global_bounds = getGlobalBounds();
	CompoundVector<Widget*> parents = getParentChain();
	v.addedToRoot = parents.contains(widget_list.root_widget);
	v.allParentsVisible = true;
	for (size_t i = 0; i < parents.size(); i++) {
		if (!parents[i]->visible) {
			v.allParentsVisible = false;
			break;
		}
	}
	v.visibleSetting = visible;
	v.onScreen = widget_list.getRootWidget()->getGlobalBounds().intersects(global_bounds);
	v.nonZeroSize = global_bounds.width > 0 && global_bounds.height > 0;
	v.hasUnclippedRegion = unclipped_region.isQuantizedNonZero();
	v.opaque = getFillColor().a > 0;
	return v;
}

void Widget::processClick(const sf::Vector2f& pos) {
	if (!visible) {
		return;
	}
	if (mouseIn) {
		if (!click_through) {
			widget_list.click_blocked = true;
		}
		if (isFocusable() && !widget_list.focused_widget_temp) {
			widget_list.focused_widget_temp = this;
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
			widget_list.release_blocked = true;
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

bool Widget::isFocused() const {
	return widget_list.focused_widget == this;
}

const std::string& Widget::getName() const {
	return name;
}

const std::string& Widget::getFullName() const {
	return full_name;
}

bool Widget::getClipChildren() const {
	return clip_children;
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
	Widget* widget = children_names.find(name);
	if (widget) {
		return widget;
	}
	for (size_t i = 0; i < children.size(); i++) {
		Widget* result = children[i]->find(name);
		if (result) {
			return result;
		}
	}
	return nullptr;
}

sf::FloatRect Widget::getVisualLocalBounds() const {
	return getLocalBounds();
}

sf::FloatRect Widget::getVisualParentLocalBounds() const {
	return getParentLocalBounds();
}

sf::FloatRect Widget::getVisualGlobalBounds() const {
	return getGlobalBounds();
}

const sf::FloatRect& Widget::getUnclippedRegion() const {
	return unclipped_region.get();
}

const sf::FloatRect& Widget::getQuantizedUnclippedRegion() const {
	return unclipped_region.getQuantized();
}

sf::Vector2f Widget::toGlobal(const sf::Vector2f& pos) const {
	return getGlobalTransform().transformPoint(pos);
}

sf::Vector2f Widget::toLocal(const sf::Vector2f& pos) const {
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

float Widget::getGlobalWidth() const {
	return getGlobalBounds().width;
}

float Widget::getGlobalHeight() const {
	return getGlobalBounds().height;
}

sf::Vector2f Widget::getAnchorOffset() const {
	return anchor_offset;
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

const sf::Vector2f Widget::getVisualGlobalTopLeft() const {
	sf::FloatRect bounds = getVisualGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top);
}

const sf::Vector2f Widget::getVisualGlobalTopRight() const {
	sf::FloatRect bounds = getVisualGlobalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top);
}

const sf::Vector2f Widget::getVisualGlobalBottomLeft() const {
	sf::FloatRect bounds = getVisualGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top + bounds.height);
}

const sf::Vector2f Widget::getVisualGlobalBottomRight() const {
	sf::FloatRect bounds = getVisualGlobalBounds();
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
		return setParentSilent(widget_list.root_widget);
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
		old_parent->children_names.remove(name, this);
	}
	new_parent->children.add(this);
	new_parent->children_names.add(name, this);
	this->parent = new_parent;
	transforms.invalidateGlobalTransform();
	updateFullName();
}

void Widget::setParent(Widget* new_parent) {
	setParentSilent(new_parent);
	internalOnSetParent(new_parent);
}

void Widget::setName(const std::string& new_name) {
	if (parent) {
		parent->children_names.remove(this->name, this);
		parent->children_names.add(new_name, this);
	}
	this->name = new_name;
	updateFullName();
}

void Widget::setClipChildren(bool value) {
	if (this == widget_list.root_widget && value == false) {
		assert(false, "Cannot disable clipChildren for root widget");
	}
	this->clip_children = value;
}

void Widget::removeFocus() {
	if (isFocused()) {
		widget_list.setFocusedWidget(nullptr);
	}
}

void Widget::processKeyboardEvent(const sf::Event& event) { }

sf::Vector2f Widget::getRenderPositionOffset() const {
	return sf::Vector2f(0.0f, 0.0f);
}

void Widget::update() {
	if (!visible) {
		return;
	}
	sf::Vector2f parent_size;
	if (parent) {
		parent_size = parent->getLocalBounds().getSize();
	} else {
		parent_size = sf::Vector2f(window.getSize());
	}
	sf::Vector2f anchored_pos = anchorToPos(parent_anchor, getPosition(), parent_size);
	setPosition(anchored_pos + anchor_offset);
	setOrigin(origin_anchor);
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->update();
	}
}

void Widget::internalOnSetParent(Widget* parent) { }

void Widget::internalOnClick(const sf::Vector2f& pos) { }

void Widget::internalOnMouseEnter(const sf::Vector2f& pos) { }

void Widget::internalOnMouseExit(const sf::Vector2f& pos) { }

void Widget::internalOnFocused() { }

void Widget::internalOnFocusLost() { }

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

void Widget::updateRenderTexture(const sf::FloatRect& texture_bounds) {
	if (texture_bounds.width == 0 || texture_bounds.height == 0) {
		return;
	}
	bool size_changed = texture_bounds.width != render_texture.getSize().x || texture_bounds.height != render_texture.getSize().y;
	if (size_changed) {
		render_texture.create(texture_bounds.width, texture_bounds.height);
	}
	sf::Transform parent_transform = getParentGlobalTransform();
	sf::Transform combined(parent_transform);
	sf::Vector2f local_origin = getOrigin();
	sf::Vector2f global_origin = toGlobal(local_origin);
	sf::Vector2f global_position = getGlobalPosition();
	sf::Vector2f global_origin_offset = global_position - global_origin;
	sf::Vector2f render_position_offset = getRenderPositionOffset();
	sf::Vector2f position_offset = global_origin_offset + render_position_offset;
	combined.translate(position_offset);
	utils::quantize_position(combined);
	render_view.setSize(texture_bounds.getSize());
	sf::Vector2f texture_bounds_center = texture_bounds.getPosition() + texture_bounds.getSize() / 2.0f;
	render_view.setCenter(texture_bounds_center);
	render_texture.setView(render_view);
	render_texture.clear(sf::Color::Transparent);
	getTransformable().setOrigin(sf::Vector2f());
	render_texture.draw(getDrawable(), combined);
	getTransformable().setOrigin(local_origin);
	render_texture.display();
}

void Widget::render(sf::RenderTarget& target) {
	if (!visible) {
		return;
	}
	if (!unclipped_region.isQuantizedNonZero()) {
		return;
	}
	updateRenderTexture(unclipped_region.getQuantized());
	sf::Sprite sprite = sf::Sprite(render_texture.getTexture());
	sprite.setPosition(unclipped_region.getQuantized().getPosition());
	target.draw(sprite);
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->render(target);
	}
}

void Widget::renderBounds(sf::RenderTarget& target, const sf::Color& color, bool include_children) {
	if (!visible) {
		return;
	}
	sf::FloatRect quantized_bounds = utils::quantize_rect(
		getGlobalBounds(),
		utils::QUANTIZE_MODE_CEIL_SUBTRACT
	);
	draw_wire_rect(target, quantized_bounds, color);
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
	sf::Color origin_color = widget_list.render_origin_color;
	float offset = widget_list.render_origin_size;
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

ShapeWidget::ShapeWidget(WidgetList& widget_list) : Widget(widget_list) { }

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

RectangleWidget::RectangleWidget(WidgetList& widget_list) : ShapeWidget(widget_list) {
	setName("rectangle");
}

void RectangleWidget::setSize(float width, float height) {
	setSize(sf::Vector2f(width, height));
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

TextWidget::TextWidget(WidgetList& widget_list) : Widget(widget_list) {
	setName("text");
}

bool TextWidget::isVisualPositionQuantized() const {
	return true;
}

sf::FloatRect TextWidget::getLocalBounds() const {
	sf::FloatRect result;
	if (adjust_local_bounds) {
		sf::FloatRect local_bounds = text.getLocalBounds();
		sf::Vector2f size(local_bounds.left + local_bounds.width, text.getCharacterSize());
		result = sf::FloatRect(sf::Vector2f(), size);
	} else {
		result = getVisualLocalBounds();
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

sf::FloatRect TextWidget::getVisualLocalBounds() const {
	sf::FloatRect local_bounds = text.getLocalBounds();
	sf::Vector2f offset = getRenderPositionOffset();
	sf::Vector2f offset_pos = local_bounds.getPosition() + offset;
	sf::FloatRect offset_bounds = sf::FloatRect(offset_pos, local_bounds.getSize());
	return offset_bounds;
}

sf::FloatRect TextWidget::getVisualParentLocalBounds() const {
	return getTransformable().getTransform().transformRect(getVisualLocalBounds());
}

sf::FloatRect TextWidget::getVisualGlobalBounds() const {
	return getParentGlobalTransform().transformRect(getVisualParentLocalBounds());
}

const sf::Font* TextWidget::getFont() const {
	return text.getFont();
}

size_t TextWidget::getStringSize() const {
	return text.getString().getSize();
}

unsigned int TextWidget::getCharacterSize() const {
	return text.getCharacterSize();
}

const sf::Color& TextWidget::getFillColor() const {
	return text.getFillColor();
}

const sf::String& TextWidget::getString() const {
	return text.getString();
}

float TextWidget::getKerning(size_t index) const {
	if (index == 0) {
		return 0.0f;
	}
	sf::Uint32 left_char = getString()[index - 1];
	sf::Uint32 right_char = getString()[index];
	float kerning = getFont()->getKerning(left_char, right_char, getCharacterSize());
	return kerning;
}

sf::Vector2f TextWidget::getLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
	sf::Vector2f parent_local_char_pos = text.findCharacterPos(index);
	sf::Vector2f local_char_pos = getTransformable().getInverseTransform() * parent_local_char_pos;
	if (with_kerning) {
		local_char_pos.x += getKerning(index);
	}
	if (top_aligned) {
		return sf::Vector2f(local_char_pos.x, 0.0f);
	} else {
		return local_char_pos;
	}
}

sf::Vector2f TextWidget::getParentLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
	sf::Vector2f local_char_pos = getLocalCharPos(index, top_aligned, with_kerning);
	sf::Vector2f parent_local_char_pos = getTransformable().getTransform() * local_char_pos;
	return parent_local_char_pos;
}

sf::Vector2f TextWidget::getGlobalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
	sf::Vector2f local_pos = getLocalCharPos(index, top_aligned, with_kerning);
	sf::Vector2f global_pos = toGlobal(local_pos);
	return global_pos;
}

size_t TextWidget::getCharAt(const sf::Vector2f& pos) const {
	ptrdiff_t result = 0;
	for (size_t i = 0; i <= getStringSize(); i++) {
		sf::Vector2f local_char_pos = getLocalCharPos(i, true, true);
		if (pos.x >= local_char_pos.x) {
			result = i;
		}
	}
	return result;
}

void TextWidget::setFont(const sf::Font& font) {
	text.setFont(font);
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

void TextWidget::setString(const sf::String& string) {
	text.setString(string);
}

void TextWidget::insert(size_t pos, const sf::String& str) {
	assert(pos >= 0 && pos <= getStringSize());
	sf::String new_str = text.getString();
	new_str.insert(pos, str);
	text.setString(new_str);
}

void TextWidget::erase(size_t index_first, size_t count) {
	if (text.getString().isEmpty()) {
		return;
	}
	size_t index_last = index_first + count - 1;
	assert(index_first >= 0 && index_last < getStringSize());
	sf::String str = text.getString();
	str.erase(index_first, count);
	text.setString(str);
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

sf::Vector2f TextWidget::getRenderPositionOffset() const {
	sf::Vector2f offset(0.0f, 0.0f);
	if (!adjust_local_bounds) {
		sf::FloatRect bounds = text.getLocalBounds();
		offset = sf::Vector2f(0.0f, bounds.getPosition().y);
	}
	return -offset;
}

void TextWidget::update() {
	if (!getFont()) {
		assert(false, "Font is not set for " + full_name);
	}
	Widget::update();
}

ContainerWidget::ContainerWidget(WidgetList& widget_list) : RectangleWidget(widget_list) {
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

CheckboxWidget::CheckboxWidget(WidgetList& widget_list) : RectangleWidget(widget_list) {
	setSize(DEFAULT_SIZE);
	RectangleWidget::setFillColor(background_fill_color);
	setName("checkbox");
	check_widget = widget_list.createWidget<RectangleWidget>();
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

bool CheckboxWidget::getValue() const {
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

void CheckboxWidget::setValueSilent(bool value) {
	checked = value;
	check_widget->setVisible(value);
}

void CheckboxWidget::setValue(bool value) {
	setValueSilent(value);
	OnValueChanged(value);
}

void CheckboxWidget::toggleValue() {
	setValueSilent(!checked);
	OnValueChanged(checked);
}

void CheckboxWidget::update() {
	Widget::update();
	check_widget->setSize(rect.getSize() * check_size);
}

void CheckboxWidget::internalOnClick(const sf::Vector2f& pos) {
	toggleValue();
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
	root_widget->setClipChildren(true);
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

Widget* WidgetList::getRootWidget() const {
	return root_widget;
}

Widget* WidgetList::getFocusedWidget() const {
	return focused_widget;
}

Widget* WidgetList::find(const std::string& name) const {
	return root_widget->find(name);
}

void WidgetList::processClick(const sf::Vector2f pos) {
	focused_widget_temp = nullptr;
	root_widget->processClick(pos);
	setFocusedWidget(focused_widget_temp);
}

void WidgetList::processRelease(const sf::Vector2f pos) {
	root_widget->processRelease(pos);
}

void WidgetList::processKeyboardEvent(const sf::Event& event) {
	if (focused_widget) {
		focused_widget->processKeyboardEvent(event);
	}
}

void WidgetList::render(sf::RenderTarget& target) {
	root_widget->unclipped_region.invalidate();
	root_widget->update();
#ifndef NDEBUG
	for (size_t i = 0; i < widgets.size(); i++) {
		widgets[i]->updateVisibility();
	}
#endif
	root_widget->render(target);
	if (debug_render) {
		root_widget->renderBounds(target, sf::Color::Green, true);
		root_widget->renderOrigin(target);
	}
	if (focused_widget) {
		focused_widget->renderBounds(target, focused_widget_bounds_color, false);
	}
}

void WidgetList::reset(const sf::Vector2f& root_size) {
	root_widget->setSize(root_size);
	root_widget->updateMouseState();
	click_blocked = false;
	release_blocked = false;
}

void WidgetList::setFocusedWidget(Widget* widget) {
	LoggerTag tag_set_focused_widget("setFocusedWidget");
	if (widget == focused_widget) {
		return;
	}
	if (widget) {
		if (!widget->isFocusable()) {
			return;
		}
		assert(widgets.contains(widget));
	}
	if (focused_widget) {
		logger << focused_widget->getFullName() << " lost focus\n";
		focused_widget->internalOnFocusLost();
		focused_widget->OnFocusLost();
	}
	focused_widget = widget;
	if (focused_widget) {
		logger << focused_widget->getFullName() << " is focused\n";
		focused_widget->internalOnFocused();
		focused_widget->OnFocused();
	}
}

TextBoxWidget::TextBoxWidget(WidgetList& widget_list) : RectangleWidget(widget_list) {
	setSize(DEFAULT_SIZE);
	setName("textbox");
	setClipChildren(true);
	text_widget = widget_list.createWidget<TextWidget>();
	text_widget->setFillColor(text_color);
	text_widget->setParentAnchor(CENTER_LEFT);
	text_widget->setAnchorOffset(TEXT_VIEW_ZERO_POS);
	text_widget->setOrigin(CENTER_LEFT);
	text_widget->setParent(this);
	cursor_widget = widget_list.createWidget<RectangleWidget>();
	cursor_widget->setVisible(false);
	cursor_widget->setFillColor(editor_text_color);
	cursor_widget->setSize(sf::Vector2f(1.0f, text_widget->getCharacterSize()));
	cursor_widget->setParent(this);
	cursor_widget->setName("cursor");
	setValueSilent("Text");
	cursor_pos = getStringSize();
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

const sf::Color& TextBoxWidget::getFailFillColor() const {
	return fail_background_color;
}

const sf::Color& TextBoxWidget::getEditFailFillColor() const {
	return editor_fail_background_color;
}

const sf::Font* TextBoxWidget::getFont() const {
	return text_widget->getFont();
}

unsigned int TextBoxWidget::getCharacterSize() const {
	return text_widget->getCharacterSize();
}

const sf::String& TextBoxWidget::getValue() const {
	return text_widget->getString();
}

bool TextBoxWidget::isValidValue() const {
	bool result;
	if (type == TextBoxType::TEXT) {
		result = true;
	} else if (type == TextBoxType::INTEGER) {
		long long number;
		result = utils::parseLL(getValue(), number);
	} else if (type == TextBoxType::FLOAT) {
		float number;
		result = utils::parseFloat(getValue(), number);
	} else {
		assert(false, "Unknown TextBoxType");
	}
	return result;
}

size_t TextBoxWidget::getStringSize() const {
	return text_widget->getStringSize();
}

TextBoxWidget::TextBoxType TextBoxWidget::getType() const {
	return type;
}

size_t TextBoxWidget::getCursorPos() const {
	return cursor_pos;
}

sf::Vector2f TextBoxWidget::getLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
	sf::Vector2f local_pos = text_widget->getParentLocalCharPos(index, top_aligned, with_kerning);
	return local_pos;
}

sf::Vector2f TextBoxWidget::getGlobalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
	return text_widget->getGlobalCharPos(index, top_aligned, with_kerning);
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

void TextBoxWidget::setFailFillColor(const sf::Color& color) {
	this->fail_background_color = color;
}

void TextBoxWidget::setEditFailFillColor(const sf::Color& color) {
	this->editor_fail_background_color = color;
}

void TextBoxWidget::setFont(const sf::Font& font) {
	text_widget->setFont(font);
}

void TextBoxWidget::setCharacterSize(unsigned int size) {
	text_widget->setCharacterSize(size);
	cursor_widget->setSize(sf::Vector2f(1.0f, size));
}

void TextBoxWidget::setValueSilent(const sf::String& value) {
	text_widget->setString(value);
	setCursorPos(cursor_pos);
}

void TextBoxWidget::setValue(const sf::String& value) {
	setValueSilent(value);
	internalOnValueChanged(value);
	OnValueChanged(value);
}

void TextBoxWidget::setType(TextBoxType type) {
	this->type = type;
	updateValid();
}

void TextBoxWidget::setCursorPos(size_t pos) {
	pos = std::clamp(pos, (size_t)0, getStringSize());
	this->cursor_pos = pos;
	float char_pos = getLocalCharPos(pos, true, true).x;
	float cursor_visual_pos = char_pos + CURSOR_OFFSET.x;
	float left_bound = CURSOR_MOVE_MARGIN;
	float right_bound = getWidth() - CURSOR_MOVE_MARGIN;
	float offset_left = cursor_visual_pos - left_bound;
	float offset_right = cursor_visual_pos - right_bound;
	if (offset_left < 0) {
		text_view_pos -= sf::Vector2f(offset_left, 0.0f);
	}
	if (offset_right > 0) {
		text_view_pos -= sf::Vector2f(offset_right, 0.0f);
	}
}

void TextBoxWidget::typeChar(sf::Uint32 code) {
	if (type == TextBoxType::INTEGER) {
		if (VALID_INTEGER_CHARS.find(code) == -1) {
			return;
		}
	} else if (type == TextBoxType::FLOAT) {
		if (VALID_FLOAT_CHARS.find(code) == -1) {
			return;
		}
	}
	insert(cursor_pos, sf::String(code));
	setCursorPos(cursor_pos + 1);
}

void TextBoxWidget::insert(size_t pos, const sf::String& str) {
	insertSilent(pos, str);
	internalOnValueChanged(getValue());
	OnValueChanged(getValue());
}

void TextBoxWidget::erase(size_t index_first, size_t count) {
	eraseSilent(index_first, count);
	internalOnValueChanged(getValue());
	OnValueChanged(getValue());
}

void TextBoxWidget::processKeyboardEvent(const sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
			case sf::Keyboard::Escape:
				internalOnCancel();
				OnCancel();
				removeFocus();
				break;
			case sf::Keyboard::Enter:
				internalOnConfirm(getValue());
				OnConfirm(getValue());
				setEditMode(!edit_mode);
				break;
		}
		if (edit_mode) {
			switch (event.key.code) {
				case sf::Keyboard::Left:
					if (cursor_pos > 0) {
						setCursorPos(cursor_pos - 1);
					}
					break;
				case sf::Keyboard::Right:
					if (cursor_pos < getStringSize()) {
						setCursorPos(cursor_pos + 1);
					}
					break;
				case sf::Keyboard::Home:
					setCursorPos(0);
					break;
				case sf::Keyboard::End:
					setCursorPos(getStringSize());
					break;
				case sf::Keyboard::Delete:
					if (cursor_pos < getStringSize()) {
						erase(cursor_pos, 1);
					}
					break;
			}
		}
	} else if (event.type == sf::Event::TextEntered) {
		if (edit_mode) {
			sf::Uint32 code = event.text.unicode;
			switch (code) {
				case '\n':
				case '\r':
					break;
				case '\b':
					if (cursor_pos > 0) {
						erase(cursor_pos - 1, 1);
						setCursorPos(cursor_pos - 1);
					}
					break;
				default:
					typeChar(code);
					break;
			}
		}
	}
}

void TextBoxWidget::update() {
	Widget::update();
	text_widget->setAnchorOffset(TEXT_VIEW_ZERO_POS + text_view_pos);
	sf::Vector2f char_pos = getLocalCharPos(cursor_pos, true, true);
	cursor_widget->setPosition(char_pos + CURSOR_OFFSET);
}

void TextBoxWidget::updateColors() {
	sf::Color rect_col;
	sf::Color text_col;
	if (edit_mode) {
		if (fail_state) {
			rect_col = editor_fail_background_color;
			text_col = editor_text_color;
		} else {
			rect_col = editor_color;
			text_col = editor_text_color;
		}
	} else {
		if (highlighted) {
			if (fail_state) {
				rect_col = highlight_color;
			} else {
				rect_col = highlight_color;
			}
		} else {
			if (fail_state) {
				rect_col = fail_background_color;
			} else {
				rect_col = background_color;
			}
		}
		text_col = text_color;
	}
	RectangleWidget::setFillColor(rect_col);
	text_widget->setFillColor(text_col);
	cursor_widget->setFillColor(text_col);
}

void TextBoxWidget::internalOnClick(const sf::Vector2f& pos) {
	setEditMode(true);
	sf::Vector2f local_pos = text_widget->toLocal(pos);
	size_t char_left = text_widget->getCharAt(local_pos);
	if (char_left >= getStringSize()) {
		setCursorPos(getStringSize());
	} else {
		size_t char_right = char_left + 1;
		sf::Vector2f pos_left = getGlobalCharPos(char_left, true, true);
		sf::Vector2f pos_right = getGlobalCharPos(char_right, true, true);
		float dist_left = abs(pos.x - pos_left.x);
		float dist_right = abs(pos.x - pos_right.x);
		sf::Vector2f cursor_visual_pos;
		if (dist_left <= dist_right) {
			setCursorPos(char_left);
		} else {
			setCursorPos(char_right);
		}
	}
}

void TextBoxWidget::internalOnEditModeToggle(bool value) {
	cursor_widget->setVisible(value);
	if (!value) {
		setCursorPos(0);
	}
	updateColors();
}

void TextBoxWidget::internalOnFocused() {
	setEditMode(true);
}

void TextBoxWidget::internalOnFocusLost() {
	setEditMode(false);
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

void TextBoxWidget::internalOnValueChanged(const sf::String& new_value) {
	updateValid();
}

void TextBoxWidget::internalOnConfirm(const sf::String& value) { }

void TextBoxWidget::internalOnCancel() { }

void TextBoxWidget::updateValid() {
	fail_state = !isValidValue();
	updateColors();
}

void TextBoxWidget::setEditMode(bool value) {
	if (edit_mode == value) {
		return;
	}
	edit_mode = value;
	internalOnEditModeToggle(value);
	OnEditModeToggle(value);
}

void TextBoxWidget::insertSilent(size_t pos, const sf::String& str) {
	text_widget->insert(pos, str);
}

void TextBoxWidget::eraseSilent(size_t index_first, size_t count) {
	text_widget->erase(index_first, count);
}

WidgetUnclippedRegion::WidgetUnclippedRegion(Widget* widget) {
	this->widget = widget;
}

const sf::FloatRect& WidgetUnclippedRegion::get() const {
	if (!valid) {
		recalc();
	}
	return unclippedRegion;
}

const sf::FloatRect& WidgetUnclippedRegion::getQuantized() const {
	if (!valid) {
		recalc();
	}
	return quantizedUnclippedRegion;
}

bool WidgetUnclippedRegion::isNonZero() const {
	if (!valid) {
		recalc();
	}
	return unclippedRegion.width > 0 && unclippedRegion.height > 0;
}

bool WidgetUnclippedRegion::isQuantizedNonZero() const {
	if (!valid) {
		recalc();
	}
	return quantizedUnclippedRegion.width > 0 && quantizedUnclippedRegion.height > 0;
}

void WidgetUnclippedRegion::recalc() const {
	CompoundVector<Widget*> parents = widget->getParentChain();
	parents.reverse();
	sf::FloatRect result = widget->getVisualGlobalBounds();
	Widget* parent = widget->parent;
	if (parent && parent->getClipChildren()) {
		sf::FloatRect parent_unclipped_region = parent->getUnclippedRegion();
		sf::FloatRect intersection;
		bool intersects = result.intersects(parent_unclipped_region, intersection);
		if (intersects) {
			result = intersection;
		} else {
			result = sf::FloatRect(widget->getGlobalPosition(), sf::Vector2f());
		}
	}
	unclippedRegion = result;
	quantizedUnclippedRegion = utils::quantize_rect(result, utils::QUANTIZE_MODE_FLOOR);
	valid = true;
}

void WidgetUnclippedRegion::invalidate() {
	valid = false;
	for (size_t i = 0; i < widget->children.size(); i++) {
		widget->children[i]->unclipped_region.invalidate();
	}
}
