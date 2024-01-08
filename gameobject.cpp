#include <numbers>
#include "gameobject.h"

GameObject::GameObject() { }

GameObject::GameObject(std::unique_ptr<sf::Shape> shape, b2Body* rigid_body) {
	this->shape = std::move(shape);
	this->rigid_body = rigid_body;
}

void GameObject::UpdateVisual() {
	b2Vec2 position = rigid_body->GetPosition();
	float angle_rad = rigid_body->GetAngle();
	shape->setPosition(position.x, -position.y);
	float angle_deg = angle_rad * 180.0f / std::numbers::pi;
	shape->setRotation(angle_deg);
}

