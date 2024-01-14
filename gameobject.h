#pragma once

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

class LineStripShape : public sf::Drawable, public sf::Transformable {
public:
	explicit LineStripShape(sf::VertexArray& varray);
	void setLineColor(sf::Color color);
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	sf::VertexArray varray;
};

class CircleNotchShape : public sf::Drawable, public sf::Transformable {
public:
	explicit CircleNotchShape(float radius, int point_count, int notch_segment_count);
	void setCircleColor(sf::Color color);
	void setNotchColor(sf::Color color);
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	sf::VertexArray varray_circle;
	sf::VertexArray varray_notch;
};

class GameObject {
public:
	b2Vec2 position = b2Vec2(0.0f, 0.0f);
	float angle = 0.0f;
	std::unique_ptr<sf::Drawable> drawable;
	sf::Transformable* transformable = nullptr;
	b2Body* rigid_body = nullptr;

	GameObject();
	GameObject(std::unique_ptr<sf::Drawable> drawable, b2Body* rigid_body);
	void UpdateVisual();
	void SetType(b2BodyType type);
	void SetDensity(float density);
	void SetFriction(float friction);
	void SetRestitution(float restitution);

private:

};

class CarObject : public GameObject {
public:
	CarObject(
		std::unique_ptr<sf::Drawable> drawable,
		b2Body* rigid_body,
		std::vector<GameObject*> wheels,
		std::vector<b2RevoluteJoint*> wheel_joints
	);
	std::vector<GameObject*> wheels;
	std::vector<b2RevoluteJoint*> wheel_joints;
private:
};

