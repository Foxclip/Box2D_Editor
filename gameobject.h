#pragma once

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

class LineStripShape : public sf::Drawable, public sf::Transformable {
public:
	explicit LineStripShape(sf::VertexArray& varray);
	void setLineColor(sf::Color color);
	sf::VertexArray varray;
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
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
	std::vector<GameObject*> children;

	GameObject();
	GameObject(std::unique_ptr<sf::Drawable> drawable, b2Body* rigid_body);
	void UpdateVisual();
	void SetEnabled(bool enabled, bool include_children);
	void SetPosition(const b2Vec2& pos, bool move_children);
	void SetAngle(float angle, bool rotate_children);
	void SetLinearVelocity(const b2Vec2& velocity, bool include_children);
	void SetAngularVelocity(float velocity, bool include_children);
	void SetType(b2BodyType type, bool include_children);
	void SetDensity(float density, bool include_children);
	void SetFriction(float friction, bool include_children);
	void SetRestitution(float restitution, bool include_children);

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
	std::vector<b2RevoluteJoint*> wheel_joints;
private:
};

class GroundObject : public GameObject {
public:
	GroundObject(std::unique_ptr<sf::Drawable> drawable, b2Body* rigid_body);
	void move_vertex(int index, b2Vec2 new_pos);
	void try_delete_vertex(int index);
private:
	std::vector<b2Vec2> get_vertices();
	void set_vertices(const std::vector<b2Vec2>& vertices);
};

