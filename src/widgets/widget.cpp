#include "widgets/widget.h"
#include "widgets/container_widget.h"
#include "widgets/widget_list.h"
#include "widgets/application.h"

namespace fw {

	Widget::Widget(WidgetList& widget_list) : widget_list(widget_list) {
		shader = &widget_list.getApplication().default_shader;
	}

	Widget::~Widget() { }

	Widget::WidgetType Widget::getType() const {
		return type;
	}

	bool Widget::isContainer() const {
		return type == WidgetType::Container || type == WidgetType::TreeView;
	}

	bool Widget::isMouseOver() const {
		return mouseIn;
	}

	void Widget::updateMouseState(const sf::Vector2f& mouse_pos) {
		sf::FloatRect bounds = getUnclippedRegion();
		bool is_over = contains_point(bounds, mouse_pos);
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

	sf::Vector2f Widget::getRelativeMousePos() const {
		sf::Vector2f mouse_pos = widget_list.application.getMousePosf();
		sf::Vector2f global_pos = getGlobalPosition();
		sf::Vector2f relative_pos = mouse_pos - global_pos;
		return relative_pos;
	}

	bool Widget::isVisualPositionQuantized() const {
		return false;
	}

	bool Widget::isRenderable() const {
		return renderable;
	}

	bool Widget::isVisible() const {
		return visible;
	}

	bool Widget::isClickThrough() const {
		return click_through;
	}

	bool Widget::getChildrenLocked() const {
		return children_locked;
	}

	WidgetVisibility Widget::checkVisibility() const {
		WidgetVisibility v;
		sf::FloatRect global_bounds = getGlobalBounds();
		const CompVector<Widget*>& parents = getParentChain();
		v.addedToRoot = this == widget_list.root_widget || parents.contains(widget_list.root_widget);
		v.allParentsVisible = true;
		for (size_t i = 0; i < parents.size(); i++) {
			if (!parents[i]->visible) {
				v.allParentsVisible = false;
				break;
			}
		}
		v.renderableSetting = isRenderable();
		v.visibleSetting = isVisible();
		sf::FloatRect root_bounds = widget_list.getRootWidget()->getGlobalBounds();
		v.onScreen = root_bounds.intersects(global_bounds);
		v.nonZeroSize = global_bounds.width > 0 && global_bounds.height > 0;
		v.hasUnclippedRegion = unclipped_region.isQuantizedNonZero();
		v.opaque = getFillColor().a > 0;
		return v;
	}

	bool Widget::containsPoint(const sf::Vector2f& point, bool include_upper_bound) const {
		return contains_point(getGlobalBounds(), point, include_upper_bound);
	}

	bool Widget::unclippedRegionContainsPoint(const sf::Vector2f& point, bool include_upper_bound) const {
		return contains_point(getUnclippedRegion(), point, include_upper_bound);
	}

	void Widget::processLeftPress(const sf::Vector2f& pos, bool became_focused) {
		if (!visible) {
			return;
		}
		is_left_pressed = true;
		internalOnLeftPress(pos, became_focused);
		OnLeftPress(pos);
	}

	void Widget::processRightPress(const sf::Vector2f& pos) {
		if (!visible) {
			return;
		}
		is_right_pressed = true;
		internalOnRightPress(pos);
		OnRightPress(pos);
	}

	void Widget::processGlobalLeftRelease(const sf::Vector2f& pos) {
		if (!visible) {
			return;
		}
		internalOnGlobalLeftRelease(pos);
		OnGlobalLeftRelease(pos);
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->processGlobalLeftRelease(pos);
		}
		is_left_pressed = false;
	}

	void Widget::processBlockableLeftRelease(const sf::Vector2f& pos) {
		if (!visible) {
			return;
		}
		internalOnBlockableLeftRelease(pos);
		OnBlockableLeftRelease(pos);
		if (is_left_pressed) {
			internalOnLeftClick(pos);
			OnLeftClick(pos);
		}
	}

