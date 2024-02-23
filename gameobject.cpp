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

const b2Vec2& GameObject::getPosition() const {
	return rigid_body->GetPosition();
}

float GameObject::getRotation() const {
	return rigid_body->GetAngle();
}

GameObject* GameObject::getParent() const {
	return parent;
}

std::vector<GameObject*> GameObject::getParentChain() const {
	const GameObject* cur_obj = this;
	std::vector<GameObject*> result;
	while (cur_obj) {
		if (cur_obj->parent) {
			result.push_back(cur_obj->parent);
			cur_obj = cur_obj->parent;
		} else {
			break;
		}
	}
	return result;
}

const std::vector<std::unique_ptr<GameObject>>& GameObject::getChildren() const {
	return children;
}

b2Vec2 GameObject::toGlobal(const b2Vec2& pos) {
	return rigid_body->GetWorldPoint(pos);
}

b2Vec2 GameObject::toLocal(const b2Vec2& pos) {
	return rigid_body->GetLocalPoint(pos);
}

void GameObject::addChild(std::unique_ptr<GameObject> child) {
	child->parent = this;
	children.push_back(std::move(child));
}

void GameObject::removeChild(size_t index) {
	children.erase(children.begin() + index);
}

bool GameObject::removeChild(GameObject* object) {
	for (size_t i = 0; i < children.size(); i++) {
		if (children[i].get() == object) {
			removeChild(i);
			return true;
		}
	}
	return false;
}

void GameObject::updateVisual() {
	b2Vec2 position = rigid_body->GetPosition();
	float angle = rigid_body->GetAngle();
	setVisualPosition(tosf(position));
	setVisualRotation(utils::to_degrees(angle));
}

void GameObject::render(sf::RenderTarget& target) {
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->render(target);
	}
	updateVisual();
	target.draw(*getDrawable());
}

void GameObject::renderMask(sf::RenderTarget& mask, bool include_children) {
	if (include_children) {
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->renderMask(mask, true);
		}
	}
	updateVisual();
	drawMask(mask);
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
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->setEnabled(enabled, true);
		}
	}
}

void GameObject::setPosition(const b2Vec2& pos, bool move_children) {
	b2Vec2 offset = pos - rigid_body->GetPosition();
	rigid_body->SetTransform(pos, rigid_body->GetAngle());
	if (move_children) {
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->setPosition(children[i]->rigid_body->GetPosition() + offset, true);
		}
	}
}

void GameObject::setAngle(float angle, bool rotate_children) {
	float angle_offset = angle - rigid_body->GetAngle();
	rigid_body->SetTransform(rigid_body->GetPosition(), angle);
	if (rotate_children) {
		for (size_t i = 0; i < children.size(); i++) {
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
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->setLinearVelocity(velocity, true);
		}
	}
}

void GameObject::setAngularVelocity(float velocity, bool include_children) {
	rigid_body->SetAngularVelocity(velocity);
	if (include_children) {
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->setAngularVelocity(velocity, true);
		}
	}
}

void GameObject::setType(b2BodyType type, bool include_children) {
	rigid_body->SetType(type);
	if (include_children) {
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->setType(type, true);
		}
	}
}

void GameObject::setDensity(float density, bool include_children) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetDensity(density);
	}
	if (include_children) {
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->setDensity(density, true);
		}
	}
}

void GameObject::setFriction(float friction, bool include_children) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetFriction(friction);
	}
	if (include_children) {
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->setFriction(friction, true);
		}
	}
}

void GameObject::setRestitution(float restitution, bool include_children) {
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetRestitution(restitution);
	}
	if (include_children) {
		for (size_t i = 0; i < children.size(); i++) {
			children[i]->setRestitution(restitution, true);
		}
	}
}

