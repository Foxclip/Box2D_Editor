#pragma once

#include "global.h"

class Widget;

class WidgetTransform {
public:
	WidgetTransform(Widget* widget);
	const sf::Transform& getTransform() const;
	const sf::Transform& getGlobalTransform() const;
	const sf::Transform& getInverseGlobalTransform() const;
	const sf::Vector2f& getPosition() const;
	float getRotation() const;
	const sf::Vector2f& getScale() const;
	void invalidateGlobalTransform();
	void setPosition(const sf::Vector2f& position);
	void setRotation(float angle);
	void setScale(const sf::Vector2f& scale);

private:
	Widget* widget = nullptr;
	mutable sf::Transform global_transform;
	mutable sf::Transform inv_global_transform;
	mutable bool global_transform_valid = false;
	mutable bool inv_global_transform_valid = false;

	void recalcGlobalTransform() const;
	void recalcInverseGlobalTransform() const;

};
