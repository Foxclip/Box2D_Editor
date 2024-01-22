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

void GameObject::render(sf::RenderTarget* target) {
	for (int i = 0; i < children.size(); i++) {
		children[i]->render(target);
	}
	updateVisual();
	target->draw(*getDrawable());
}

void GameObject::setVisualPosition(const sf::Vector2f& pos) {
	sf::Transformable* tr = getTransformable();
	tr->setPosition(pos);
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

std::string BoxObject::serialize(int indent_level) {
	std::string str;
	b2Fixture* fixture = rigid_body->GetFixtureList();
	std::string indent_str;
	for (int i = 0; i < indent_level; i++) {
		indent_str += "    ";
	}
	str += indent_str + "object box\n";
	str += indent_str + "    type " + utils::body_type_to_str(rigid_body->GetType()) + "\n";
	str += indent_str + "    size " + utils::vec_to_str(size) + "\n";
	str += indent_str + "    color " + utils::color_to_str(color) + "\n";
	str += indent_str + "    position " + utils::vec_to_str(rigid_body->GetPosition()) + "\n";
	str += indent_str + "    rotation " + std::to_string(rigid_body->GetAngle()) + "\n";
	str += indent_str + "    density " + std::to_string(fixture->GetDensity()) + "\n";
	str += indent_str + "    friction " + std::to_string(fixture->GetFriction()) + "\n";
	str += indent_str + "    restitution " + std::to_string(fixture->GetRestitution()) + "\n";
	return str;
}

std::unique_ptr<BoxObject> BoxObject::deserialize(TokensPointer& tp, b2World* world) {
	try {
		b2Vec2 position = b2Vec2(0.0f, 0.0f);
		float angle = 0.0f;
		b2Vec2 size = b2Vec2(1.0f, 1.0f);
		sf::Color color = sf::Color::White;
		float density = 1.0f, friction = 0.0f, restitution = 0.0f;
		b2BodyType type = b2_staticBody;
		while(tp.valid_range()) {
			std::string pname = tp.gets();
			if (pname == "type") {
				type = utils::str_to_body_type(tp.gets());
			} else if (pname == "size") {
				size = tp.getb2Vec2();
			} else if (pname == "color") {
				color = tp.getColor();
			} else if (pname == "position") {
				position = tp.getb2Vec2();
			} else if (pname == "rotation") {
				angle = tp.getf();
			} else if (pname == "density") {
				density = tp.getf();
			} else if (pname == "friction") {
				friction = tp.getf();
			} else if (pname == "restitution") {
				restitution = tp.getf();
			} else if (TokensPointer::isEntityName(pname)) {
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

BallObject::BallObject(b2World* world, b2Vec2 pos, float angle, float radius, sf::Color color, sf::Color notch_color) {
	b2BodyDef bodyDef;
	bodyDef.position = pos;
	bodyDef.angle = angle;
	rigid_body = world->CreateBody(&bodyDef);
	b2CircleShape circle_shape;
	circle_shape.m_radius = radius;
	b2Fixture* fixture = rigid_body->CreateFixture(&circle_shape, 1.0f);
	circle_notch_shape = std::make_unique<CircleNotchShape>(radius, 30, 4);
	circle_notch_shape->setCircleColor(color);
	circle_notch_shape->setNotchColor(notch_color);
	this->radius = radius;
	this->color = color;
	this->notch_color = notch_color;
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

sf::Drawable* BallObject::getDrawable() {
	return circle_notch_shape.get();
}

sf::Transformable* BallObject::getTransformable() {
	return circle_notch_shape.get();
}

std::string BallObject::serialize(int indent_level) {
	std::string str;
	b2Fixture* fixture = rigid_body->GetFixtureList();
	std::string indent_str;
	for (int i = 0; i < indent_level; i++) {
		indent_str += "    ";
	}
	str += indent_str + "object ball\n";
	str += indent_str + "    type " + utils::body_type_to_str(rigid_body->GetType()) + "\n";
	str += indent_str + "    radius " + std::to_string(radius) + "\n";
	str += indent_str + "    color " + utils::color_to_str(color) + "\n";
	str += indent_str + "    notch_color " + utils::color_to_str(notch_color) + "\n";
	str += indent_str + "    position " + utils::vec_to_str(rigid_body->GetPosition()) + "\n";
	str += indent_str + "    rotation " + std::to_string(rigid_body->GetAngle()) + "\n";
	str += indent_str + "    density " + std::to_string(fixture->GetDensity()) + "\n";
	str += indent_str + "    friction " + std::to_string(fixture->GetFriction()) + "\n";
	str += indent_str + "    restitution " + std::to_string(fixture->GetRestitution()) + "\n";
	return str;
}

std::unique_ptr<BallObject> BallObject::deserialize(TokensPointer& tp, b2World* world) {
	try {
		b2Vec2 position = b2Vec2(0.0f, 0.0f);
		float angle = 0.0f;
		float radius = 1.0f;
		sf::Color color = sf::Color::White;
		sf::Color notch_color = sf::Color(128, 128, 128);
		bool notch_color_set = false;
		float density = 1.0f, friction = 0.0f, restitution = 0.0f;
		b2BodyType type = b2_staticBody;
		while (tp.valid_range()) {
			std::string pname = tp.gets();
			if (pname == "type") {
				type = utils::str_to_body_type(tp.gets());
			} else if (pname == "radius") {
				radius = tp.getf();
			} else if (pname == "color") {
				color = tp.getColor();
				if (!notch_color_set) {
					notch_color = color;
				}
			} else if (pname == "notch_color") {
				notch_color = tp.getColor();
				notch_color_set = true;
			} else if (pname == "position") {
				position = tp.getb2Vec2();
			} else if (pname == "rotation") {
				angle = tp.getf();
			} else if (pname == "density") {
				density = tp.getf();
			} else if (pname == "friction") {
				friction = tp.getf();
			} else if (pname == "restitution") {
				restitution = tp.getf();
			} else if (TokensPointer::isEntityName(pname)) {
				tp.move(-1);
				break;
			} else {
				throw std::runtime_error("Unknown BallObject parameter name: " + pname);
			}
		}
		std::unique_ptr<BallObject> ball = std::make_unique<BallObject>(world, position, angle, radius, color, notch_color);
		ball->setType(type, false);
		ball->setDensity(density, false);
		ball->setFriction(friction, false);
		ball->setRestitution(restitution, false);
		return ball;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
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

CarObject::CarObject(b2World* world, b2Vec2 pos, float angle, std::vector<float> lengths, std::vector<float> wheels, sf::Color color) {
	convex_shape = std::make_unique<sf::ConvexShape>(lengths.size());
	float pi = std::numbers::pi;
	auto get_pos = [&](int i) {
		float angle = (float)i / lengths.size() * 2 * pi;
		b2Vec2 vector = b2Vec2(std::cos(angle), std::sin(angle));
		int index = i < lengths.size() ? i : i % lengths.size();
		b2Vec2 pos = lengths[index] * vector;
		return pos;
	};
	b2BodyDef bodyDef;
	bodyDef.position = pos;
	bodyDef.angle = angle;
	rigid_body = world->CreateBody(&bodyDef);
	for (int i = 0; i < lengths.size(); i++) {
		b2Vec2 vertices[3];
		vertices[0] = b2Vec2(0.0f, 0.0f);
		vertices[1] = get_pos(i);
		vertices[2] = get_pos(i + 1);
		b2PolygonShape triangle;
		triangle.Set(vertices, 3);
		b2Fixture* fixture = rigid_body->CreateFixture(&triangle, 1.0f);
		if (wheels[i] > 0.0f) {
			b2Vec2 anchor_pos = vertices[1];
			b2Vec2 anchor_pos_world = pos + anchor_pos;
			std::unique_ptr<BallObject> wheel = std::make_unique<BallObject>(
				world, anchor_pos_world, 0.0f, wheels[i], sf::Color::Yellow, sf::Color(64, 64, 0)
			);
			BallObject* wheel_ptr = wheel.get();
			wheel_ptr->setType(b2_dynamicBody, false);
			wheel_ptr->setDensity(1.0f, false);
			wheel_ptr->setFriction(0.3f, false);
			wheel_ptr->setRestitution(0.5f, false);
			children.push_back(std::move(wheel));
			b2RevoluteJointDef wheel_joint_def;
			wheel_joint_def.Initialize(rigid_body, wheel_ptr->rigid_body, anchor_pos_world);
			wheel_joint_def.maxMotorTorque = 30.0f;
			wheel_joint_def.motorSpeed = -10.0f;
			wheel_joint_def.enableMotor = true;
			b2RevoluteJoint* wheel_joint = (b2RevoluteJoint*)world->CreateJoint(&wheel_joint_def);
			wheel_joints.push_back(wheel_joint);
		}
		convex_shape->setPoint(i, tosf(vertices[1]));
	}

	convex_shape->setFillColor(color);
	this->lengths = lengths;
	this->wheels = wheels;
	this->color = color;
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

sf::Drawable* CarObject::getDrawable() {
	return convex_shape.get();
}

sf::Transformable* CarObject::getTransformable() {
	return convex_shape.get();
}

std::string CarObject::serialize(int indent_level) {
	std::string str;
	b2Fixture* fixture = rigid_body->GetFixtureList();
	std::string indent_str;
	for (int i = 0; i < indent_level; i++) {
		indent_str += "    ";
	}
	str += indent_str + "object car\n";
	str += indent_str + "    lengths " + utils::farr_to_str(lengths) + "\n";
	str += indent_str + "    wheels " + utils::farr_to_str(wheels) + "\n";
	str += indent_str + "    color " + utils::color_to_str(color) + "\n";
	str += indent_str + "    position " + utils::vec_to_str(rigid_body->GetPosition()) + "\n";
	str += indent_str + "    rotation " + std::to_string(rigid_body->GetAngle()) + "\n";
	str += indent_str + "    density " + std::to_string(fixture->GetDensity()) + "\n";
	str += indent_str + "    friction " + std::to_string(fixture->GetFriction()) + "\n";
	str += indent_str + "    restitution " + std::to_string(fixture->GetRestitution()) + "\n";
	return str;
}

std::unique_ptr<CarObject> CarObject::deserialize(TokensPointer& tp, b2World* world) {
	try {
		b2Vec2 position = b2Vec2(0.0f, 0.0f);
		float angle = 0.0f;
		std::vector<float> lengths;
		std::vector<float> wheels;
		sf::Color color = sf::Color::White;
		float density = 0.0f, friction = 0.0f, restitution = 0.0f;
		while (tp.valid_range()) {
			std::string pname = tp.gets();
			if (pname == "lengths") {
				lengths = tp.getfArr();
			} else if (pname == "wheels") {
				wheels = tp.getfArr();
			} else if (pname == "color") {
				color = tp.getColor();
			} else if (pname == "position") {
				position = tp.getb2Vec2();
			} else if (pname == "rotation") {
				angle = tp.getf();
			} else if (pname == "density") {
				density = tp.getf();
			} else if (pname == "friction") {
				friction = tp.getf();
			} else if (pname == "restitution") {
				restitution = tp.getf();
			} else if (TokensPointer::isEntityName(pname)) {
				tp.move(-1);
				break;
			} else {
				throw std::runtime_error("Unknown CarObject parameter name: " + pname);
			}
		}
		std::unique_ptr<CarObject> car = std::make_unique<CarObject>(world, position, angle, lengths, wheels, color);
		car->setType(b2_dynamicBody, false);
		car->setDensity(density, false);
		car->setFriction(friction, false);
		car->setRestitution(restitution, false);
		return car;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
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

GroundObject::GroundObject(b2World* world, b2Vec2 pos, std::vector<b2Vec2> vertices, sf::Color color) {
	b2BodyDef bodyDef;
	bodyDef.position = pos;
	rigid_body = world->CreateBody(&bodyDef);
	b2ChainShape chain;
	chain.CreateChain(vertices.data(), vertices.size(), vertices.front(), vertices.back());
	b2Fixture* fixture = rigid_body->CreateFixture(&chain, 1.0f);
	sf::VertexArray drawable_vertices(sf::LinesStrip, vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		drawable_vertices[i].position = tosf(vertices[i]);
	}
	line_strip_shape = std::make_unique<LineStripShape>(drawable_vertices);
	line_strip_shape->setLineColor(color);
	this->color = color;
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
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

std::string GroundObject::serialize(int indent_level) {
	std::string str;
	b2Fixture* fixture = rigid_body->GetFixtureList();
	std::string indent_str;
	for (int i = 0; i < indent_level; i++) {
		indent_str += "    ";
	}
	str += indent_str + "object ground\n";
	str += indent_str + "    vertices ";
	b2ChainShape* chain = getShape();
	for (int i = 0; i < chain->m_count; i++) {
		str += utils::vec_to_str(chain->m_vertices[i]);
		if (i < chain->m_count - 1) {
			str += " ";
		}
	}
	str += indent_str + "\n";
	str += indent_str + "    color " + utils::color_to_str(color) + "\n";
	str += indent_str + "    position " + utils::vec_to_str(rigid_body->GetPosition()) + "\n";
	str += indent_str + "    friction " + std::to_string(fixture->GetFriction()) + "\n";
	str += indent_str + "    restitution " + std::to_string(fixture->GetRestitution()) + "\n";
	return str;
}

std::unique_ptr<GroundObject> GroundObject::deserialize(TokensPointer& tp, b2World* world) {
	try {
		b2Vec2 position = b2Vec2(0.0f, 0.0f);
		float angle = 0.0f;
		std::vector<b2Vec2> vertices;
		sf::Color color = sf::Color::White;
		float friction = 0.0f, restitution = 0.0f;
		while (tp.valid_range()) {
			std::string pname = tp.gets();
			if (pname == "vertices") {
				while (!tp.fail()) {
					b2Vec2 vertex = tp.getb2Vec2();
					if (!tp.fail()) {
						vertices.push_back(vertex);
					}
				}
				tp.reset();
				tp.move(-1);
			} else if (pname == "color") {
				color = tp.getColor();
			} else if (pname == "position") {
				position = tp.getb2Vec2();
			} else if (pname == "friction") {
				friction = tp.getf();
			} else if (pname == "restitution") {
				restitution = tp.getf();
			} else if (TokensPointer::isEntityName(pname)) {
				tp.move(-1);
				break;
			} else {
				throw std::runtime_error("Unknown GroundObject parameter name: " + pname);
			}
		}
		std::unique_ptr<GroundObject> ground = std::make_unique<GroundObject>(world, position, vertices, color);
		ground->setDensity(1.0f, false);
		ground->setFriction(friction, false);
		ground->setRestitution(restitution, false);
		return ground;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
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