void GameObject::moveVertices(const std::vector<size_t>& index_list, const b2Vec2& offset) {
	for (size_t i = 0; i < index_list.size(); i++) {
		size_t index = index_list[i];
		EditableVertex vertex = vertices[index];
		vertex.pos += offset;
		vertex.orig_pos = vertex.pos;
	}
	syncVertices();
}

void GameObject::offsetVertex(size_t index, const b2Vec2& offset, bool sync) {
	vertices[index].pos = vertices[index].orig_pos + offset;
	if (sync) {
		syncVertices();
	}
}

void GameObject::offsetSelected(const b2Vec2& offset, bool sync) {
	for (size_t i = 0; i < vertices.size(); i++) {
		if (vertices[i].selected) {
			vertices[i].pos = vertices[i].orig_pos + offset;
		}
	}
	if (sync) {
		syncVertices();
	}
}

void GameObject::saveOffsets() {
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].orig_pos = vertices[i].pos;
	}
}

size_t GameObject::indexLoop(ptrdiff_t index) {
	return utils::neg_mod(index, (ptrdiff_t)getVertexCount());
}

size_t GameObject::getVertexCount() {
	return vertices.size();
}

size_t GameObject::getEdgeCount() {
	if (isClosed()) {
		return vertices.size();
	} else {
		return vertices.size() - 1;
	}
}

const EditableVertex& GameObject::getVertex(size_t index) {
	return vertices[index];
}

b2Vec2 GameObject::getGlobalVertexPos(size_t index) {
	return rigid_body->GetWorldPoint(vertices[index].pos);
}

void GameObject::setGlobalVertexPos(size_t index, const b2Vec2& new_pos) {
	b2Vec2 local_pos = rigid_body->GetLocalPoint(new_pos);
	vertexSet(index, local_pos);
	syncVertices();
}

bool GameObject::tryDeleteVertex(ptrdiff_t index) {
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

void GameObject::addVertexGlobal(size_t index, const b2Vec2& pos) {
	b2Vec2 local_pos = rigid_body->GetLocalPoint(pos);
	vertices.insert(vertices.begin() + index, local_pos);
	syncVertices();
}

void GameObject::selectVertex(size_t index) {
	vertices[index].selected = true;
}

bool GameObject::isVertexSelected(size_t index) {
	return vertices[index].selected;
}

void GameObject::selectAllVertices() {
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].selected = true;
	}
}

void GameObject::deselectAllVertices() {
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].selected = false;
	}
}

std::vector<b2Vec2> GameObject::getPositions() {
	std::vector<b2Vec2> b2vertices(vertices.size());
	for (size_t i = 0; i < vertices.size(); i++) {
		b2vertices[i] = vertices[i].pos;
	}
	return b2vertices;
}

void GameObject::vertexSet(size_t index, const b2Vec2& new_pos) {
	EditableVertex& vertex = vertices[index];
	vertex.pos = new_pos;
	vertex.orig_pos = vertex.pos;
}

void GameObject::destroyFixtures() {
	std::vector<b2Fixture*> fixtures;
	for (b2Fixture* fixture = rigid_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixtures.push_back(fixture);
	}
	for (size_t i = 0; i < fixtures.size(); i++) {
		rigid_body->DestroyFixture(fixtures[i]);
	}
}

TokenWriter& GameObject::serializeBody(TokenWriter& tw, b2Body* body) {
	tw << "body" << "\n";
	{
		TokenWriterIndent body_indent(tw);
		tw.writeStringParam("type", utils::body_type_to_str(body->GetType()));
		tw.writeb2Vec2Param("position", body->GetPosition());
		tw.writeFloatParam("angle", body->GetAngle());
		tw.writeb2Vec2Param("linear_velocity", body->GetLinearVelocity());
		tw.writeFloatParam("angular_velocity", body->GetAngularVelocity());
		tw << "fixtures" << "\n";
		{
			TokenWriterIndent fixtures_indent(tw);
			for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
				serializeFixture(tw, fixture) << "\n";
			}
		}
		tw << "/fixtures" << "\n";
	}
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
	{
		TokenWriterIndent fixture_indent(tw);
		tw.writeFloatParam("density", fixture->GetDensity());
		tw.writeFloatParam("friction", fixture->GetFriction());
		tw.writeFloatParam("restitution", fixture->GetRestitution());
	}
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

