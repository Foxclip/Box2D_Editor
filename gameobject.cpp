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
	transformable->setPosition(sf::Vector2f(position.x, position.y));
	transformable->setRotation(utils::to_degrees(angle));
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

CarObject::CarObject(
	std::unique_ptr<sf::Drawable> drawable,
	b2Body* rigid_body,
	std::vector<GameObject*> wheels,
	std::vector<b2WheelJoint*> wheel_joints
) {
	this->transformable = dynamic_cast<sf::Transformable*>(drawable.get());
	assert(this->transformable != nullptr);
	this->drawable = std::move(drawable);
	this->rigid_body = rigid_body;
	this->wheels = wheels;
	this->wheel_joints = wheel_joints;
}

CircleNotchShape::CircleNotchShape(float radius, int point_count, int notch_segment_count) {
	varray_circle = sf::VertexArray(sf::TriangleFan, point_count + 1);
	varray_notch = sf::VertexArray(sf::TriangleFan, notch_segment_count + 2);
	float angle_offset = 0.0f;
	varray_circle[0] = sf::Vertex(sf::Vector2f(0.0f, 0.0f));
	for (int i = 0; i < point_count; i++) {
		std::pair<float, float> pair = utils::getCircleVertex(i, point_count, radius, angle_offset);
		varray_circle[i] = sf::Vertex(sf::Vector2f(pair.first, pair.second));
	}
	varray_notch[0] = sf::Vertex(sf::Vector2f(0.0f, 0.0f));
	for (int i = 0; i < notch_segment_count + 1; i++) {
		std::pair<float, float> pair = utils::getCircleVertex(i, point_count, radius, angle_offset);
		varray_notch[i] = sf::Vertex(sf::Vector2f(pair.first, pair.second));
	}
}

void CircleNotchShape::setCircleColor(sf::Color color) {
	for (int i = 0; i < varray_circle.getVertexCount(); i++) {
		varray_circle[i].color = color;
	}
}

void CircleNotchShape::setNotchColor(sf::Color color) {
	for (int i = 0; i < varray_notch.getVertexCount(); i++) {
		varray_notch[i].color = color;
	}
}

void CircleNotchShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(varray_circle, states);
	target.draw(varray_notch, states);
}
