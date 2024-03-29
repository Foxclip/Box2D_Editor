#include "widgets/widget.h"
#include "widgets/widget_list.h"

Widget::Widget(WidgetList& widget_list) : widget_list(widget_list) { }

bool Widget::isMouseOver() const {
	return mouseIn;
}

void Widget::updateMouseState(const sf::Vector2f& mouse_pos) {
	sf::FloatRect bounds = getGlobalBounds();
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
		children[i]->updateMouseState(mouse_pos);
	}
}

bool Widget::isVisualPositionQuantized() const {
	return false;
}

bool Widget::isVisible() const {
	return visible;
}

bool Widget::isClickThrough() const {
	return click_through;
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
	internalOnClick(pos);
	OnClick(pos);
}

void Widget::processRelease(const sf::Vector2f& pos) {
	internalOnRelease(pos);
	OnRelease(pos);
}

void Widget::processMouse(const sf::Vector2f& pos) {
	if (!visible) {
		return;
	}
	internalProcessMouse(pos);
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->processMouse(pos);
	}
}

bool Widget::isFocusable() const {
	return false;
}

bool Widget::isFocused() const {
	return widget_list.focused_widget == this;
}

sf::Cursor::Type Widget::getCursorType() const {
	return sf::Cursor::Arrow;
}

bool Widget::getForceCustomCursor() const {
	return force_custom_cursor;
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

RenderLayer Widget::getRenderLayer() const {
	return layer;
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

Widget* Widget::getChild(size_t index) const {
	return children[index];
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

sf::Vector2f Widget::getTopLeft() const {
	sf::FloatRect bounds = getParentLocalBounds();
	return sf::Vector2f(bounds.left, bounds.top);
}

sf::Vector2f Widget::getTopRight() const {
	sf::FloatRect bounds = getParentLocalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top);
}

sf::Vector2f Widget::getBottomLeft() const {
	sf::FloatRect bounds = getParentLocalBounds();
	return sf::Vector2f(bounds.left, bounds.top + bounds.height);
}

sf::Vector2f Widget::getBottomRight() const {
	sf::FloatRect bounds = getParentLocalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height);
}

sf::Vector2f Widget::getGlobalTopLeft() const {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top);
}

sf::Vector2f Widget::getGlobalTopRight() const {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top);
}

sf::Vector2f Widget::getGlobalBottomLeft() const {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top + bounds.height);
}

sf::Vector2f Widget::getGlobalBottomRight() const {
	sf::FloatRect bounds = getGlobalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height);
}

sf::Vector2f Widget::getVisualGlobalTopLeft() const {
	sf::FloatRect bounds = getVisualGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top);
}

sf::Vector2f Widget::getVisualGlobalTopRight() const {
	sf::FloatRect bounds = getVisualGlobalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top);
}

sf::Vector2f Widget::getVisualGlobalBottomLeft() const {
	sf::FloatRect bounds = getVisualGlobalBounds();
	return sf::Vector2f(bounds.left, bounds.top + bounds.height);
}

sf::Vector2f Widget::getVisualGlobalBottomRight() const {
	sf::FloatRect bounds = getVisualGlobalBounds();
	return sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height);
}

void Widget::setOrigin(Anchor anchor) {
	assert(!widget_list.isLocked());
	sf::Vector2f origin_pos = anchorToPos(anchor, getOrigin(), getSize());
	setOrigin(origin_pos);
	this->origin_anchor = anchor;
	updateAnchoredPosition();
}

void Widget::setOrigin(float x, float y) {
	assert(!widget_list.isLocked());
	getTransformable().setOrigin(x, y);
	this->origin_anchor = CUSTOM;
	updateAnchoredPosition();
}

void Widget::setOrigin(const sf::Vector2f& origin) {
	assert(!widget_list.isLocked());
	getTransformable().setOrigin(origin);
	this->origin_anchor = CUSTOM;
	updateAnchoredPosition();
}

void Widget::setParentAnchor(Anchor anchor) {
	assert(!widget_list.isLocked());
	this->parent_anchor = anchor;
	updateAnchoredPosition();
}

