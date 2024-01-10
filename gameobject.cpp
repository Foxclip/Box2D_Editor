#include <numbers>
#include "gameobject.h"
#include "utils.h"

GameObject::GameObject() { }

GameObject::GameObject(std::unique_ptr<sf::Shape> shape, b2Body* rigid_body, b2Fixture* fixture) {
	this->shape = std::move(shape);
	this->rigid_body = rigid_body;
	this->fixture = fixture;
}

void GameObject::UpdateVisual() {
	b2Vec2 position = rigid_body->GetPosition();
	float angle = rigid_body->GetAngle();
	shape->setPosition(position.x, -position.y);
	shape->setRotation(-utils::to_degrees(angle));
}
