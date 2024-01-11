#include <numbers>
#include "gameobject.h"
#include "utils.h"

GameObject::GameObject() { }

GameObject::GameObject(std::unique_ptr<sf::Drawable> drawable, b2Body* rigid_body) {
	this->transformable = dynamic_cast<sf::Transformable*>(drawable.get());
	assert(this->transformable != nullptr);
	this->drawable = std::move(drawable);
	this->rigid_body = rigid_body;
}

void GameObject::UpdateVisual() {
	if (rigid_body != nullptr) {
		b2Vec2 position = rigid_body->GetPosition();
		float angle = rigid_body->GetAngle();
		this->position = position;
		this->angle = angle;
	}
	transformable->setPosition(sf::Vector2f(position.x, -position.y));
	transformable->setRotation(-utils::to_degrees(angle));
}

void GameObject::SetType(b2BodyType type) {
	rigid_body->SetType(type);
}

void GameObject::SetDensity(float density) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetDensity(density);
	}
}

void GameObject::SetFriction(float friction) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetFriction(friction);
	}
}

void GameObject::SetRestitution(float restitution) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetRestitution(restitution);
	}
}

LineStripShape::LineStripShape(sf::VertexArray& varray) {
	this->varray = varray;
}

void LineStripShape::setLineColor(sf::Color color) {
	for (int i = 0; i < varray.getVertexCount(); i++) {
		varray[i].color = color;
	}
}

void LineStripShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(varray, states);
}
