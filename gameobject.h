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

class GameObject {
public:
	b2Vec2 position = b2Vec2(0.0f, 0.0f);
	float angle = 0.0f;
	std::unique_ptr<sf::Drawable> drawable;
	sf::Transformable* transformable = nullptr;

	GameObject();
	GameObject(std::unique_ptr<sf::Drawable> drawable, b2Body* rigid_body);
	void UpdateVisual();
	void SetType(b2BodyType type);
	void SetDensity(float density);
	void SetFriction(float friction);
	void SetRestitution(float restitution);

private:
	b2Body* rigid_body = nullptr;

};