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
	b2Body* rigid_body = nullptr;
	std::vector<GameObject*> children;
	GameObject* parent = nullptr;

	GameObject();
	virtual sf::Drawable* getDrawable() = 0;
	virtual sf::Transformable* getTransformable() = 0;
	void updateVisual();
	void setVisualPosition(const sf::Vector2f& pos);
	void setVisualRotation(float angle);
	void setEnabled(bool enabled, bool include_children);
	void setPosition(const b2Vec2& pos, bool move_children);
	void setAngle(float angle, bool rotate_children);
	void setLinearVelocity(const b2Vec2& velocity, bool include_children);
	void setAngularVelocity(float velocity, bool include_children);
	void setType(b2BodyType type, bool include_children);
	void setDensity(float density, bool include_children);
	void setFriction(float friction, bool include_children);
	void setRestitution(float restitution, bool include_children);
	virtual std::string serialize() = 0;

private:

};

class BoxObject : public GameObject {
public:
	BoxObject(std::unique_ptr<sf::RectangleShape> shape, b2Body* rigid_body);
	sf::Drawable* getDrawable();
	sf::Transformable* getTransformable();
	std::string serialize();
	b2Vec2 size = b2Vec2();
private:
	std::unique_ptr<sf::RectangleShape> rect_shape;
};

class BallObject : public GameObject {
public:
	BallObject(std::unique_ptr<CircleNotchShape> shape, b2Body* rigid_body);
	sf::Drawable* getDrawable();
	sf::Transformable* getTransformable();
	std::string serialize();
	float radius = 0.0f;
private:
	std::unique_ptr<CircleNotchShape> circle_notch_shape;
};

class CarObject : public GameObject {
public:
	CarObject(
		std::unique_ptr<sf::ConvexShape> shape,
		b2Body* rigid_body,
		std::vector<GameObject*> wheels,
		std::vector<b2RevoluteJoint*> wheel_joints
	);
	sf::Drawable* getDrawable();
	sf::Transformable* getTransformable();
	std::string serialize();
	std::vector<float> lengths;
	std::vector<float> wheels;
private:
	std::unique_ptr<sf::ConvexShape> convex_shape;
	std::vector<b2RevoluteJoint*> wheel_joints;
};

class GroundObject : public GameObject {
public:
	GroundObject(std::unique_ptr<LineStripShape> shape, b2Body* rigid_body);
	void moveVertex(int index, const b2Vec2& new_pos);
	void tryDeleteVertex(int index);
	void addVertex(int index, const b2Vec2& pos);
	sf::Drawable* getDrawable();
	sf::Transformable* getTransformable();
	b2ChainShape* getShape();
	std::string serialize();
private:
	std::unique_ptr<LineStripShape> line_strip_shape;
	std::vector<b2Vec2> getVertices();
	void setVertices(const std::vector<b2Vec2>& vertices);
};

