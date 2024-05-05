#include "widgets/widget_transform.h"
#include "widgets/widget.h"
#include <numbers>

namespace fw {

	WidgetTransform::WidgetTransform(Widget* widget) {
		this->widget = widget;
	}

	const sf::Transform& WidgetTransform::getTransform() const {
		if (!transform_valid) {
			recalcTransform();
		}
		return transform;
	}

	const sf::Transform& WidgetTransform::getTransformOffset() const {
		if (!transform_offset_valid) {
			recalcTransformOffset();
		}
		return transform_offset;
	}

	const sf::Transform& WidgetTransform::getInverseTransform() const {
		if (!inv_transform_valid) {
			recalcInverseTransform();
		}
		return inv_transform;
	}

	const sf::Transform& WidgetTransform::getGlobalTransform() const {
		if (!global_transform_valid) {
			recalcGlobalTransform();
		}
		return global_transform;
	}

	const sf::Transform& WidgetTransform::getGlobalTransformOffset() const {
		if (!global_transform_offset_valid) {
			recalcGlobalTransformOffset();
		}
		return global_transform_offset;
	}

	const sf::Transform& WidgetTransform::getInverseGlobalTransform() const {
		if (!inv_global_transform_valid) {
			recalcInverseGlobalTransform();
		}
		return inv_global_transform;
	}

	const sf::Vector2f& WidgetTransform::getPosition() const {
		return position;
	}

	sf::Vector2f WidgetTransform::getOffsetPosition() const {
		return position - origin;
		//float x = transform_offset.getMatrix()[12];
		//float y = transform_offset.getMatrix()[13];
		//return sf::Vector2f(x, y);
	}

	float WidgetTransform::getRotation() const {
		return rotation;
	}

	const sf::Vector2f& WidgetTransform::getScale() const {
		return scale;
	}

	const sf::Vector2f& WidgetTransform::getOrigin() const {
		return origin;
	}

	void WidgetTransform::invalidateTransform() {
		transform_valid = false;
		inv_transform_valid = false;
		invalidateTransformOffset();
		invalidateGlobalTransform();
	}

	void WidgetTransform::invalidateTransformOffset() {
		transform_offset_valid = false;
		global_transform_offset_valid = false;
	}

	void WidgetTransform::invalidateGlobalTransform() {
		global_transform_valid = false;
		global_transform_offset_valid = false;
		inv_global_transform_valid = false;
		for (size_t i = 0; i < widget->getChildren().size(); i++) {
			Widget* child = widget->getChildren()[i];
			child->transforms.invalidateGlobalTransform();
		}
	}

	void WidgetTransform::setPosition(const sf::Vector2f& position) {
		this->position = position;
		invalidateTransform();
	}

	void WidgetTransform::setRotation(float angle) {
		this->rotation = angle;
		invalidateTransform();
	}

	void WidgetTransform::setScale(const sf::Vector2f& scale) {
		this->scale = scale;
		invalidateTransform();
	}

	void WidgetTransform::setOrigin(float x, float y) {
		this->origin.x = x;
		this->origin.y = y;
		invalidateTransformOffset();
	}

	void WidgetTransform::setOrigin(const sf::Vector2f origin) {
		setOrigin(origin.x, origin.y);
	}

	void WidgetTransform::recalcTransform() const {
		float angle  = (float)(-rotation * std::numbers::pi / 180.f);
		float cosine = std::cos(angle);
		float sine   = std::sin(angle);
		float sxc    = scale.x * cosine;
		float syc    = scale.y * cosine;
		float sxs    = scale.x * sine;
		float sys    = scale.y * sine;
		transform = sf::Transform(
			sxc, sys, position.x,
			-sxs, syc, position.y,
			0.f, 0.f, 1.f
		);
		transform_valid = true;
	}

	void WidgetTransform::recalcTransformOffset() const {
		float angle  = (float)(-rotation * std::numbers::pi / 180.f);
		float cosine = std::cos(angle);
		float sine   = std::sin(angle);
		float sxc    = scale.x * cosine;
		float syc    = scale.y * cosine;
		float sxs    = scale.x * sine;
		float sys    = scale.y * sine;
		float tx     = -origin.x * sxc - origin.y * sys + position.x;
		float ty     =  origin.x * sxs - origin.y * syc + position.y;
		transform_offset = sf::Transform(
			sxc, sys, tx,
			-sxs, syc, ty,
			0.f, 0.f, 1.f
		);
		transform_offset_valid = true;
	}

	void WidgetTransform::recalcInverseTransform() const {
		inv_transform = getTransform().getInverse();
		inv_transform_valid = true;
	}

	void WidgetTransform::recalcGlobalTransform() const {
		const sf::Transform& transform = getTransform();
		const sf::Transform& parent_transform = widget->getParentGlobalTransform();
		global_transform = parent_transform * transform;
		global_transform_valid = true;
	}

	void WidgetTransform::recalcGlobalTransformOffset() const {
		const sf::Transform& transform_offset = getTransformOffset();
		const sf::Transform& parent_transform = widget->getParentGlobalTransform();
		global_transform_offset = parent_transform * transform_offset;
		global_transform_offset_valid = true;
	}

	void WidgetTransform::recalcInverseGlobalTransform() const {
		const sf::Transform& transform = getTransform();
		const sf::Transform& global_transform = getGlobalTransform();
		inv_global_transform = global_transform.getInverse();
		inv_global_transform_valid = true;
	}

}