void Widget::setAnchorOffset(float x, float y) {
	assert(!widget_list.isLocked());
	this->anchor_offset = sf::Vector2f(x, y);
	updateAnchoredPosition();
}

void Widget::setAnchorOffset(const sf::Vector2f& offset) {
	assert(!widget_list.isLocked());
	this->anchor_offset = offset;
	updateAnchoredPosition();
}

void Widget::setPosition(float x, float y) {
	assert(!widget_list.isLocked());
	transforms.setPosition(sf::Vector2f(x, y));
}

void Widget::setPosition(const sf::Vector2f& position) {
	assert(!widget_list.isLocked());
	transforms.setPosition(position);
}

void Widget::setRotation(float angle) {
	assert(!widget_list.isLocked());
	transforms.setRotation(angle);
}

void Widget::setVisible(bool value) {
	assert(!widget_list.isLocked());
	this->visible = value;
	widget_list.render_queue.invalidate();
}

void Widget::setClickThrough(bool value) {
	assert(!widget_list.isLocked());
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
	widget_list.render_queue.invalidate();
}

void Widget::setParent(Widget* new_parent) {
	assert(!widget_list.isLocked());
	setParentSilent(new_parent);
	internalOnSetParent(new_parent);
}

void Widget::setForceCustomCursor(bool value) {
	assert(!widget_list.isLocked());
	this->force_custom_cursor = value;
}

void Widget::setName(const std::string& new_name) {
	assert(!widget_list.isLocked());
	if (parent) {
		parent->children_names.remove(this->name, this);
		parent->children_names.add(new_name, this);
	}
	this->name = new_name;
	updateFullName();
}

void Widget::setClipChildren(bool value) {
	assert(!widget_list.isLocked());
	if (this == widget_list.root_widget && value == false) {
		assert(false, "Cannot disable clipChildren for root widget");
	}
	this->clip_children = value;
}

void Widget::setRenderLayer(RenderLayer layer) {
	assert(!widget_list.isLocked());
	this->layer = layer;
	widget_list.render_queue.invalidate();
}

void Widget::removeFocus() {
	assert(!widget_list.isLocked());
	if (isFocused()) {
		widget_list.setFocusedWidget(nullptr);
	}
}

void Widget::processKeyboardEvent(const sf::Event& event) {
	assert(!widget_list.isLocked());
	internalProcessKeyboardEvent(event);
}

sf::Vector2f Widget::getRenderPositionOffset() const {
	return sf::Vector2f(0.0f, 0.0f);
}

void Widget::updateAnchoredPosition() {
	assert(!widget_list.isLocked());
	sf::Vector2f parent_size;
	if (parent) {
		parent_size = parent->getLocalBounds().getSize();
	}
	sf::Vector2f anchored_pos = anchorToPos(parent_anchor, getPosition(), parent_size);
	setPosition(anchored_pos + anchor_offset);
}

void Widget::update() {
	assert(!widget_list.isLocked());
	if (!visible) {
		return;
	}
	updateAnchoredPosition();
	setOrigin(origin_anchor);
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->update();
	}
	widget_list.render_queue.invalidate();
}

void Widget::internalOnSetParent(Widget* parent) { }

void Widget::internalOnClick(const sf::Vector2f& pos) { }

void Widget::internalOnRelease(const sf::Vector2f& pos) { }

void Widget::internalProcessKeyboardEvent(const sf::Event& event) { }

void Widget::internalProcessMouse(const sf::Vector2f& pos) { }

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
	assert(!widget_list.isLocked());
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
	bool size_changed =
		texture_bounds.width != render_texture.getSize().x
		|| texture_bounds.height != render_texture.getSize().y;
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
}

void Widget::renderBounds(sf::RenderTarget& target, const sf::Color& color, bool include_children) {
	if (!visible) {
		return;
	}
	sf::FloatRect quantized_bounds = utils::quantize_rect(
		getGlobalBounds(),
		utils::QUANTIZE_MODE_FLOOR_SUBTRACT
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
