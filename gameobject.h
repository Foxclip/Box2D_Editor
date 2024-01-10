#pragma once

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

class GameObject {
public:
	b2Vec2 position;
	float angle;
	std::unique_ptr<sf::Shape> shape;
	b2Body* rigid_body = nullptr;
	b2Fixture* fixture = nullptr;

	GameObject();
	GameObject(std::unique_ptr<sf::Shape> shape, b2Body* rigid_body, b2Fixture* fixture);
	void UpdateVisual();

private:

};