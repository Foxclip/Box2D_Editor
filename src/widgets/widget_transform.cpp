#include "../../include/widgets/widget_transform.h"
#include "../../include/widgets/widget.h"

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