GameObject* GameObject::getGameobject(b2Body* body) {
	return reinterpret_cast<GameObject*>(body->GetUserData().pointer);
}

BoxObject::BoxObject(b2World* world, b2BodyDef def, b2Vec2 size, sf::Color color) {
	this->color = color;
	rigid_body = world->CreateBody(&def);
	vertices.push_back(EditableVertex(0.5f * size));
	syncVertices();
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

bool BoxObject::isClosed() {
	return false;
}

sf::Drawable* BoxObject::getDrawable() {
	return rect_shape.get();
}

sf::Transformable* BoxObject::getTransformable() {
	return rect_shape.get();
}

void BoxObject::drawMask(sf::RenderTarget& mask) {
	sf::Color orig_color = rect_shape->getFillColor();
	rect_shape->setFillColor(sf::Color::White);
	mask.draw(*rect_shape);
	rect_shape->setFillColor(orig_color);
}

TokenWriter& BoxObject::serialize(TokenWriter& tw) const {
	tw << "object" << id << "box" << "\n";
	{
		TokenWriterIndent box_indent(tw);
		tw.writeb2Vec2Param("size", size);
		tw.writeColorParam("color", color);
		serializeBody(tw, rigid_body) << "\n";
	}
	tw << "/object";
	return tw;
}

std::unique_ptr<BoxObject> BoxObject::deserialize(TokenReader& tr, b2World* world) {
	try {
		b2Vec2 size = b2Vec2(1.0f, 1.0f);
		sf::Color color = sf::Color::White;
		BodyDef body_def;
		if (tr.tryEat("object")) {
			tr.eat("box");
		}
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

void BoxObject::syncVertices() {
	size = b2Vec2(abs(vertices.front().pos.x) * 2.0f, abs(vertices.front().pos.y) * 2.0f);
	b2Fixture* old_fixture = rigid_body->GetFixtureList();
	if (old_fixture) {
		rigid_body->DestroyFixture(old_fixture);
	}
	b2PolygonShape box_shape;
	box_shape.SetAsBox(size.x / 2.0f, size.y / 2.0f);
	b2Fixture* fixture = rigid_body->CreateFixture(&box_shape, 1.0f);
	rect_shape = std::make_unique<sf::RectangleShape>(tosf(size));
	rect_shape->setOrigin(size.x / 2.0f, size.y / 2.0f);
	rect_shape->setFillColor(color);
}

BallObject::BallObject(b2World* world, b2BodyDef def, float radius, sf::Color color, sf::Color notch_color) {
	this->radius = radius;
	this->color = color;
	this->notch_color = notch_color;
	rigid_body = world->CreateBody(&def);
	vertices.push_back(EditableVertex(b2Vec2(radius, 0.0f)));
	syncVertices();
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

bool BallObject::isClosed() {
	return false;
}

sf::Drawable* BallObject::getDrawable() {
	return circle_notch_shape.get();
}

sf::Transformable* BallObject::getTransformable() {
	return circle_notch_shape.get();
}

void BallObject::drawMask(sf::RenderTarget& mask) {
	circle_notch_shape->drawMask(mask);
}

TokenWriter& BallObject::serialize(TokenWriter& tw) const {
	tw << "object" << id << "ball" << "\n";
	{
		TokenWriterIndent ball_indent(tw);
		tw.writeFloatParam("radius", radius);
		tw.writeColorParam("color", color);
		tw.writeColorParam("notch_color", notch_color);
		serializeBody(tw, rigid_body) << "\n";
	}
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
		if (tr.tryEat("object")) {
			tr.eat("ball");
		}
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

void BallObject::syncVertices() {
	radius = vertices.front().pos.Length();
	b2Fixture* old_fixture = rigid_body->GetFixtureList();
	if (old_fixture) {
		rigid_body->DestroyFixture(old_fixture);
	}
	b2CircleShape circle_shape;
	circle_shape.m_radius = radius;
	b2Fixture* fixture = rigid_body->CreateFixture(&circle_shape, 1.0f);
	circle_notch_shape = std::make_unique<CircleNotchShape>(radius, 30, 4);
	circle_notch_shape->setCircleColor(color);
	circle_notch_shape->setNotchColor(notch_color);
}

LineStripShape::LineStripShape() { }

LineStripShape::LineStripShape(sf::VertexArray& varray) {
	this->varray = varray;
}

void LineStripShape::setLineColor(sf::Color color) {
	for (size_t i = 0; i < varray.getVertexCount(); i++) {
		varray[i].color = color;
	}
	line_color = color;
}

void LineStripShape::drawMask(sf::RenderTarget& mask, sf::RenderStates states) {
	states.transform *= getTransform();
	sf::Color orig_color = line_color;
	setLineColor(sf::Color::White);
	mask.draw(varray, states);
	setLineColor(orig_color);
}

void LineStripShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(varray, states);
}

CarObject::CarObject(
	b2World* world,
	std::vector<std::unique_ptr<Joint>>& joints,
	b2BodyDef def,
	std::vector<float> lengths,
	std::vector<float> wheels,
	sf::Color color
) {
	this->color = color;
	this->lengths = lengths;
	rigid_body = world->CreateBody(&def);
	for (size_t i = 0; i < lengths.size(); i++) {
		b2Vec2 pos = utils::get_pos<b2Vec2>(lengths, i);
		vertices.push_back(pos);
		if (wheels[i] > 0.0f) {
			create_wheel(pos, wheels[i], joints);
		}
	}
	polygon = std::make_unique<PolygonObject>();
	syncVertices();
	polygon->setFillColor(color);
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

CarObject::CarObject(
	b2World* world,
	b2BodyDef def,
	std::vector<float> lengths,
	sf::Color color
) {
	this->color = color;
	this->lengths = lengths;
	rigid_body = world->CreateBody(&def);
	for (size_t i = 0; i < lengths.size(); i++) {
		b2Vec2 pos = utils::get_pos<b2Vec2>(lengths, i);
		vertices.push_back(pos);
	}
	polygon = std::make_unique<PolygonObject>();
	syncVertices();
	polygon->setFillColor(color);
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

bool CarObject::isClosed() {
	return true;
}

PolygonObject* CarObject::getPolygonObject() const {
	return polygon.get();
}

sf::Drawable* CarObject::getDrawable() {
	return polygon.get();
}

sf::Transformable* CarObject::getTransformable() {
	return polygon.get();
}

void CarObject::render(sf::RenderTarget& target) {
	polygon->draw_varray = draw_varray;
	GameObject::render(target);
}

void CarObject::drawMask(sf::RenderTarget& mask) {
	sf::Color orig_color = polygon->getFillColor();
	polygon->setFillColor(sf::Color::White);
	mask.draw(*polygon);
	polygon->setFillColor(orig_color);
}

TokenWriter& CarObject::serialize(TokenWriter& tw) const {
	tw << "object" << id << "car" << "\n";
	{
		TokenWriterIndent car_indent(tw);
		tw.writeFloatArrParam("lengths", lengths);
		tw.writeColorParam("color", color);
		serializeBody(tw, rigid_body) << "\n";
	}
	tw << "/object";
	return tw;
}

std::unique_ptr<CarObject> CarObject::deserialize(TokenReader& tr, b2World* world) {
	try {
		sf::Color color = sf::Color::White;
		std::vector<float> lengths;
		BodyDef body_def;
		std::vector<std::unique_ptr<BallObject>> wheels;
		if (tr.tryEat("object")) {
			tr.eat("car");
		}
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "lengths") {
				lengths = tr.readFloatArr();
			} else if (pname == "color") {
				color = tr.readColor();
			} else if (pname == "body") {
				body_def = deserializeBody(tr);
			} else if (pname == "/object") {
				break;
			} else {
				throw std::runtime_error("Unknown CarObject parameter name: " + pname);
			}
		}
		b2BodyDef bdef = body_def.body_def;
		b2FixtureDef fdef = body_def.fixture_defs.front();
		std::unique_ptr<CarObject> car = std::make_unique<CarObject>(world, bdef, lengths, color);
		car->setDensity(fdef.density, false);
		car->setFriction(fdef.friction, false);
		car->setRestitution(fdef.restitution, false);
		return car;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

void CarObject::create_wheel(b2Vec2 wheel_pos, float radius, std::vector<std::unique_ptr<Joint>>& joints) {
	b2Vec2 anchor_pos = wheel_pos;
	b2Vec2 anchor_pos_world = rigid_body->GetPosition() + anchor_pos;
	b2BodyDef wheel_body_def;
	wheel_body_def.type = b2_dynamicBody;
	wheel_body_def.position = anchor_pos_world;
	std::unique_ptr<BallObject> wheel = std::make_unique<BallObject>(
		rigid_body->GetWorld(), wheel_body_def, radius, sf::Color(255, 255, 0), sf::Color(64, 64, 0)
	);
	BallObject* wheel_ptr = wheel.get();
	wheel_ptr->setDensity(1.0f, false);
	wheel_ptr->setFriction(0.3f, false);
	wheel_ptr->setRestitution(0.5f, false);
	addChild(std::move(wheel));
	b2RevoluteJointDef wheel_joint_def;
	wheel_joint_def.Initialize(rigid_body, wheel_ptr->rigid_body, anchor_pos_world);
	wheel_joint_def.maxMotorTorque = 30.0f;
	wheel_joint_def.motorSpeed = -10.0f;
	wheel_joint_def.enableMotor = true;
	std::unique_ptr<RevoluteJoint> joint = std::make_unique<RevoluteJoint>(
		wheel_joint_def, rigid_body->GetWorld(), this, wheel_ptr
	);
	joints.push_back(std::move(joint));
}

void CarObject::syncVertices() {
	polygon->resetVarray(vertices.size());
	for (size_t i = 0; i < vertices.size(); i++) {
		polygon->setPoint(i, tosf(vertices[i].pos));
	}
	polygon->recut();
	destroyFixtures();
	std::vector<PolygonObject> convex_polygons = polygon->getConvexPolygons();
	for (size_t polygon_i = 0; polygon_i < convex_polygons.size(); polygon_i++) {
		PolygonObject& polygon = convex_polygons[polygon_i];
		std::vector<b2Vec2> b2points;
		for (size_t vertex_i = 0; vertex_i < polygon.getPointCount(); vertex_i++) {
			sf::Vector2f point = polygon.getPoint(vertex_i);
			point = point + polygon.getPosition();
			b2points.push_back(tob2(point));
		}
		b2PolygonShape b2polygon;
		bool valid = b2polygon.Set(b2points.data(), b2points.size());
		if (valid) {
			rigid_body->CreateFixture(&b2polygon, 1.0f);
		}
	}
}

CircleNotchShape::CircleNotchShape(float radius, size_t point_count, size_t notch_segment_count) {
	varray_circle = sf::VertexArray(sf::TriangleFan, point_count + 1);
	varray_notch = sf::VertexArray(sf::TriangleFan, notch_segment_count + 2);
	float segment_angle = 2 * std::numbers::pi / (float)point_count;
	float notch_angle = segment_angle * notch_segment_count;
	float angle_offset = -notch_angle / 2.0f;
	varray_circle[0] = sf::Vertex(sf::Vector2f(0.0f, 0.0f));
	for (size_t i = 0; i < point_count; i++) {
		std::pair<float, float> pair = utils::getCircleVertex(i, point_count, radius, angle_offset);
		varray_circle[i] = sf::Vertex(sf::Vector2f(pair.first, pair.second));
	}
	varray_notch[0] = sf::Vertex(sf::Vector2f(0.0f, 0.0f));
	for (size_t i = 0; i < notch_segment_count + 1; i++) {
		std::pair<float, float> pair = utils::getCircleVertex(i, point_count, radius, angle_offset);
		varray_notch[i] = sf::Vertex(sf::Vector2f(pair.first, pair.second));
	}
}

const sf::Color& CircleNotchShape::getCircleColor() const {
	return circle_color;
}

const sf::Color& CircleNotchShape::getNotchColor() const {
	return notch_color;
}

void CircleNotchShape::setCircleColor(const sf::Color& color) {
	for (size_t i = 0; i < varray_circle.getVertexCount(); i++) {
		varray_circle[i].color = color;
	}
	circle_color = color;
}

void CircleNotchShape::setNotchColor(const sf::Color& color) {
	for (size_t i = 0; i < varray_notch.getVertexCount(); i++) {
		varray_notch[i].color = color;
	}
	notch_color = color;
}

void CircleNotchShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(varray_circle, states);
	target.draw(varray_notch, states);
}

void CircleNotchShape::drawMask(sf::RenderTarget& mask, sf::RenderStates states) {
	states.transform *= getTransform();
	sf::Color orig_color = circle_color;
	setCircleColor(sf::Color::White);
	mask.draw(varray_circle, states);
	setCircleColor(orig_color);
}

ChainObject::ChainObject(b2World* world, b2BodyDef def, std::vector<b2Vec2> p_vertices, sf::Color color) {
	this->color = color;
	rigid_body = world->CreateBody(&def);
	sf::VertexArray drawable_vertices(sf::LinesStrip, p_vertices.size());
	for (size_t i = 0; i < p_vertices.size(); i++) {
		drawable_vertices[i].position = tosf(p_vertices[i]);
		EditableVertex ev(p_vertices[i]);
		vertices.push_back(ev);
	}
	line_strip_shape = std::make_unique<LineStripShape>(drawable_vertices);
	syncVertices();
	line_strip_shape->setLineColor(color);
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

bool ChainObject::isClosed() {
	return false;
}

sf::Drawable* ChainObject::getDrawable() {
	return line_strip_shape.get();
}

sf::Transformable* ChainObject::getTransformable() {
	return line_strip_shape.get();
}

void ChainObject::drawMask(sf::RenderTarget& mask) {
	line_strip_shape->drawMask(mask);
}

TokenWriter& ChainObject::serialize(TokenWriter& tw) const {
	tw << "object" << id << "chain" << "\n";
	{
		TokenWriterIndent ground_indent(tw);
		tw << "vertices";
		b2ChainShape* chain = getShape();
		for (size_t i = 0; i < chain->m_count; i++) {
			tw << chain->m_vertices[i];
		}
		tw << "\n";
		tw.writeColorParam("color", color);
		serializeBody(tw, rigid_body) << "\n";
	}
	tw << "/object";
	return tw;
}

std::unique_ptr<ChainObject> ChainObject::deserialize(TokenReader& tr, b2World* world) {
	try {
		std::vector<b2Vec2> vertices;
		sf::Color color = sf::Color::White;
		BodyDef body_def;
		if (tr.tryEat("object")) {
			tr.eat("ground"); // change to chain
		}
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
				throw std::runtime_error("Unknown ChainObject parameter name: " + pname);
			}
		}
		b2BodyDef bdef = body_def.body_def;
		b2FixtureDef fdef = body_def.fixture_defs.front();
		std::unique_ptr<ChainObject> ground = std::make_unique<ChainObject>(world, bdef, vertices, color);
		ground->setDensity(fdef.density, false);
		ground->setFriction(fdef.friction, false);
		ground->setRestitution(fdef.restitution, false);
		return ground;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

b2ChainShape* ChainObject::getShape() const {
	return static_cast<b2ChainShape*>(rigid_body->GetFixtureList()->GetShape());
}

void ChainObject::syncVertices() {
	std::vector<b2Vec2> b2vertices = getPositions();
	b2Fixture* old_fixture = rigid_body->GetFixtureList();
	if (old_fixture) {
		rigid_body->DestroyFixture(old_fixture);
	}
	b2ChainShape new_chain;
	new_chain.CreateChain(b2vertices.data(), b2vertices.size(), b2vertices.front(), b2vertices.back());
	b2Fixture* new_fixture = rigid_body->CreateFixture(&new_chain, 1.0f);
	line_strip_shape->varray = sf::VertexArray(sf::LinesStrip, b2vertices.size());
	for (size_t i = 0; i < b2vertices.size(); i++) {
		line_strip_shape->varray[i].position = tosf(b2vertices[i]);
		line_strip_shape->varray[i].color = color;
	}
}

EditableVertex::EditableVertex(b2Vec2 pos) {
	this->pos = pos;
	this->orig_pos = pos;
	this->selected = false;
}

RevoluteJoint::RevoluteJoint(b2RevoluteJointDef& def, b2World* world, GameObject* object1, GameObject* object2) {
	this->object1 = object1;
	this->object2 = object2;
	def.bodyA = object1->rigid_body;
	def.bodyB = object2->rigid_body;
	joint = world->CreateJoint(&def);
}

RevoluteJoint::RevoluteJoint(b2RevoluteJoint* joint) {
	this->joint = joint;
}

TokenWriter& RevoluteJoint::serialize(TokenWriter& tw) {
	b2RevoluteJoint* joint = static_cast<b2RevoluteJoint*>(this->joint);
	size_t bodyAId = GameObject::getGameobject(joint->GetBodyA())->id;
	size_t bodyBId = GameObject::getGameobject(joint->GetBodyB())->id;
	tw << "joint revolute" << "\n";
	{
		TokenWriterIndent joint_indent(tw);
		tw.writeSizetParam("body_a", bodyAId);
		tw.writeSizetParam("body_b", bodyBId);
		tw.writeb2Vec2Param("anchor_a", joint->GetLocalAnchorA());
		tw.writeb2Vec2Param("anchor_b", joint->GetLocalAnchorB());
		tw.writeBoolParam("collide_connected", joint->GetCollideConnected());
		tw.writeFloatParam("angle_lower_limit", joint->GetLowerLimit());
		tw.writeFloatParam("angle_upper_limit", joint->GetUpperLimit());
		tw.writeBoolParam("angle_limit_enabled", joint->IsLimitEnabled());
		tw.writeFloatParam("motor_max_torque", joint->GetMaxMotorTorque());
		tw.writeFloatParam("motor_speed", joint->GetMotorSpeed());
		tw.writeBoolParam("motor_enabled", joint->IsMotorEnabled());
	}
	tw << "/joint";
	return tw;
}

b2RevoluteJointDef RevoluteJoint::deserialize(TokenReader& tr, ptrdiff_t& p_body_a, ptrdiff_t& p_body_b) {
	try {
		b2RevoluteJointDef def;
		def.bodyA = nullptr;
		def.bodyB = nullptr;
		p_body_a = -1;
		p_body_b = -1;
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "body_a") {
				p_body_a = tr.readULL();
			} else if (pname == "body_b") {
				p_body_b = tr.readULL();
			} else if (pname == "anchor_a") {
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

Joint::~Joint() {
	if (valid) {
		joint->GetBodyA()->GetWorld()->DestroyJoint(joint);
	}
}
