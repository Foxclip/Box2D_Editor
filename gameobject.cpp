#include <numbers>
#include "gameobject.h"
#include "utils.h"

const auto tob2 = utils::tob2;
const auto tosf = utils::tosf;

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
	transformable->setPosition(tosf(position));
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
	std::vector<b2RevoluteJoint*> wheel_joints
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
	float segment_angle = 2 * std::numbers::pi / (float)point_count;
	float notch_angle = segment_angle * notch_segment_count;
	float angle_offset = -notch_angle / 2.0f;
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

void GroundObject::move_vertex(int index, b2Vec2 new_pos) {
	b2Fixture* old_fixture = rigid_body->GetFixtureList();
	b2ChainShape* old_chain = static_cast<b2ChainShape*>(old_fixture->GetShape());
	std::vector<b2Vec2> vertices;
	vertices.assign(old_chain->m_vertices, old_chain->m_vertices + old_chain->m_count);
	vertices[index] = new_pos;
	rigid_body->DestroyFixture(old_fixture);
	b2ChainShape new_chain; 
	new_chain.CreateChain(vertices.data(), vertices.size(), vertices.front(), vertices.back());
	b2Fixture* new_fixture = rigid_body->CreateFixture(&new_chain, 1.0f);
	LineStripShape* visual = static_cast<LineStripShape*>(drawable.get());
	visual->varray[index].position = tosf(new_pos);
}

GroundObject::GroundObject(std::unique_ptr<sf::Drawable> drawable, b2Body* rigid_body) {
	this->transformable = dynamic_cast<sf::Transformable*>(drawable.get());
	assert(this->transformable != nullptr);
	this->drawable = std::move(drawable);
	this->rigid_body = rigid_body;
}
