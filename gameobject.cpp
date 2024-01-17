#include <numbers>
#include "gameobject.h"
#include "utils.h"

const auto tob2 = utils::tob2;
const auto tosf = utils::tosf;
const auto to2i = utils::to2i;
const auto to2f = utils::to2f;

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

void GameObject::SetEnabled(bool enabled, bool include_children) {
	rigid_body->SetEnabled(enabled);
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->SetEnabled(enabled, true);
		}
	}
}

void GameObject::SetPosition(const b2Vec2& pos, bool move_children) {
	b2Vec2 offset = pos - rigid_body->GetPosition();
	rigid_body->SetTransform(pos, rigid_body->GetAngle());
	if (move_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->SetPosition(children[i]->rigid_body->GetPosition() + offset, true);
		}
	}
}

void GameObject::SetAngle(float angle, bool rotate_children) {
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
			children[i]->SetPosition(new_pos, true);
			children[i]->SetAngle(children[i]->rigid_body->GetAngle() + angle_offset, true);
		}
	}
}

void GameObject::SetLinearVelocity(const b2Vec2& velocity, bool include_children) {
	rigid_body->SetLinearVelocity(velocity);
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->SetLinearVelocity(velocity, true);
		}
	}
}

void GameObject::SetAngularVelocity(float velocity, bool include_children) {
	rigid_body->SetAngularVelocity(velocity);
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->SetAngularVelocity(velocity, true);
		}
	}
}

void GameObject::SetType(b2BodyType type, bool include_children) {
	rigid_body->SetType(type);
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->SetType(type, true);
		}
	}
}

void GameObject::SetDensity(float density, bool include_children) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetDensity(density);
	}
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->SetDensity(density, true);
		}
	}
}

void GameObject::SetFriction(float friction, bool include_children) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetFriction(friction);
	}
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->SetFriction(friction, true);
		}
	}
}

void GameObject::SetRestitution(float restitution, bool include_children) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetRestitution(restitution);
	}
	if (include_children) {
		for (int i = 0; i < children.size(); i++) {
			children[i]->SetRestitution(restitution, true);
		}
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

GroundObject::GroundObject(std::unique_ptr<sf::Drawable> drawable, b2Body* rigid_body) {
	this->transformable = dynamic_cast<sf::Transformable*>(drawable.get());
	assert(this->transformable != nullptr);
	this->drawable = std::move(drawable);
	this->rigid_body = rigid_body;
}

void GroundObject::move_vertex(int index, b2Vec2 new_pos) {
	std::vector<b2Vec2> vertices = get_vertices();
	vertices[index] = new_pos;
	set_vertices(vertices);
}

void GroundObject::try_delete_vertex(int index) {
	if (index < 0) {
		return;
	}
	std::vector<b2Vec2> vertices = get_vertices();
	if (vertices.size() <= 2) {
		return;
	}
	vertices.erase(vertices.begin() + index);
	set_vertices(vertices);
}

std::vector<b2Vec2> GroundObject::get_vertices() {
	b2Fixture* fixture = rigid_body->GetFixtureList();
	b2ChainShape* chain = static_cast<b2ChainShape*>(fixture->GetShape());
	std::vector<b2Vec2> vertices;
	vertices.assign(chain->m_vertices, chain->m_vertices + chain->m_count);
	return vertices;
}

void GroundObject::set_vertices(const std::vector<b2Vec2>& vertices) {
	b2Fixture* old_fixture = rigid_body->GetFixtureList();
	rigid_body->DestroyFixture(old_fixture);
	b2ChainShape new_chain;
	new_chain.CreateChain(vertices.data(), vertices.size(), vertices.front(), vertices.back());
	b2Fixture* new_fixture = rigid_body->CreateFixture(&new_chain, 1.0f);
	LineStripShape* visual = static_cast<LineStripShape*>(drawable.get());
	visual->varray = sf::VertexArray(sf::LinesStrip, vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		visual->varray[i].position = tosf(vertices[i]);
	}
}
