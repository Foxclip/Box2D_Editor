#include "widgets/widget.h"
#include "widgets/container_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	Widget::Widget(WidgetList& widget_list) : widget_list(widget_list) { }

	Widget::WidgetType Widget::getType() const {
		return type;
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
		CompVector<Widget*> parents = getParentChain();
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

	void Widget::processLeftPress(const sf::Vector2f& pos) {
		if (!visible) {
			return;
		}
		internalOnLeftPress(pos);
		OnLeftPress(pos);
	}

	void Widget::processRightPress(const sf::Vector2f& pos) {
		if (!visible) {
			return;
		}
		internalOnRightPress(pos);
		OnRightPress(pos);
	}

	void Widget::processLeftRelease(const sf::Vector2f& pos) {
		if (!visible) {
			return;
		}
		internalOnLeftRelease(pos);
		OnLeftRelease(pos);
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->processLeftRelease(pos);
		}
	}

	void Widget::processRightRelease(const sf::Vector2f& pos) {
		if (!visible) {
			return;
		}
		internalOnRightRelease(pos);
		OnRightRelease(pos);
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->processRightRelease(pos);
		}
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

	bool Widget::isFocusable() const {
		return is_focusable;
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

	sf::Shader* Widget::getShader() const {
		return shader;
	}

	Widget* Widget::getParent() const {
		return parent;
	}

	CompVector<Widget*> Widget::getParentChain() const {
		const Widget* cur_obj = this;
		CompVector<Widget*> result;
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

	const CompVector<Widget*>& Widget::getChildren() const {
		return children;
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
		if (isRenderable()) {
			size_t this_layer_index = get_layer_index(const_cast<Widget*>(this));
			RenderQueueLayer layer(this_layer_index);
			layer.widgets.add(const_cast<Widget*>(this));
			layers.insert(layer);
		}
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

	Widget::SizePolicy Widget::getHorizontalSizePolicy() const {
		return horizontal_size_policy;
	}

	Widget::SizePolicy Widget::getVerticalSizePolicy() const {
		return vertical_size_policy;
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

	void Widget::setSize(float width, float height) {
		setSizeInternal(width, height);
	}

	void Widget::setSize(const sf::Vector2f& size) {
		setSize(size.x, size.y);
	}

	void Widget::setSizeKeepPos(float width, float height) {
		sf::Vector2f orig_offset_pos = getTopLeft();
		setSize(width, height);
		sf::Vector2f new_offset_pos = getTopLeft();
		sf::Vector2f offset = orig_offset_pos - new_offset_pos;
		setPosition(getPosition() + offset);
	}

	void Widget::setSizeKeepPos(const sf::Vector2f& size) {
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
		setOriginKeepPos(origin.x, origin.y);
	}

	void Widget::setOrigin(float x, float y) {
		setOriginInternal(x, y);
		origin_anchor = Anchor::CUSTOM;
	}

	void Widget::setOrigin(const sf::Vector2f& origin) {
		setOrigin(origin.x, origin.y);
		origin_anchor = Anchor::CUSTOM;
	}

	void Widget::setParentAnchor(Anchor anchor) {
		wAssert(!widget_list.isLocked());
		this->parent_anchor = anchor;
		updateAnchoredPosition();
	}

	void Widget::setAnchorOffset(float x, float y) {
		wAssert(!widget_list.isLocked());
		this->anchor_offset = sf::Vector2f(x, y);
		updateAnchoredPosition();
	}

	void Widget::setAnchorOffset(const sf::Vector2f& offset) {
		wAssert(!widget_list.isLocked());
		this->anchor_offset = offset;
		updateAnchoredPosition();
	}

	void Widget::setHorizontalSizePolicy(SizePolicy policy) {
		wAssert(!widget_list.isLocked());
		wAssert(this != widget_list.getRootWidget());
		this->horizontal_size_policy = policy;
	}

	void Widget::setVerticalSizePolicy(SizePolicy policy) {
		wAssert(!widget_list.isLocked());
		wAssert(this != widget_list.getRootWidget());
		this->vertical_size_policy = policy;
	}

	void Widget::setPosition(float x, float y) {
		wAssert(!widget_list.isLocked());
		transforms.setPosition(sf::Vector2f(x, y));
	}

	void Widget::setPosition(const sf::Vector2f& position) {
		wAssert(!widget_list.isLocked());
		transforms.setPosition(position);
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
		this->click_through = value;
	}

	void Widget::setFocusable(bool value) {
		wAssert(!widget_list.isLocked());
		this->is_focusable = value;
	}

	void Widget::setParentSilent(Widget* new_parent) {
		wAssert(!widget_list.isLocked());
		if (!new_parent) {
			return setParentSilent(widget_list.root_widget);
		}
		if (new_parent == this) {
			updateFullName();
			throw std::runtime_error("Cannot parent object to itself: " + full_name);
		}
		if (new_parent) {
			CompVector<Widget*> parent_chain = new_parent->getParentChain();
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

	void Widget::lockChildren() {
		wAssert(!widget_list.isLocked());
		children_locked = true;
	}

	void Widget::unlockChildren() {
		wAssert(!widget_list.isLocked());
		children_locked = false;
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

	void Widget::setRenderLayer(GlobalRenderLayer layer) {
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

	void Widget::setShader(sf::Shader* shader) {
		wAssert(!widget_list.isLocked());
		this->shader = shader;
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
		updateAnchoredPosition();
	}

	void Widget::setOriginInternal(const sf::Vector2f& origin) {
		wAssert(!widget_list.isLocked());
		setOriginInternal(origin.x, origin.y);
	}

	void Widget::setSizeInternal(const sf::Vector2f& size) {
		wAssert(!widget_list.isLocked());
		setSizeInternal(size.x, size.y);
	}

	void Widget::addChild(Widget* child) {
		wAssert(!widget_list.isLocked());
		wAssert(!children_locked);
		children.add(child);
		children_names.add(child->name, child);
		child->parent = this;
	}

	void Widget::removeChild(Widget* child) {
		wAssert(!widget_list.isLocked());
		wAssert(!children_locked);
		children.remove(child);
		children_names.remove(child->name, child);
		local_layers.erase(child);
	}

	void Widget::updateAnchoredPosition() {
		wAssert(!widget_list.isLocked());
		if (!parent) {
			return;
		}
		if (parent_anchor == Anchor::CUSTOM) {
			return;
		}
		sf::Vector2f parent_size = parent->getLocalBounds().getSize();
		sf::Vector2f anchored_pos = getPosition();
		anchored_pos = anchorToPos(parent_anchor, parent_size);
		setPosition(anchored_pos + anchor_offset);
	}

	void Widget::updateOrigin() {
		sf::Vector2f origin_pos = getOrigin();
		if (origin_anchor != Anchor::CUSTOM) {
			origin_pos = anchorToPos(origin_anchor, getSize());
		}
		setOriginInternal(origin_pos);
	}

	void Widget::update() {
		wAssert(!widget_list.isLocked());
		if (!visible) {
			return;
		}
		OnUpdate();
		updateAnchoredPosition();
		updateOrigin();
		internalUpdate();
	}

	void Widget::updateHorizontalSize() {
		sf::Vector2f new_pos = getPosition();
		sf::Vector2f new_size = getSize();
		if (horizontal_size_policy == SizePolicy::PARENT) {
			if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(parent)) {
				new_pos.x = container->getHorizontalPadding();
				new_size.x = container->getWidth() - container->getHorizontalPadding() * 2;
			} else {
				new_pos.x = 0.0f;
				new_size.x = parent->getWidth();
			}
		}
		setPosition(new_pos);
		setSizeInternal(new_size);
	}

	void Widget::updateVerticalSize() {
		sf::Vector2f new_pos = getPosition();
		sf::Vector2f new_size = getSize();
		if (vertical_size_policy == SizePolicy::PARENT) {
			if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(parent)) {
				new_pos.y = container->getVerticalPadding();
				new_size.y = container->getHeight() - container->getVerticalPadding() * 2;
			} else {
				new_pos.y = 0.0f;
				new_size.y = parent->getHeight();
			}
		}
		setPosition(new_pos);
		setSizeInternal(new_size);
	}

	void Widget::internalUpdate() { }


	void Widget::internalOnSetParent(Widget* parent) { }

	void Widget::internalOnLeftPress(const sf::Vector2f& pos) { }

	void Widget::internalOnRightPress(const sf::Vector2f& pos) { }

	void Widget::internalOnLeftRelease(const sf::Vector2f& pos) { }

	void Widget::internalOnRightRelease(const sf::Vector2f& pos) { }

	void Widget::internalProcessKeyboardEvent(const sf::Event& event) { }

	void Widget::internalProcessMouse(const sf::Vector2f& pos) { }

	void Widget::internalOnMouseEnter(const sf::Vector2f& pos) { }

	void Widget::internalOnMouseExit(const sf::Vector2f& pos) { }

	void Widget::internalOnFocused() { }

	void Widget::internalOnFocusLost() { }

	std::string Widget::calcFullName() const {
		CompVector<Widget*> parents = getParentChain();
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
			render_texture.create((unsigned int)texture_bounds.width, (unsigned int)texture_bounds.height);
		}
		sf::Transform global_transform = getGlobalTransform();
		sf::Transform combined(global_transform);
		sf::Vector2f render_position_offset = getRenderPositionOffset();
		combined.translate(render_position_offset);
		quantize_position(combined);
		render_view.setSize(texture_bounds.getSize());
		sf::Vector2f texture_bounds_center = texture_bounds.getPosition() + texture_bounds.getSize() / 2.0f;
		render_view.setCenter(texture_bounds_center);
		render_texture.setView(render_view);
		render_texture.clear(sf::Color::Transparent);
		sf::Transformable* transformable = getTransformable();
		wAssert(transformable);
		const sf::Transform& transformable_transform = transformable->getTransform();
		wAssert(transformable_transform == sf::Transform::Identity);
		wAssert(transformable->getOrigin() == sf::Vector2f());
		OnBeforeRender(render_texture);
		sf::Drawable* drawable = getDrawable();
		wAssert(drawable);
		render_texture.draw(*drawable, combined);
		OnAfterRender(render_texture);
		render_texture.display();
	}

	void Widget::render(sf::RenderTarget& target) {
		if (!visible) {
			return;
		}
		if (!unclipped_region.isQuantizedNonZero()) {
			return;
		}
		OnBeforeGlobalRender(target);
		updateRenderTexture(unclipped_region.getQuantized());
		sf::Sprite sprite = sf::Sprite(render_texture.getTexture());
		sprite.setPosition(unclipped_region.getQuantized().getPosition());
		if (shader) {
			target.draw(sprite, shader);
		} else {
			target.draw(sprite);
		}
		OnAfterGlobalRender(target);
	}

	void Widget::renderBounds(sf::RenderTarget& target, const sf::Color& color, bool include_children) {
		if (!visible) {
			return;
		}
		sf::FloatRect quantized_bounds = quantize_rect(
			getGlobalBounds(),
			QUANTIZE_MODE_FLOOR_SUBTRACT
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
		float offset = widget_list.render_origin_size;
		sf::Vector2f hoffset = sf::Vector2f(offset, 0.0f);
		sf::Vector2f voffset = sf::Vector2f(0.0f, offset);
		sf::Vector2f origin_pos = getGlobalOriginPosition();
		draw_line(target, origin_pos - hoffset, origin_pos + hoffset, widget_list.render_transform_origin_color);
		draw_line(target, origin_pos - voffset, origin_pos + voffset, widget_list.render_transform_origin_color);
		sf::Vector2f pos = getGlobalPosition();
		draw_line(target, pos - hoffset, pos + hoffset, widget_list.render_origin_color);
		draw_line(target, pos - voffset, pos + voffset, widget_list.render_origin_color);
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->renderOrigin(target);
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

}