	void Widget::processGlobalRightRelease(const sf::Vector2f& pos) {
		if (!visible) {
			return;
		}
		internalOnGlobalRightRelease(pos);
		OnGlobalRightRelease(pos);
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->processGlobalRightRelease(pos);
		}
		is_right_pressed = false;
	}

	void Widget::processBlockableRightRelease(const sf::Vector2f& pos) {
		if (!visible) {
			return;
		}
		internalOnBlockableRightRelease(pos);
		OnBlockableRightRelease(pos);
		if (is_right_pressed) {
			internalOnRightClick(pos);
			OnRightClick(pos);
		}
	}

	void Widget::processMouseMove(const sf::Vector2f& pos) {
		if (!visible) {
			return;
		}
		internalOnMouseMoved(pos);
		OnMouseMoved(pos);
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->processMouseMove(pos);
		}
	}

	void Widget::processScrollX(const sf::Vector2f pos, float delta) {
		if (!visible) {
			return;
		}
		internalOnScrollX(pos, delta);
		OnScrollX(pos, delta);
	}

	void Widget::processScrollY(const sf::Vector2f pos, float delta) {
		if (!visible) {
			return;
		}
		internalOnScrollY(pos, delta);
		OnScrollY(pos, delta);
	}

	void Widget::processMouse(const sf::Vector2f& pos) {
		if (!visible) {
			return;
		}
		internalProcessMouse(pos);
		OnProcessMouse(pos);
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->processMouse(pos);
		}
	}

	WidgetList& Widget::getWidgetList() const {
		return widget_list;
	}

	Widget::FocusableType Widget::getFocusableType() const {
		return focusable_type;
	}

	bool Widget::isFocused() const {
		return widget_list.focused_widget == this;
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

	GlobalRenderLayer Widget::getGlobalRenderLayer() const {
		return global_layer;
	}

	size_t Widget::getLocalRenderLayer() const {
		return local_layers.at(const_cast<Widget*>(this));
	}

	size_t Widget::getParentLocalRenderLayer() const {
		if (!parent) {
			return 0;
		}
		return parent->local_layers.at(const_cast<Widget*>(this));
	}

	bool Widget::getQuantizeRenderedPosition() const {
		return quantize_position;
	}

	sf::Shader* Widget::getShader() const {
		return shader;
	}

	Widget* Widget::getParent() const {
		return parent;
	}

	const CompVector<Widget*>& Widget::getParentChain() const {
		return parent_chain.get();
	}

	const CompVector<Widget*>& Widget::getChildren() const {
		return children;
	}

	size_t Widget::getChildrenCount() const {
		return children.size();
	}

	CompVector<Widget*> Widget::getAllChildren() const {
		CompVector<Widget*> result;
		result.insert(result.end(), children.begin(), children.end());
		for (size_t i = 0; i < children.size(); i++) {
			const CompVector<Widget*>& child_children = children[i]->getAllChildren();
			result.insert(result.end(), child_children.begin(), child_children.end());
		}
		return result;
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

	CompVector<Widget*> Widget::getRenderQueue() const {
		CompVector<Widget*> result;
		if (!isVisible()) {
			return result;
		}
		std::set<RenderQueueLayer> layers;
		auto get_layer_index = [&](Widget* widget) {
			size_t layer_index = 0;
			if (local_layers.contains(widget)) {
				layer_index = local_layers.at(widget);
			}
			return layer_index;
		};
		size_t this_layer_index = get_layer_index(const_cast<Widget*>(this));
		RenderQueueLayer layer(this_layer_index);
		layer.widgets.add(const_cast<Widget*>(this));
		layers.insert(layer);
		for (size_t i = 0; i < getChildren().size(); i++) {
			Widget* widget = getChild(i);
			if (!widget->isVisible()) {
				continue;
			}
			size_t layer_index = get_layer_index(widget);
			CompVector<Widget*> child_render_queue = widget->getRenderQueue();
			auto it = layers.find(RenderQueueLayer(layer_index));
			if (it != layers.end()) {
				RenderQueueLayer* layer = const_cast<RenderQueueLayer*>(&*it);
				layer->widgets.insert(layer->widgets.end(), child_render_queue.begin(), child_render_queue.end());
			} else {
				RenderQueueLayer layer(layer_index);
				layer.widgets.insert(layer.widgets.end(), child_render_queue.begin(), child_render_queue.end());
				layers.insert(layer);
			}
		}
		for (const RenderQueueLayer& layer : layers) {
			result.insert(result.end(), layer.widgets.begin(), layer.widgets.end());
		}
		return result;
	}

	sf::FloatRect Widget::getParentLocalBounds() const {
		return getTransform().transformRect(getLocalBounds());
	}

	sf::FloatRect Widget::getGlobalBounds() const {
		return getGlobalTransform().transformRect(getLocalBounds());
	}

	sf::FloatRect Widget::getVisualLocalBounds() const {
		return getLocalBounds();
	}

	sf::FloatRect Widget::getVisualParentLocalBounds() const {
		return getTransform().transformRect(getVisualLocalBounds());
	}

	sf::FloatRect Widget::getVisualGlobalBounds() const {
		return getGlobalTransform().transformRect(getVisualLocalBounds());
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

	Widget::Anchor Widget::getParentAnchor() const {
		return parent_anchor;
	}

	sf::Vector2f Widget::getAnchorOffset() const {
		return anchor_offset;
	}

	Widget::SizePolicy Widget::getSizeXPolicy() const {
		return size_policy_x;
	}

	Widget::SizePolicy Widget::getSizeYPolicy() const {
		return size_policy_y;
	}

	const sf::Vector2f& Widget::getMinSize() const {
		return min_size;
	}

	const sf::Vector2f& Widget::getMaxSize() const {
		return max_size;
	}

	WidgetUpdateSocket* Widget::getPosXTarget() {
		return &pos_x_target;
	}

	WidgetUpdateSocket* Widget::getPosYTarget() {
		return &pos_y_target;
	}

	WidgetUpdateSocket* Widget::getSizeXTarget() {
		return &size_x_target;
	}

	WidgetUpdateSocket* Widget::getSizeYTarget() {
		return &size_y_target;
	}

	WidgetUpdateSocket* Widget::getChildrenXTarget() {
		return &children_x_target;
	}

	WidgetUpdateSocket* Widget::getChildrenYTarget() {
		return &children_y_target;
	}

	const CompVector<WidgetLink*>& Widget::getLinks() const {
		return links.getCompVector();
	}

	const sf::Transform& Widget::getTransform() const {
		return transforms.getTransform();
	}

	const sf::Transform& Widget::getInverseTransform() const {
		return transforms.getInverseTransform();
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

	const sf::Vector2f& Widget::getOrigin() const {
		return transforms.getOrigin();
	}

	Widget::Anchor Widget::getOriginAnchor() const {
		return origin_anchor;
	}

	const sf::Vector2f& Widget::getPosition() const {
		return transforms.getPosition();
	}

	sf::Vector2f Widget::getTransformPosition() const {
		return getPosition() - getOrigin();
	}

	sf::Vector2f Widget::getGlobalPosition() const {
		return getGlobalTransform().transformPoint(sf::Vector2f());
	}

	sf::Vector2f Widget::getGlobalOriginPosition() const {
		return getParentGlobalTransform().transformPoint(getPosition());
	}

	sf::Vector2f Widget::getTop() const {
		sf::FloatRect bounds = getParentLocalBounds();
		return sf::Vector2f(bounds.left + bounds.width / 2.0f, bounds.top);
	}

	sf::Vector2f Widget::getLeft() const {
		sf::FloatRect bounds = getParentLocalBounds();
		return sf::Vector2f(bounds.left, bounds.top + bounds.height / 2.0f);
	}

	sf::Vector2f Widget::getRight() const {
		sf::FloatRect bounds = getParentLocalBounds();
		return sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height / 2.0f);
	}

	sf::Vector2f Widget::getBottom() const {
		sf::FloatRect bounds = getParentLocalBounds();
		return sf::Vector2f(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height);
	}

	sf::Vector2f Widget::getGlobalTop() const {
		sf::FloatRect bounds = getGlobalBounds();
		return sf::Vector2f(bounds.left + bounds.width / 2.0f, bounds.top);
	}

	sf::Vector2f Widget::getGlobalLeft() const {
		sf::FloatRect bounds = getGlobalBounds();
		return sf::Vector2f(bounds.left, bounds.top + bounds.height / 2.0f);
	}

	sf::Vector2f Widget::getGlobalRight() const {
		sf::FloatRect bounds = getGlobalBounds();
		return sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height / 2.0f);
	}

	sf::Vector2f Widget::getGlobalBottom() const {
		sf::FloatRect bounds = getGlobalBounds();
		return sf::Vector2f(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height);
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

	sf::Vector2f Widget::getCenter() const {
		sf::FloatRect bounds = getLocalBounds();
		return bounds.getPosition() + bounds.getSize() / 2.0f;
	}

	sf::Vector2f Widget::getGlobalCenter() const {
		sf::FloatRect bounds = getGlobalBounds();
		return bounds.getPosition() + bounds.getSize() / 2.0f;
	}

	sf::Vector2f Widget::getVisualGlobalCenter() const {
		sf::FloatRect bounds = getVisualGlobalBounds();
		return bounds.getPosition() + bounds.getSize() / 2.0f;
	}

	float Widget::getAlphaMultiplier() const {
		return alpha_multiplier;
	}

	void Widget::setSize(float width, float height) {
		wAssert(!widget_list.isLocked());
		setSizeInternal(width, height);
	}

	void Widget::setSize(const sf::Vector2f& size) {
		wAssert(!widget_list.isLocked());
		setSize(size.x, size.y);
	}

	void Widget::setWidth(float width) {
		wAssert(!widget_list.isLocked());
		setSize(width, getHeight());
	}

	void Widget::setHeight(float height) {
		wAssert(!widget_list.isLocked());
		setSize(getWidth(), height);
	}

	void Widget::setSizeKeepPos(float width, float height) {
		wAssert(!widget_list.isLocked());
		sf::Vector2f orig_offset_pos = getTopLeft();
		setSize(width, height);
		sf::Vector2f new_offset_pos = getTopLeft();
		sf::Vector2f offset = orig_offset_pos - new_offset_pos;
		setPosition(getPosition() + offset);
	}

	void Widget::setSizeKeepPos(const sf::Vector2f& size) {
		wAssert(!widget_list.isLocked());
		setSizeKeepPos(size.x, size.y);
	}

	void Widget::setOrigin(Anchor anchor) {
		wAssert(!widget_list.isLocked());
		this->origin_anchor = anchor;
		updateOrigin();
	}

	void Widget::setOriginKeepPos(Anchor anchor) {
		wAssert(!widget_list.isLocked());
		sf::Vector2f old_orig = getOrigin();
		setOrigin(anchor);
		sf::Vector2f new_orig = getOrigin();
		sf::Vector2f offset = new_orig - old_orig;
		setPosition(getPosition() + offset);
	}

	void Widget::setOriginKeepPos(float x, float y) {
		wAssert(!widget_list.isLocked());
		sf::Vector2f old_orig = getOrigin();
		setOrigin(x, y);
		sf::Vector2f new_orig = getOrigin();
		sf::Vector2f offset = new_orig - old_orig;
		setPosition(getPosition() + offset);
	}

	void Widget::setOriginKeepPos(const sf::Vector2f& origin) {
		wAssert(!widget_list.isLocked());
		setOriginKeepPos(origin.x, origin.y);
	}

	void Widget::setOrigin(float x, float y) {
		wAssert(!widget_list.isLocked());
		setOriginInternal(x, y);
		origin_anchor = Anchor::CUSTOM;
	}

	void Widget::setOrigin(const sf::Vector2f& origin) {
		wAssert(!widget_list.isLocked());
		setOrigin(origin.x, origin.y);
		origin_anchor = Anchor::CUSTOM;
	}

	void Widget::setParentAnchor(Anchor anchor) {
		wAssert(!widget_list.isLocked());
		this->parent_anchor = anchor;
		updatePositionX();
		updatePositionY();
	}

	void Widget::setAnchorOffset(float x, float y) {
		wAssert(!widget_list.isLocked());
		this->anchor_offset = sf::Vector2f(x, y);
		updatePositionX();
		updatePositionY();
	}

	void Widget::setAnchorOffset(const sf::Vector2f& offset) {
		wAssert(!widget_list.isLocked());
		this->anchor_offset = offset;
		updatePositionX();
		updatePositionY();
	}

	void Widget::setAnchorOffsetX(float x) {
		wAssert(!widget_list.isLocked());
		setAnchorOffset(x, getAnchorOffset().y);
	}

	void Widget::setAnchorOffsetY(float y) {
		wAssert(!widget_list.isLocked());
		setAnchorOffset(getAnchorOffset().x, y);
	}

	void Widget::setSizeXPolicy(SizePolicy policy) {
		wAssert(!widget_list.isLocked());
		wAssert(this != widget_list.getRootWidget());
		this->size_policy_x = policy;
	}

	void Widget::setSizeYPolicy(SizePolicy policy) {
		wAssert(!widget_list.isLocked());
		wAssert(this != widget_list.getRootWidget());
		this->size_policy_y = policy;
	}

	void Widget::setSizePolicy(SizePolicy policy) {
		wAssert(!widget_list.isLocked());
		wAssert(this != widget_list.getRootWidget());
		setSizeXPolicy(policy);
		setSizeYPolicy(policy);
	}

	void Widget::setMinSize(float width, float height) {
		wAssert(!widget_list.isLocked());
		this->min_size.x = width;
		this->min_size.y = height;
	}

	void Widget::setMinSize(const sf::Vector2f& size) {
		wAssert(!widget_list.isLocked());
		setMinSize(size.x, size.y);
	}

	void Widget::setMaxSize(float width, float height) {
		wAssert(!widget_list.isLocked());
		this->max_size.x = width;
		this->max_size.y = height;
	}

	void Widget::setMaxSize(const sf::Vector2f& size) {
		wAssert(!widget_list.isLocked());
		setMaxSize(size.x, size.y);
	}

	void Widget::setPosition(float x, float y) {
		wAssert(!widget_list.isLocked());
		transforms.setPosition(sf::Vector2f(x, y));
	}

	void Widget::setPosition(const sf::Vector2f& position) {
		wAssert(!widget_list.isLocked());
		transforms.setPosition(position);
	}

	void Widget::setPositionX(float x) {
		wAssert(!widget_list.isLocked());
		transforms.setPosition(x, getPosition().y);
	}

	void Widget::setPositionY(float y) {
		wAssert(!widget_list.isLocked());
		transforms.setPosition(getPosition().x, y);
	}

	void Widget::setTransformPosition(float x, float y) {
		wAssert(!widget_list.isLocked());
		transforms.setPosition(sf::Vector2f(x, y) + getOrigin());
	}

	void Widget::setTransformPosition(const sf::Vector2f& position) {
		wAssert(!widget_list.isLocked());
		transforms.setPosition(position + getOrigin());
	}

	void Widget::setGlobalPosition(float x, float y) {
		wAssert(!widget_list.isLocked());
		transforms.setPosition(sf::Vector2f(x, y));
	}

	void Widget::setGlobalPosition(const sf::Vector2f& position) {
		wAssert(!widget_list.isLocked());
		transforms.setGlobalPosition(position);
	}

	void Widget::setGlobalPositionX(float x) {
		wAssert(!widget_list.isLocked());
		transforms.setGlobalPosition(x, getGlobalPosition().y);
	}

	void Widget::setGlobalPositionY(float y) {
		wAssert(!widget_list.isLocked());
		transforms.setGlobalPosition(getGlobalPosition().x, y);
	}

	void Widget::setRotation(float angle) {
		wAssert(!widget_list.isLocked());
		transforms.setRotation(angle);
	}

	void Widget::setRenderable(bool value) {
		wAssert(!widget_list.isLocked());
		this->renderable = value;
	}

	void Widget::setVisible(bool value) {
		wAssert(!widget_list.isLocked());
		this->visible = value;
	}

	void Widget::toggleVisible() {
		wAssert(!widget_list.isLocked());
		this->visible = !this->visible;
	}

	void Widget::setClickThrough(bool value) {
		wAssert(!widget_list.isLocked());
		if (this == widget_list.getRootWidget() && value) {
			wAssert(false, "Cannot enable clickThrough on root widget");
		}
		this->click_through = value;
	}

	void Widget::setFocusableType(FocusableType value) {
		wAssert(!widget_list.isLocked());
		this->focusable_type = value;
	}

	void Widget::setParentSilent(Widget* new_parent) {
		wAssert(!widget_list.isLocked());
		Stage stage = widget_list.application.getStage();
		wAssert(
			stage == Stage::NONE || stage == Stage::AFTER_INPUT,
			"Cannot set parent in this stage"
		);
		if (!new_parent) {
			return setParentSilent(widget_list.root_widget);
		}
		if (new_parent == this) {
			updateFullName();
			throw std::runtime_error("Cannot parent object to itself: " + full_name);
		}
		if (new_parent) {
			const CompVector<Widget*>& parent_chain = new_parent->getParentChain();
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
			old_parent->removeChild(this);
		}
		new_parent->addChild(this);
		transforms.invalidateGlobalTransform();
		updateFullName();
	}

	void Widget::setParent(Widget* new_parent) {
		wAssert(!widget_list.isLocked());
		setParentSilent(new_parent);
		internalOnSetParent(new_parent);
	}

	void Widget::setParentKeepPosSilent(Widget* new_parent) {
		wAssert(!widget_list.isLocked());
		sf::Vector2f old_global_pos = getGlobalOriginPosition();
		setParentSilent(new_parent);
		setGlobalPosition(old_global_pos);
	}

	void Widget::setParentKeepPos(Widget* new_parent) {
		wAssert(!widget_list.isLocked());
		sf::Vector2f old_global_pos = getGlobalOriginPosition();
		setParent(new_parent);
		setGlobalPosition(old_global_pos);
	}

	void Widget::moveChildToIndex(Widget* child, size_t index) {
		wAssert(!widget_list.isLocked());
		wAssert(!children_locked);
		wAssert(children.contains(child));
		wAssert(index >= 0 && index <= getChildrenCount());
		Stage stage = widget_list.application.getStage();
		wAssert(
			stage == Stage::NONE || stage == Stage::AFTER_INPUT,
			"Cannot reorder children in this stage"
		);
		children.moveValueToIndex(child, index);
	}

	void Widget::moveToIndex(size_t index) {
		wAssert(!widget_list.isLocked());
		wAssert(parent);
		Stage stage = widget_list.application.getStage();
		if (stage == Stage::INPUT) {
			widget_list.addPendingMove(this, index);
		} else if (stage == Stage::NONE || stage == Stage::AFTER_INPUT) {
			parent->moveChildToIndex(this, index);
		} else {
			wAssert(false, "Cannot reorder children in this stage");
		}
	}

	void Widget::moveToTop() {
		wAssert(!widget_list.isLocked());
		wAssert(parent);
		moveToIndex(parent->getChildrenCount());
	}

	void Widget::lockChildren() {
		wAssert(!widget_list.isLocked());
		children_locked = true;
	}

	void Widget::unlockChildren() {
		wAssert(!widget_list.isLocked());
		children_locked = false;
	}

	WidgetLink* Widget::addLink(
		const std::string& name,
		const std::vector<WidgetUpdateTarget*>& targets,
		const ExecuteFuncType& func
	) {
		wAssert(!widget_list.isLocked());
		DataPointerUnique<WidgetLink> uptr = make_data_pointer<WidgetLink>("WidgetLink " + name, name, targets, this, func);
		WidgetLink* ptr = uptr.get();
		for (size_t i = 0; i < targets.size(); i++) {
			WidgetUpdateTarget* target = targets[i];
			target->dependent_links.add(ptr);
		}
		links.add(std::move(uptr));
		return ptr;
	}

	WidgetLink* Widget::addLink(
		const std::string& name,
		WidgetUpdateTarget* target,
		const ExecuteFuncType& func
	) {
		wAssert(!widget_list.isLocked());
		std::vector<WidgetUpdateTarget*> targets = { target };
		return addLink(name, targets, func);
	}

	WidgetLink* Widget::addLink(
		const std::string& name,
		const TargetsFuncType& targets_func,
		const ExecuteFuncType& func
	) {
		wAssert(!widget_list.isLocked());
		DataPointerUnique<WidgetLink> uptr = make_data_pointer<WidgetLink>("WidgetLink " + name, name, targets_func, this, func);
		WidgetLink* ptr = uptr.get();
		links.add(std::move(uptr));
		return ptr;
	}

	void Widget::removeLink(WidgetLink* link) {
		wAssert(!widget_list.isLocked());
		wAssert(links.contains(link));
		for (size_t i = 0; i < link->getTargets().size(); i++) {
			WidgetUpdateTarget* target = link->getTargets()[i];
			target->dependent_links.remove(link);
		}
		for (size_t i = 0; i < link->dependent_links.size(); i++) {
			WidgetLink* dep_link = link->dependent_links[i];
			dep_link->remove();
		}
		links.remove(link);
	}

	void Widget::setForceCustomCursor(bool value) {
		wAssert(!widget_list.isLocked());
		this->force_custom_cursor = value;
	}

	void Widget::setName(const std::string& new_name) {
		wAssert(!widget_list.isLocked());
		if (parent) {
			parent->children_names.remove(this->name, this);
			parent->children_names.add(new_name, this);
		}
		this->name = new_name;
		updateFullName();
	}

	void Widget::setClipChildren(bool value) {
		wAssert(!widget_list.isLocked());
		if (this == widget_list.root_widget && value == false) {
			wAssert(false, "Cannot disable clipChildren for root widget");
		}
		this->clip_children = value;
	}

	void Widget::setGlobalRenderLayer(GlobalRenderLayer layer) {
		wAssert(!widget_list.isLocked());
		this->global_layer = layer;
	}

	void Widget::setLocalRenderLayer(size_t layer) {
		wAssert(!widget_list.isLocked());
		local_layers[this] = layer;
	}

	void Widget::setParentLocalRenderLayer(size_t layer) {
		wAssert(!widget_list.isLocked());
		wAssert(parent);
		parent->local_layers[this] = layer;
	}

	void Widget::setQuantizeRenderedPosition(bool value) {
		wAssert(!widget_list.isLocked());
		this->quantize_position = value;
	}

	void Widget::setShader(sf::Shader* shader) {
		wAssert(!widget_list.isLocked());
		this->shader = shader;
	}

	void Widget::setAlphaMultiplier(float value) {
		wAssert(!widget_list.isLocked());
		this->alpha_multiplier = value;
	}

	void Widget::removeFocus() {
		wAssert(!widget_list.isLocked());
		if (isFocused()) {
			widget_list.setFocusedWidget(nullptr);
		}
	}

	void Widget::processKeyboardEvent(const sf::Event& event) {
		wAssert(!widget_list.isLocked());
		internalProcessKeyboardEvent(event);
	}

	sf::Vector2f Widget::getRenderPositionOffset() const {
		return sf::Vector2f(0.0f, 0.0f);
	}

	void Widget::setOriginInternal(float x, float y) {
		wAssert(!widget_list.isLocked());
		transforms.setOrigin(x, y);
	}

	void Widget::setOriginInternal(const sf::Vector2f& origin) {
		wAssert(!widget_list.isLocked());
		setOriginInternal(origin.x, origin.y);
	}

	void Widget::setSizeInternal(const sf::Vector2f& size) {
		wAssert(!widget_list.isLocked());
		setSizeInternal(size.x, size.y);
	}

	void Widget::setRenderIterations(size_t iterations) {
		wAssert(!widget_list.isLocked());
		this->render_iterations = iterations;
	}

	void Widget::addChild(Widget* child) {
		wAssert(!widget_list.isLocked());
		wAssert(!children_locked);
		children.add(child);
		children_names.add(child->name, child);
		child->parent = this;
		child->parent_chain.invalidate();
	}

	void Widget::removeChild(Widget* child) {
		wAssert(!widget_list.isLocked());
		wAssert(!children_locked);
		children.remove(child);
		children_names.remove(child->name, child);
		local_layers.erase(child);
	}

	void Widget::removeSocket(WidgetUpdateSocket* socket) {
		wAssert(!widget_list.isLocked());
		for (size_t i = 0; i < socket->dependent_links.size(); i++) {
			WidgetLink* link = socket->dependent_links[i];
			link->remove();
		}
	}

	void Widget::updateOrigin() {
		wAssert(!widget_list.isLocked());
		sf::Vector2f origin_pos = getOrigin();
		if (origin_anchor != Anchor::CUSTOM) {
			origin_pos = anchorToPos(origin_anchor, getSize());
		}
		setOriginInternal(origin_pos);
	}

	void Widget::preUpdate() {
		wAssert(!widget_list.isLocked());
		if (!visible) {
			return;
		}
		updateOrigin();
		OnPreUpdate();
		internalPreUpdate();
		for (size_t i = 0; i < children.size(); i++) {
			getChild(i)->preUpdate();
		}
	}

	void Widget::postUpdate() {
		wAssert(!widget_list.isLocked());
		if (!visible) {
			return;
		}
		OnPostUpdate();
		internalPostUpdate();
		for (size_t i = 0; i < children.size(); i++) {
			getChild(i)->postUpdate();
		}
	}

	void Widget::updatePositionX() {
		wAssert(!widget_list.isLocked());
		if (!parent) {
			return;
		}
		if (parent_anchor == Anchor::CUSTOM) {
			return;
		}
		if (parent->isContainer()) {
			return;
		}
		sf::Vector2f parent_size = parent->getLocalBounds().getSize();
		sf::Vector2f anchored_pos = getPosition();
		anchored_pos = anchorToPos(parent_anchor, parent_size);
		sf::Vector2f offset_pos = anchored_pos + anchor_offset;
		setPosition(offset_pos.x, getPosition().y);
	}

	void Widget::updatePositionY() {
		wAssert(!widget_list.isLocked());
		if (!parent) {
			return;
		}
		if (parent_anchor == Anchor::CUSTOM) {
			return;
		}
		if (parent->isContainer()) {
			return;
		}
		sf::Vector2f parent_size = parent->getLocalBounds().getSize();
		sf::Vector2f anchored_pos = getPosition();
		anchored_pos = anchorToPos(parent_anchor, parent_size);
		sf::Vector2f offset_pos = anchored_pos + anchor_offset;
		setPosition(getPosition().x, offset_pos.y);
	}

	void Widget::updateChildrenX() { }

	void Widget::updateChildrenY() { }

	void Widget::internalPreUpdate() { }

	void Widget::internalPostUpdate() { }

	void Widget::updateSizeX() {
		wAssert(!widget_list.isLocked());
		sf::Vector2f new_pos = getPosition();
		sf::Vector2f new_size = getSize();
		if (size_policy_x == SizePolicy::PARENT) {
			if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(parent)) {
				new_pos.x = container->getInnerPaddingX();
				new_size.x = container->getWidth() - container->getInnerPaddingX() * 2;
			} else {
				new_pos.x = 0.0f;
				new_size.x = parent->getWidth();
			}
		}
		setPosition(new_pos);
		setSizeInternal(new_size);
	}

	void Widget::updateSizeY() {
		wAssert(!widget_list.isLocked());
		sf::Vector2f new_pos = getPosition();
		sf::Vector2f new_size = getSize();
		if (size_policy_y == SizePolicy::PARENT) {
			if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(parent)) {
				new_pos.y = container->getInnerPaddingY();
				new_size.y = container->getHeight() - container->getInnerPaddingY() * 2;
			} else {
				new_pos.y = 0.0f;
				new_size.y = parent->getHeight();
			}
		}
		setPosition(new_pos);
		setSizeInternal(new_size);
	}

	void Widget::internalOnSetParent(Widget* parent) { }

	void Widget::internalOnLeftPress(const sf::Vector2f& pos, bool became_focused) { }

	void Widget::internalOnRightPress(const sf::Vector2f& pos) { }

	void Widget::internalOnGlobalLeftRelease(const sf::Vector2f& pos) { }

	void Widget::internalOnBlockableLeftRelease(const sf::Vector2f& pos) { }

	void Widget::internalOnGlobalRightRelease(const sf::Vector2f& pos) { }

	void Widget::internalOnBlockableRightRelease(const sf::Vector2f& pos) { }

	void Widget::internalOnLeftClick(const sf::Vector2f& pos) { }

	void Widget::internalOnRightClick(const sf::Vector2f& pos) { }

	void Widget::internalOnScrollX(const sf::Vector2f& pos, float delta) { }

	void Widget::internalOnScrollY(const sf::Vector2f& pos, float delta) { }

	void Widget::internalProcessKeyboardEvent(const sf::Event& event) { }

	void Widget::internalProcessMouse(const sf::Vector2f& pos) { }

	void Widget::internalOnMouseMoved(const sf::Vector2f& pos) { }

	void Widget::internalOnMouseEnter(const sf::Vector2f& pos) { }

	void Widget::internalOnMouseExit(const sf::Vector2f& pos) { }

	void Widget::internalOnFocused() { }

	void Widget::internalOnFocusLost() { }

	void Widget::internalOnBeforeRender() { }

	void Widget::internalOnAfterRender() { }

	std::string Widget::calcFullName() const {
		const CompVector<Widget*>& parents = getParentChain();
		std::string result;
		for (ptrdiff_t i = parents.size() - 1; i >= 0; i--) {
			result += parents[i]->name;
			result += "|";
		}
		result += getName();
		return result;
	}

	void Widget::updateFullName() {
		wAssert(!widget_list.isLocked());
		full_name = calcFullName();
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->updateFullName();
		}
		ptrdiff_t index = widget_list.getAllWidgets().getIndex(this);
		if (index >= 0) {
			widget_list.getWidgetUptrs().getUptr(index).setName("Widget " + full_name);
		}
	}

	void Widget::updateVisibility() {
		visibility = checkVisibility();
	}

	void Widget::updateRenderTexture(const sf::FloatRect& texture_bounds) {
		if (texture_bounds.width == 0 || texture_bounds.height == 0) {
			return;
		}
		render_textures.create((unsigned int)texture_bounds.width, (unsigned int)texture_bounds.height);
		sf::Transform global_transform = getGlobalTransform();
		sf::Transform combined(global_transform);
		sf::Vector2f render_position_offset = getRenderPositionOffset();
		combined.translate(render_position_offset);
		if (getQuantizeRenderedPosition()) {
			fw::quantize_position(combined);
		}
		sf::Vector2f physical_size = to2f(render_textures.getPhysicalSize());
		render_view.setSize(physical_size);
		sf::Vector2f bounds_center = texture_bounds.getPosition() + physical_size / 2.0f;
		render_view.setCenter(bounds_center);
		sf::RenderTexture& normal_texture = render_textures.getNormal();
		normal_texture.setView(render_view);
		normal_texture.clear(sf::Color::Transparent);
		sf::Transformable* transformable = getTransformable();
		wAssert(transformable);
		const sf::Transform& transformable_transform = transformable->getTransform();
		wAssert(transformable_transform == sf::Transform::Identity);
		wAssert(transformable->getOrigin() == sf::Vector2f());
		OnBeforeRender(normal_texture);
		sf::Drawable* drawable = getDrawable();
		wAssert(drawable);
		sf::RenderStates states(combined);
		// CanvasWidget is premultiplied at this stage, the rest of widgets are not
		// Using this blend mode instead of sf::BlendAlpha gives same result for all widgets
		states.blendMode = sf::BlendMode(sf::BlendMode::One, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add);
		for (size_t i = 0; i < render_iterations; i++) {
			normal_texture.draw(*drawable, states);
		}
		OnAfterRender(normal_texture);
		normal_texture.display();
	}

	void Widget::render(sf::RenderTarget& target) {
		if (!visible) {
			return;
		}
		if (!unclipped_region.isQuantizedNonZero()) {
			return;
		}
		OnBeforeGlobalRender(target);
		if (isRenderable()) {
			if (alpha_multiplier == 0.0f) {
				return;
			}
			{
				// render with straight alpha to texture
				wAssert(shader);
				updateRenderTexture(unclipped_region.getQuantized());
				sf::Sprite sprite = sf::Sprite(render_textures.getNormal().getTexture());
				sprite.setTextureRect(sf::IntRect(sf::Vector2i(), to2i(render_textures.getSize())));
				sf::RenderStates states;
				states.blendMode = sf::BlendNone;
				states.shader = shader;
				shader->setUniform("texture", sf::Shader::CurrentTexture);
				shader->setUniform("alpha_multiplier", alpha_multiplier);
				sf::RenderTexture& premultiplied_texture = render_textures.getPremultiplied();
				premultiplied_texture.clear(sf::Color::Transparent);
				premultiplied_texture.draw(sprite, states);
				premultiplied_texture.display();
			}
			{
				// premultiply alpha
				sf::Sprite sprite = sf::Sprite(render_textures.getPremultiplied().getTexture());
				sprite.setTextureRect(sf::IntRect(sf::Vector2i(), to2i(render_textures.getSize())));
				sprite.setPosition(unclipped_region.getQuantized().getPosition());
				sf::RenderStates states;
				states.blendMode = sf::BlendMode(sf::BlendMode::One, sf::BlendMode::OneMinusSrcAlpha, sf::BlendMode::Add);
				if (type != WidgetType::Canvas) { // canvas is already premultiplied
					sf::Shader* premultiply = &widget_list.getApplication().premultiply;
					premultiply->setUniform("type", static_cast<int>(ColorType::TEXTURE));
					premultiply->setUniform("src_texture", sf::Shader::CurrentTexture);
					states.shader = premultiply;
				}
				target.draw(sprite, states);
			}
		}
		OnAfterGlobalRender(target);
	}

	void Widget::renderBounds(sf::RenderTarget& target, const sf::Color& color, bool include_children, bool transformed) {
		if (!visible) {
			return;
		}
		if (transformed) {
			sf::FloatRect quantized_local_bounds = quantize_rect(
				getLocalBounds(),
				QUANTIZE_MODE_FLOOR_SUBTRACT
			);
			draw_wire_rect(target, quantized_local_bounds, color, getGlobalTransform());
		} else {
			sf::FloatRect quantized_global_bounds = quantize_rect(
				getGlobalBounds(),
				QUANTIZE_MODE_FLOOR_SUBTRACT
			);
			draw_wire_rect(target, quantized_global_bounds, color);
		}
		if (include_children) {
			for (size_t i = 0; i < children.size(); i++) {
				children[i]->renderBounds(target, color, true, transformed);
			}
		}
	}

	void Widget::renderOrigin(sf::RenderTarget& target, bool include_children) {
		if (!visible) {
			return;
		}
		float offset = DEBUG_RENDER_ORIGIN_SIZE;
		sf::Vector2f hoffset = sf::Vector2f(offset, 0.0f);
		sf::Vector2f voffset = sf::Vector2f(0.0f, offset);
		sf::Vector2f origin_pos = getGlobalOriginPosition();
		draw_line(target, origin_pos - hoffset, origin_pos + hoffset, DEBUG_RENDER_TRANSFORM_POSITION_COLOR);
		draw_line(target, origin_pos - voffset, origin_pos + voffset, DEBUG_RENDER_TRANSFORM_POSITION_COLOR);
		sf::Vector2f pos = getGlobalPosition();
		draw_line(target, pos - hoffset, pos + hoffset, DEBUG_RENDER_POSITION_COLOR);
		draw_line(target, pos - voffset, pos + voffset, DEBUG_RENDER_POSITION_COLOR);
		if (include_children) {
			for (size_t i = 0; i < children.size(); i++) {
				children[i]->renderOrigin(target, true);
			}
		}
	}

	void Widget::setDebugRender(bool value) {
		debug_render = value;
	}

	void Widget::remove(bool with_clildren) {
		wAssert(!widget_list.isLocked());
		Stage stage = widget_list.application.getStage();
		if (stage == Stage::INPUT) {
			widget_list.addPendingDelete(this, with_clildren);
		} else if (stage == Stage::NONE || stage == Stage::AFTER_INPUT) {
			widget_list.removeWidget(this, with_clildren);
		} else {
			wAssert(false, "Cannot remove children in this stage");
		}
	}

	sf::Vector2f Widget::anchorToPos(Anchor p_anchor, const sf::Vector2f& size) {
		float x, y;
		switch (p_anchor) {
			case Anchor::TOP_LEFT: x = 0.0f; y = 0.0f; break;
			case Anchor::TOP_CENTER: x = size.x / 2.0f; y = 0.0f; break;
			case Anchor::TOP_RIGHT: x = size.x; y = 0.0f; break;
			case Anchor::CENTER_LEFT: x = 0.0f; y = size.y / 2.0f; break;
			case Anchor::CENTER: x = size.x / 2.0f; y = size.y / 2.0f; break;
			case Anchor::CENTER_RIGHT: x = size.x; y = size.y / 2.0f; break;
			case Anchor::BOTTOM_LEFT: x = 0.0f; y = size.y; break;
			case Anchor::BOTTOM_CENTER: x = size.x / 2.0f; y = size.y; break;
			case Anchor::BOTTOM_RIGHT: x = size.x; y = size.y; break;
			case Anchor::CUSTOM: wAssert("Custom anchor is not valid here"); return sf::Vector2f();
			default: wAssert("Unknown anchor"); return sf::Vector2f();
		}
		return sf::Vector2f(x, y);
	}

}
