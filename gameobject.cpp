#include <numbers>
#include "gameobject.h"
#include "utils.h"

const auto tob2 = utils::tob2;
const auto tosf = utils::tosf;
const auto to2i = utils::to2i;
const auto to2f = utils::to2f;

GameObject::GameObject() { }

void GameObject::updateVisual() {
	b2Vec2 position = rigid_body->GetPosition();
	float angle = rigid_body->GetAngle();
	setVisualPosition(tosf(position));
	setVisualRotation(utils::to_degrees(angle));
}

void GameObject::setEnabled(bool enabled, bool include_children) {
	rigid_body->SetEnabled(enabled);
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->setEnabled(enabled, true);
		}
	}
}

void GameObject::setPosition(const b2Vec2& pos, bool move_children) {
	b2Vec2 offset = pos - rigid_body->GetPosition();
	rigid_body->SetTransform(pos, rigid_body->GetAngle());
	if (move_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->setPosition(children[i]->rigid_body->GetPosition() + offset, true);
		}
	}
}

void GameObject::setAngle(float angle, bool rotate_children) {
	float angle_offset = angle - rigid_body->GetAngle();
	rigid_body->SetTransform(rigid_body->GetPosition(), angle);
	if (rotate_children) {
		for (int i = 0; i < children.size(); i++) {
			b2Vec2 old_pos = children[i]->rigid_body->GetPosition();
			b2Vec2 rel_pos = old_pos - rigid_body->GetPosition();
			float old_child_angle = atan2(rel_pos.y, rel_pos.x);
			float new_child_angle = old_child_angle + angle_offset;
			float radius = rel_pos.Length();
			float new_x = cos(new_child_angle) * radius;
			float new_y = sin(new_child_angle) * radius;
			b2Vec2 new_rel_pos = b2Vec2(new_x, new_y);
			b2Vec2 new_pos = rigid_body->GetPosition() + new_rel_pos;
			children[i]->setPosition(new_pos, true);
			children[i]->setAngle(children[i]->rigid_body->GetAngle() + angle_offset, true);
		}
	}
}

void GameObject::setLinearVelocity(const b2Vec2& velocity, bool include_children) {
	rigid_body->SetLinearVelocity(velocity);
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->setLinearVelocity(velocity, true);
		}
	}
}

void GameObject::setAngularVelocity(float velocity, bool include_children) {
	rigid_body->SetAngularVelocity(velocity);
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->setAngularVelocity(velocity, true);
		}
	}
}

void GameObject::setType(b2BodyType type, bool include_children) {
	rigid_body->SetType(type);
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->setType(type, true);
		}
	}
}

void GameObject::setDensity(float density, bool include_children) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetDensity(density);
	}
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->setDensity(density, true);
		}
	}
}

void GameObject::setFriction(float friction, bool include_children) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetFriction(friction);
	}
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->setFriction(friction, true);
		}
	}
}

void GameObject::setRestitution(float restitution, bool include_children) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetRestitution(restitution);
	}
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->setRestitution(restitution, true);
		}
	}
}

ShapeObject::ShapeObject() { }

ShapeObject::ShapeObject(std::unique_ptr<sf::Shape> shape, b2Body* rigid_body) {
	this->shape = std::move(shape);
	this->rigid_body = rigid_body;
}

void ShapeObject::setVisualPosition(const sf::Vector2f& pos) {
	shape->setPosition(pos);
}

void ShapeObject::setVisualRotation(float angle) {
	shape->setRotation(angle);
}

sf::Drawable* ShapeObject::getDrawable() {
	return shape.get();
}

BoxObject::BoxObject(std::unique_ptr<sf::Shape> shape, b2Body* rigid_body)
	: ShapeObject(std::move(shape), rigid_body) { }

BallObject::BallObject(std::unique_ptr<CircleNotchShape> shape, b2Body* rigid_body) {
	this->circle_notch_shape = std::move(shape);
	this->rigid_body = rigid_body;
}

void BallObject::setVisualPosition(const sf::Vector2f& pos) {
	circle_notch_shape->setPosition(pos);
}

void BallObject::setVisualRotation(float angle) {
	circle_notch_shape->setRotation(angle);
}

sf::Drawable* BallObject::getDrawable() {
	return circle_notch_shape.get();
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
	std::unique_ptr<sf::ConvexShape> shape,
	b2Body* rigid_body,
	std::vector<GameObject*> wheels,
	std::vector<b2RevoluteJoint*> wheel_joints
) {
	this->shape = std::move(shape);
	this->rigid_body = rigid_body;
	this->children = wheels;
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

GroundObject::GroundObject(std::unique_ptr<LineStripShape> shape, b2Body* rigid_body) {
	this->line_strip_shape = std::move(shape);
	this->rigid_body = rigid_body;
}

void GroundObject::moveVertex(int index, const b2Vec2& new_pos) {
	std::vector<b2Vec2> vertices = getVertices();
	vertices[index] = new_pos;
	setVertices(vertices);
}

void GroundObject::tryDeleteVertex(int index) {
	if (index < 0) {
		return;
	}
	std::vector<b2Vec2> vertices = getVertices();
	if (vertices.size() <= 2) {
		return;
	}
	vertices.erase(vertices.begin() + index);
	setVertices(vertices);
}

void GroundObject::addVertex(int index, const b2Vec2& pos) {
	std::vector<b2Vec2> vertices = getVertices();
	vertices.insert(vertices.begin() + index, pos);
	setVertices(vertices);
}

sf::Drawable* GroundObject::getDrawable() {
	return line_strip_shape.get();
}

void GroundObject::setVisualPosition(const sf::Vector2f& pos) {
	line_strip_shape->setPosition(pos);
}

void GroundObject::setVisualRotation(float angle) {
	line_strip_shape->setRotation(angle);
}

std::vector<b2Vec2> GroundObject::getVertices() {
	b2Fixture* fixture = rigid_body->GetFixtureList();
	b2ChainShape* chain = static_cast<b2ChainShape*>(fixture->GetShape());
	std::vector<b2Vec2> vertices;
	vertices.assign(chain->m_vertices, chain->m_vertices + chain->m_count);
	return vertices;
}

void GroundObject::setVertices(const std::vector<b2Vec2>& vertices) {
	b2Fixture* old_fixture = rigid_body->GetFixtureList();
	rigid_body->DestroyFixture(old_fixture);
	b2ChainShape new_chain;
	new_chain.CreateChain(vertices.data(), vertices.size(), vertices.front(), vertices.back());
	b2Fixture* new_fixture = rigid_body->CreateFixture(&new_chain, 1.0f);
	line_strip_shape->varray = sf::VertexArray(sf::LinesStrip, vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		line_strip_shape->varray[i].position = tosf(vertices[i]);
	}
}
