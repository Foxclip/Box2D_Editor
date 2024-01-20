#include <numbers>
#include "gameobject.h"
#include "utils.h"

const auto tob2 = utils::tob2;
const auto tosf = utils::tosf;
const auto to2i = utils::to2i;
const auto to2f = utils::to2f;

GameObject::GameObject() { }

GameObject::~GameObject() {
	if (rigid_body) {
		rigid_body->GetWorld()->DestroyBody(rigid_body);
		rigid_body = nullptr;
	}
}

void GameObject::updateVisual() {
	b2Vec2 position = rigid_body->GetPosition();
	float angle = rigid_body->GetAngle();
	setVisualPosition(tosf(position));
	setVisualRotation(utils::to_degrees(angle));
}

void GameObject::setVisualPosition(const sf::Vector2f& pos) {
	getTransformable()->setPosition(pos);
}

void GameObject::setVisualRotation(float angle) {
	getTransformable()->setRotation(angle);
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

BoxObject::BoxObject(b2World* world, b2Vec2 pos, float angle, b2Vec2 size, sf::Color color) {
	b2BodyDef bodyDef;
	bodyDef.position = pos;
	bodyDef.angle = angle;
	rigid_body = world->CreateBody(&bodyDef);
	b2PolygonShape box_shape;
	box_shape.SetAsBox(size.x / 2.0f, size.y / 2.0f);
	b2Fixture* fixture = rigid_body->CreateFixture(&box_shape, 1.0f);
	rect_shape = std::make_unique<sf::RectangleShape>(tosf(size));
	rect_shape->setOrigin(size.x / 2.0f, size.y / 2.0f);
	rect_shape->setFillColor(color);
	this->size = size;
	this->color = color;
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

sf::Drawable* BoxObject::getDrawable() {
	return rect_shape.get();
}

sf::Transformable* BoxObject::getTransformable() {
	return rect_shape.get();
}

std::string BoxObject::serialize() {
	std::string str;
	b2Fixture* fixture = rigid_body->GetFixtureList();
	str += "object box\n";
	str += "    type " + utils::body_type_to_str(rigid_body->GetType()) + "\n";
	str += "    size " + utils::vec_to_str(size) + "\n";
	str += "    color " + utils::color_to_str(color) + "\n";
	str += "    position " + utils::vec_to_str(rigid_body->GetPosition()) + "\n";
	str += "    rotation " + std::to_string(rigid_body->GetAngle()) + "\n";
	str += "    density " + std::to_string(fixture->GetDensity()) + "\n";
	str += "    friction " + std::to_string(fixture->GetFriction()) + "\n";
	str += "    restitution " + std::to_string(fixture->GetRestitution()) + "\n";
	return str;
}

std::unique_ptr<GameObject> BoxObject::deserialize(TokensPointer& tp, b2World* world) {
	try {
		b2Vec2 position = b2Vec2(0.0f, 0.0f);
		float angle = 0.0f;
		b2Vec2 size = b2Vec2(1.0f, 1.0f);
		sf::Color color = sf::Color::White;
		float density = 1.0f, friction = 0.0f, restitution = 0.0f;
		b2BodyType type = b2_staticBody;;
		while(tp.valid()) {
			std::string pname = tp.gets();
			if (pname == "type") {
				type = utils::str_to_body_type(tp.gets());
			} else if (pname == "size") {
				size.x = tp.getf();
				size.y = tp.getf();
			} else if (pname == "color") {
				color.r = tp.geti();
				color.g = tp.geti();
				color.b = tp.geti();
			} else if (pname == "position") {
				position.x = tp.getf();
				position.y = tp.getf();
			} else if (pname == "rotation") {
				angle = tp.getf();
			} else if (pname == "density") {
				density = tp.getf();
			} else if (pname == "friction") {
				friction = tp.getf();
			} else if (pname == "restitution") {
				restitution = tp.getf();
			} else if (pname == "object") {
				tp.move(-1);
				break;
			} else {
				throw std::runtime_error("Unknown BoxObject parameter name: " + pname);
			}
		}
		std::unique_ptr<BoxObject> box = std::make_unique<BoxObject>(world, position, angle, size, color);
		box->setType(type, false);
		box->setDensity(density, false);
		box->setFriction(friction, false);
		box->setRestitution(restitution, false);
		return box;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

BallObject::BallObject(std::unique_ptr<CircleNotchShape> shape, b2Body* rigid_body) {
	this->circle_notch_shape = std::move(shape);
	this->rigid_body = rigid_body;
}

sf::Drawable* BallObject::getDrawable() {
	return circle_notch_shape.get();
}

sf::Transformable* BallObject::getTransformable() {
	return circle_notch_shape.get();
}

std::string BallObject::serialize() {
	std::string str;
	b2Fixture* fixture = rigid_body->GetFixtureList();
	str += "object ball\n";
	str += "    type " + utils::body_type_to_str(rigid_body->GetType()) + "\n";
	str += "    radius " + std::to_string(radius) + "\n";
	str += "    color " + utils::color_to_str(color) + "\n";
	str += "    position " + utils::vec_to_str(rigid_body->GetPosition()) + "\n";
	str += "    rotation " + std::to_string(rigid_body->GetAngle()) + "\n";
	str += "    density " + std::to_string(fixture->GetDensity()) + "\n";
	str += "    friction " + std::to_string(fixture->GetFriction()) + "\n";
	str += "    restitution " + std::to_string(fixture->GetRestitution()) + "\n";
	return str;
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
	this->convex_shape = std::move(shape);
	this->rigid_body = rigid_body;
	for (int i = 0; i < wheels.size(); i++) {
		wheels[i]->parent = this;
	}
	this->children = wheels;
	this->wheel_joints = wheel_joints;
}

sf::Drawable* CarObject::getDrawable() {
	return convex_shape.get();
}

sf::Transformable* CarObject::getTransformable() {
	return convex_shape.get();
}

std::string CarObject::serialize() {
	return std::string();
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

sf::Transformable* GroundObject::getTransformable() {
	return line_strip_shape.get();
}

b2ChainShape* GroundObject::getShape() {
	return static_cast<b2ChainShape*>(rigid_body->GetFixtureList()->GetShape());
}

std::string GroundObject::serialize() {
	std::string str;
	b2Fixture* fixture = rigid_body->GetFixtureList();
	str += "object ground\n";
	str += "    vertices ";
	b2ChainShape* chain = getShape();
	for (int i = 0; i < chain->m_count; i++) {
		str += utils::vec_to_str(chain->m_vertices[i]);
		if (i < chain->m_count - 1) {
			str += " ";
		}
	}
	str += "\n";
	str += "    color " + utils::color_to_str(color) + "\n";
	str += "    friction " + std::to_string(fixture->GetFriction()) + "\n";
	str += "    restitution " + std::to_string(fixture->GetRestitution()) + "\n";
	return str;
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

