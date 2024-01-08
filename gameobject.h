#pragma once

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

class GameObject {
public:
	std::unique_ptr<sf::Shape> shape;
	b2Body* rigid_body = nullptr;

	GameObject();
	GameObject(std::unique_ptr<sf::Shape> shape, b2Body* rigid_body);
	void UpdateVisual();

private:

};