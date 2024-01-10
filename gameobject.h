#pragma once

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

class GameObject {
public:
	b2Vec2 position;
	float angle;
	std::unique_ptr<sf::Shape> shape;

	GameObject();
	GameObject(std::unique_ptr<sf::Shape> shape, b2Body* rigid_body);
	void UpdateVisual();
	void SetType(b2BodyType type);
	void SetDensity(float density);
	void SetFriction(float friction);
	void SetRestitution(float restitution);

private:
	b2Body* rigid_body = nullptr;

};