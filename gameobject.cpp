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

TokenWriter& GameObject::serializeBody(TokenWriter& tw, b2Body* body) {
	tw << "body" << "\n";
	tw.addIndentLevel(1);
	tw.writeStringParam("type", utils::body_type_to_str(body->GetType()));
	tw.writeb2Vec2Param("position", body->GetPosition());
	tw.writeFloatParam("angle", body->GetAngle());
	tw.writeb2Vec2Param("linear_velocity", body->GetLinearVelocity());
	tw.writeFloatParam("angular_velocity", body->GetAngularVelocity());
	tw << "fixtures" << "\n";
	tw.addIndentLevel(1);
	for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		serializeFixture(tw, fixture) << "\n";
	}
	tw.addIndentLevel(-1);
	tw << "/fixtures" << "\n";
	tw.addIndentLevel(-1);
	tw << "/body";
	return tw;
}

BodyDef GameObject::deserializeBody(TokenReader& tr) {
	try {
		BodyDef result;
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "type") {
				result.body_def.type = utils::str_to_body_type(tr.readString());
			} else if (pname == "position") {
				result.body_def.position = tr.readb2Vec2();
			} else if (pname == "angle") {
				result.body_def.angle = tr.readFloat();
			} else if (pname == "linear_velocity") {
				result.body_def.linearVelocity = tr.readb2Vec2();
			} else if (pname == "angular_velocity") {
				result.body_def.angularVelocity = tr.readFloat();
			} else if (pname == "fixtures") {
				while (tr.validRange()) {
					std::string str = tr.readString();
					if (str == "/fixtures") {
						break;
					} else if (str == "fixture") {
						b2FixtureDef fixture_def = deserializeFixture(tr);
						result.fixture_defs.push_back(fixture_def);
					} else {
						throw std::runtime_error("Expected fixture, got: \"" + str + "\"");
					}
				}
			} else if (pname == "/body") {
				break;
			} else {
				throw std::runtime_error("Unknown b2Body parameter name: " + pname);
			}
		}
		return result;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

TokenWriter& GameObject::serializeFixture(TokenWriter& tw, b2Fixture* fixture) {
	tw << "fixture" << "\n";
	tw.addIndentLevel(1);
	tw.writeFloatParam("density", fixture->GetDensity());
	tw.writeFloatParam("friction", fixture->GetFriction());
	tw.writeFloatParam("restitution", fixture->GetRestitution());
	tw.addIndentLevel(-1);
	tw << "/fixture";
	return tw;
}

b2FixtureDef GameObject::deserializeFixture(TokenReader& tr) {
	try {
		b2FixtureDef def;
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "density") {
				def.density = tr.readFloat();
			} else if (pname == "friction") {
				def.friction = tr.readFloat();
			} else if (pname == "restitution") {
				def.restitution = tr.readFloat();
			} else if (pname == "/fixture") {
				break;
			} else {
				throw std::runtime_error("Unknown fixture parameter name: " + pname);
			}
		}
		return def;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

