#pragma once

#include <SFML/Graphics.hpp>

namespace fw {

	class Widget;

	class WidgetTransform {
	public:
		WidgetTransform(Widget* widget);
		const sf::Transform& getTransform() const;
		const sf::Transform& getInverseTransform() const;
		const sf::Transform& getGlobalTransform() const;
		const sf::Transform& getInverseGlobalTransform() const;
		const sf::Vector2f& getPosition() const;
		float getRotation() const;
		const sf::Vector2f& getScale() const;
		const sf::Vector2f& getOrigin() const;
		void invalidateTransform();
		void invalidateGlobalTransform();
		void setPosition(const sf::Vector2f& position);
		void setGlobalPosition(const sf::Vector2f& position);
		void setRotation(float angle);
		void setScale(const sf::Vector2f& scale);
		void setOrigin(float x, float y);
		void setOrigin(const sf::Vector2f origin);

	private:
		Widget* widget = nullptr;
		sf::Vector2f position;
		float rotation = 0.0f;
		sf::Vector2f scale = sf::Vector2f(1.0f, 1.0f);
		sf::Vector2f origin;
		mutable sf::Transform transform;
		mutable sf::Transform inv_transform;
		mutable sf::Transform global_transform;
		mutable sf::Transform inv_global_transform;
		mutable bool transform_valid = false;
		mutable bool inv_transform_valid = false;
		mutable bool global_transform_valid = false;
		mutable bool inv_global_transform_valid = false;

		void recalcTransform() const;
		void recalcInverseTransform() const;
		void recalcGlobalTransform() const;
		void recalcInverseGlobalTransform() const;

	};

}