TokenWriter& GameObject::serializeJoint(TokenWriter& tw, b2Joint* p_joint) {
	try {
		if (b2RevoluteJoint* joint = dynamic_cast<b2RevoluteJoint*>(p_joint)) {
			return serializeRevoluteJoint(tw, joint);
		} else {
			throw std::runtime_error("Unknown joint type");
		}
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

TokenWriter& GameObject::serializeRevoluteJoint(TokenWriter& tw, b2RevoluteJoint* joint) {
	tw << "joint revolute" << "\n";
	tw.addIndentLevel(1);
	tw.writeb2Vec2Param("anchor_a", joint->GetLocalAnchorA());
	tw.writeb2Vec2Param("anchor_b", joint->GetLocalAnchorB());
	tw.writeBoolParam("collide_connected", joint->GetCollideConnected());
	tw.writeFloatParam("angle_lower_limit", joint->GetLowerLimit());
	tw.writeFloatParam("angle_upper_limit", joint->GetUpperLimit());
	tw.writeBoolParam("angle_limit_enabled", joint->IsLimitEnabled());
	tw.writeFloatParam("motor_max_torque", joint->GetMaxMotorTorque());
	tw.writeFloatParam("motor_speed", joint->GetMotorSpeed());
	tw.writeBoolParam("motor_enabled", joint->IsMotorEnabled());
	tw.addIndentLevel(-1);
	tw << "/joint";
	return tw;
}

b2RevoluteJointDef GameObject::deserializeRevoluteJoint(TokenReader& tr) {
	try {
		b2RevoluteJointDef def;
		def.bodyA = nullptr;
		def.bodyB = nullptr;
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "anchor_a") {
				def.localAnchorA = tr.readb2Vec2();
			} else if (pname == "anchor_b") {
				def.localAnchorB = tr.readb2Vec2();
			} else if (pname == "collide_connected") {
				def.collideConnected = tr.readBool();
			} else if (pname == "angle_lower_limit") {
				def.lowerAngle = tr.readFloat();
			} else if (pname == "angle_upper_limit") {
				def.upperAngle = tr.readFloat();
			} else if (pname == "angle_limit_enabled") {
				def.enableLimit = tr.readBool();
			} else if (pname == "motor_max_torque") {
				def.maxMotorTorque = tr.readFloat();
			} else if (pname == "motor_speed") {
				def.motorSpeed = tr.readFloat();
			} else if (pname == "motor_enabled") {
				def.enableMotor = tr.readBool();
			} else if (pname == "/joint") {
				break;
			} else {
				throw std::runtime_error("Unknown RevoluteJoint parameter name: " + pname);
			}
		}
		return def;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

BoxObject::BoxObject(b2World* world, b2BodyDef def, b2Vec2 size, sf::Color color) {
	rigid_body = world->CreateBody(&def);
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

TokenWriter& BoxObject::serialize(TokenWriter& tw) {
	tw << "object box" << "\n";
	tw.addIndentLevel(1);
	tw.writeb2Vec2Param("size", size);
	tw.writeColorParam("color", color);
	serializeBody(tw, rigid_body) << "\n";
	tw.addIndentLevel(-1);
	tw << "/object";
	return tw;
}

std::unique_ptr<BoxObject> BoxObject::deserialize(TokenReader& tr, b2World* world) {
	try {
		b2Vec2 size = b2Vec2(1.0f, 1.0f);
		sf::Color color = sf::Color::White;
		BodyDef body_def;
		while(tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "size") {
				size = tr.readb2Vec2();
			} else if (pname == "color") {
				color = tr.readColor();
			} else if (pname == "body") {
				body_def = deserializeBody(tr);
			} else if (pname == "/object") {
				break;
			} else {
				throw std::runtime_error("Unknown BoxObject parameter name: " + pname);
			}
		}
		b2BodyDef bdef = body_def.body_def;
		b2FixtureDef fdef = body_def.fixture_defs.front();
		std::unique_ptr<BoxObject> box = std::make_unique<BoxObject>(world, bdef, size, color);
		box->setDensity(fdef.density, false);
		box->setFriction(fdef.friction, false);
		box->setRestitution(fdef.restitution, false);
		return box;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

BallObject::BallObject(b2World* world, b2BodyDef def, float radius, sf::Color color, sf::Color notch_color) {
	rigid_body = world->CreateBody(&def);
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

TokenWriter& BallObject::serialize(TokenWriter& tw) {
	tw << "object ball" << "\n";
	tw.addIndentLevel(1);
	tw.writeFloatParam("radius", radius);
	tw.writeColorParam("color", color);
	tw.writeColorParam("notch_color", notch_color);
	serializeBody(tw, rigid_body) << "\n";
	tw.addIndentLevel(-1);
	tw << "/object";
	return tw;
}

std::unique_ptr<BallObject> BallObject::deserialize(TokenReader& tr, b2World* world) {
	try {
		float radius = 1.0f;
		sf::Color color = sf::Color::White;
		sf::Color notch_color = sf::Color(128, 128, 128);
		bool notch_color_set = false;
		BodyDef body_def;
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "radius") {
				radius = tr.readFloat();
			} else if (pname == "color") {
				color = tr.readColor();
				if (!notch_color_set) {
					notch_color = color;
				}
			} else if (pname == "notch_color") {
				notch_color = tr.readColor();
				notch_color_set = true;
			} else if (pname == "body") {
				body_def = deserializeBody(tr);
			} else if (pname == "/object") {
				break;
			} else {
				throw std::runtime_error("Unknown BallObject parameter name: " + pname);
			}
		}
		b2BodyDef bdef = body_def.body_def;
		b2FixtureDef fdef = body_def.fixture_defs.front();
		std::unique_ptr<BallObject> ball = std::make_unique<BallObject>(world, bdef, radius, color, notch_color);
		ball->setDensity(fdef.density, false);
		ball->setFriction(fdef.friction, false);
		ball->setRestitution(fdef.restitution, false);
		return ball;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

LineStripShape::LineStripShape() { }

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

CarObject::CarObject(b2World* world, b2BodyDef def, std::vector<float> lengths, std::vector<float> wheels, sf::Color color) {
	rigid_body = world->CreateBody(&def);
	create_shape(lengths);
	for (int i = 0; i < wheels.size(); i++) {
		if (wheels[i] > 0.0f) {
			b2Vec2 wheel_pos = utils::get_pos(lengths, i);
			create_wheel(wheel_pos, wheels[i]);
		}
	}
	convex_shape->setFillColor(color);
	this->lengths = lengths;
	this->color = color;
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

CarObject::CarObject(
	b2World* world,
	b2BodyDef def,
	std::vector<float> lengths,
	std::vector<std::unique_ptr<BallObject>> wheels,
	std::vector<b2RevoluteJointDef> joint_defs,
	sf::Color color
) {
	rigid_body = world->CreateBody(&def);
	create_shape(lengths);
	for (int i = 0; i < joint_defs.size(); i++) {
		b2RevoluteJointDef def = joint_defs[i];
		def.bodyA = rigid_body;
		def.bodyB = wheels[i]->rigid_body;
		b2RevoluteJoint* joint = static_cast<b2RevoluteJoint*>(world->CreateJoint(&def));
		wheel_joints.push_back(joint);
	}
	for (int i = 0; i < wheels.size(); i++) {
		children.push_back(std::move(wheels[i]));
	}
	convex_shape->setFillColor(color);
	this->lengths = lengths;
	this->color = color;
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

sf::Drawable* CarObject::getDrawable() {
	return convex_shape.get();
}

sf::Transformable* CarObject::getTransformable() {
	return convex_shape.get();
}

TokenWriter& CarObject::serialize(TokenWriter& tw) {
	tw << "object car" << "\n";
	tw.addIndentLevel(1);
	tw.writeFloatArrParam("lengths", lengths);
	tw.writeColorParam("color", color);
	serializeBody(tw, rigid_body) << "\n";
	tw << "wheels" << "\n";
	tw.addIndentLevel(1);
	for (int i = 0; i < children.size(); i++) {
		children[i]->serialize(tw) << "\n";
		serializeJoint(tw, wheel_joints[i]) << "\n";
	}
	tw.addIndentLevel(-1);
	tw << "/wheels" << "\n";
	tw.addIndentLevel(-1);
	tw << "/object";
	return tw;
}

std::unique_ptr<CarObject> CarObject::deserialize(TokenReader& tr, b2World* world) {
	try {
		sf::Color color = sf::Color::White;
		std::vector<float> lengths;
		BodyDef body_def;
		std::vector<std::unique_ptr<BallObject>> wheels;
		std::vector<b2RevoluteJointDef> joint_defs;
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "lengths") {
				lengths = tr.readFloatArr();
			} else if (pname == "color") {
				color = tr.readColor();
			} else if (pname == "body") {
				body_def = deserializeBody(tr);
			} else if (pname == "wheels") {
				while (tr.validRange()) {
					std::string str = tr.readString();
					if (str == "object") {
					} else if (str == "/wheels") {
						break;
					} else {
						throw std::runtime_error("Expected wheel, got: \"" + str + "\"");
					}
					tr.eat("ball");
					std::unique_ptr<BallObject> wheel = BallObject::deserialize(tr, world);
					wheels.push_back(std::move(wheel));
					tr.eat("joint");
					tr.eat("revolute");
					b2RevoluteJointDef def = deserializeRevoluteJoint(tr);
					joint_defs.push_back(def);
				}
			} else if (pname == "/object") {
				break;
			} else {
				throw std::runtime_error("Unknown CarObject parameter name: " + pname);
			}
		}
		b2BodyDef bdef = body_def.body_def;
		b2FixtureDef fdef = body_def.fixture_defs.front();
		std::unique_ptr<CarObject> car = std::make_unique<CarObject>(world, bdef, lengths, std::move(wheels), joint_defs, color);
		car->setDensity(fdef.density, false);
		car->setFriction(fdef.friction, false);
		car->setRestitution(fdef.restitution, false);
		return car;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

void CarObject::create_shape(std::vector<float> lengths) {
	convex_shape = std::make_unique<sf::ConvexShape>(lengths.size());
	for (int i = 0; i < lengths.size(); i++) {
		b2Vec2 vertices[3];
		vertices[0] = b2Vec2(0.0f, 0.0f);
		vertices[1] = utils::get_pos(lengths, i);
		vertices[2] = utils::get_pos(lengths, i + 1);
		b2PolygonShape triangle;
		triangle.Set(vertices, 3);
		b2Fixture* fixture = rigid_body->CreateFixture(&triangle, 1.0f);
		convex_shape->setPoint(i, tosf(vertices[1]));
	}
}

void CarObject::create_wheel(b2Vec2 wheel_pos, float radius) {
	b2Vec2 anchor_pos = wheel_pos;
	b2Vec2 anchor_pos_world = rigid_body->GetPosition() + anchor_pos;
	b2BodyDef wheel_body_def;
	wheel_body_def.type = b2_dynamicBody;
	wheel_body_def.position = anchor_pos_world;
	std::unique_ptr<BallObject> wheel = std::make_unique<BallObject>(
		rigid_body->GetWorld(), wheel_body_def, radius, sf::Color::Yellow, sf::Color(64, 64, 0)
	);
	BallObject* wheel_ptr = wheel.get();
	wheel_ptr->setDensity(1.0f, false);
	wheel_ptr->setFriction(0.3f, false);
	wheel_ptr->setRestitution(0.5f, false);
	children.push_back(std::move(wheel));
	b2RevoluteJointDef wheel_joint_def;
	wheel_joint_def.Initialize(rigid_body, wheel_ptr->rigid_body, anchor_pos_world);
	wheel_joint_def.maxMotorTorque = 30.0f;
	wheel_joint_def.motorSpeed = -10.0f;
	wheel_joint_def.enableMotor = true;
	b2RevoluteJoint* wheel_joint = (b2RevoluteJoint*)rigid_body->GetWorld()->CreateJoint(&wheel_joint_def);
	wheel_joints.push_back(wheel_joint);
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

GroundObject::GroundObject(b2World* world, b2BodyDef def, std::vector<b2Vec2> p_vertices, sf::Color color) {
	rigid_body = world->CreateBody(&def);
	sf::VertexArray drawable_vertices(sf::LinesStrip, p_vertices.size());
	for (int i = 0; i < p_vertices.size(); i++) {
		drawable_vertices[i].position = tosf(p_vertices[i]);
		GroundVertex gv(p_vertices[i]);
		vertices.push_back(gv);
	}
	line_strip_shape = std::make_unique<LineStripShape>(drawable_vertices);
	syncVertices();
	line_strip_shape->setLineColor(color);
	this->color = color;
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

void GroundObject::moveVertices(const std::vector<int>& index_list, const b2Vec2& offset) {
	for (int i = 0; i < index_list.size(); i++) {
		int index = index_list[i];
		GroundVertex vertex = vertices[index];
		vertex.pos += offset;
		vertex.orig_pos = vertex.pos;
	}
	syncVertices();
}

void GroundObject::offsetSelected(const b2Vec2& offset) {
	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i].selected) {
			vertices[i].pos = vertices[i].orig_pos + offset;
		}
	}
	syncVertices();
}

void GroundObject::saveOffsets() {
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].orig_pos = vertices[i].pos;
	}
}

int GroundObject::getVertexCount() {
	return vertices.size();
}

const GroundVertex& GroundObject::getVertex(int index) {
	return vertices[index];
}

b2Vec2 GroundObject::getVertexPos(int index) {
	return vertices[index].pos;
}

void GroundObject::setVertexPos(int index, const b2Vec2& new_pos) {
	vertexSet(index, new_pos);
	syncVertices();
}

bool GroundObject::tryDeleteVertex(int index) {
	if (index < 0) {
		return false;
	}
	if (vertices.size() <= 2) {
		return false;
	}
	vertices.erase(vertices.begin() + index);
	syncVertices();
	return true;
}

void GroundObject::addVertex(int index, const b2Vec2& pos) {
	vertices.insert(vertices.begin() + index, pos);
	syncVertices();
}

void GroundObject::selectVertex(int index) {
	vertices[index].selected = true;
}

bool GroundObject::isVertexSelected(int index) {
	return vertices[index].selected;
}

void GroundObject::clearVertexSelection() {
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].selected = false;
	}
}

sf::Drawable* GroundObject::getDrawable() {
	return line_strip_shape.get();
}

sf::Transformable* GroundObject::getTransformable() {
	return line_strip_shape.get();
}

TokenWriter& GroundObject::serialize(TokenWriter& tw) {
	tw << "object ground" << "\n";
	tw.addIndentLevel(1);
	tw << "vertices";
	b2ChainShape* chain = getShape();
	for (int i = 0; i < chain->m_count; i++) {
		tw << chain->m_vertices[i];
	}
	tw << "\n";
	tw.writeColorParam("color", color);
	serializeBody(tw, rigid_body) << "\n";
	tw.addIndentLevel(-1);
	tw << "/object";
	return tw;
}

std::unique_ptr<GroundObject> GroundObject::deserialize(TokenReader& tr, b2World* world) {
	try {
		std::vector<b2Vec2> vertices;
		sf::Color color = sf::Color::White;
		BodyDef body_def;
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "vertices") {
				vertices = tr.readb2Vec2Arr();
			} else if (pname == "color") {
				color = tr.readColor();
			} else if (pname == "body") {
				body_def = deserializeBody(tr);
			} else if (pname == "/object") {
				break;
			} else {
				throw std::runtime_error("Unknown GroundObject parameter name: " + pname);
			}
		}
		b2BodyDef bdef = body_def.body_def;
		b2FixtureDef fdef = body_def.fixture_defs.front();
		std::unique_ptr<GroundObject> ground = std::make_unique<GroundObject>(world, bdef, vertices, color);
		ground->setDensity(fdef.density, false);
		ground->setFriction(fdef.friction, false);
		ground->setRestitution(fdef.restitution, false);
		return ground;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

b2ChainShape* GroundObject::getShape() {
	return static_cast<b2ChainShape*>(rigid_body->GetFixtureList()->GetShape());
}

std::vector<b2Vec2> GroundObject::getPositions() {
	std::vector<b2Vec2> b2vertices(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		b2vertices[i] = vertices[i].pos;
	}
	return b2vertices;
}

void GroundObject::vertexSet(int index, const b2Vec2& new_pos) {
	GroundVertex vertex = vertices[index];
	vertex.pos = new_pos;
	vertex.orig_pos = vertex.pos;
}

void GroundObject::syncVertices() {
	std::vector<b2Vec2> b2vertices = getPositions();
	b2Fixture* old_fixture = rigid_body->GetFixtureList();
	if (old_fixture) {
		rigid_body->DestroyFixture(old_fixture);
	}
	b2ChainShape new_chain;
	new_chain.CreateChain(b2vertices.data(), b2vertices.size(), b2vertices.front(), b2vertices.back());
	b2Fixture* new_fixture = rigid_body->CreateFixture(&new_chain, 1.0f);
	line_strip_shape->varray = sf::VertexArray(sf::LinesStrip, b2vertices.size());
	for (int i = 0; i < b2vertices.size(); i++) {
		line_strip_shape->varray[i].position = tosf(b2vertices[i]);
	}
}

GroundVertex::GroundVertex(b2Vec2 pos) {
	this->pos = pos;
	this->orig_pos = pos;
	this->selected = false;
}
