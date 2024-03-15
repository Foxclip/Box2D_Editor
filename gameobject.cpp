#include <numbers>
#include "gameobject.h"
#include "objectlist.h"
#include "utils.h"

const auto tob2 = utils::tob2;
const auto tosf = utils::tosf;
const auto to2i = utils::to2i;
const auto to2f = utils::to2f;

GameObject::GameObject() { }

GameObject::~GameObject() {
	if (rigid_body) {
		//logger << "Destroy body: " << id << "\n";
		rigid_body->GetWorld()->DestroyBody(rigid_body);
		rigid_body = nullptr;
	}
}

const std::string& GameObject::getName() const {
	return name;
}

b2BodyType GameObject::getType() const {
	return rigid_body->GetType();
}

const b2Vec2& GameObject::getPosition() const {
	return getTransform().p;
}

const b2Vec2& GameObject::getLinearVelocity() const {
	return rigid_body->GetLinearVelocity();
}

float GameObject::getAngularVelocity() const {
	return rigid_body->GetAngularVelocity();
}

float GameObject::getRotation() const {
	return getTransform().q.GetAngle();
}

const b2Vec2& GameObject::getGlobalPosition() const {
	return getGlobalTransform().p;
}

float GameObject::getGlobalRotation() const {
	return getGlobalTransform().q.GetAngle();
}

const b2Transform& GameObject::getTransform() const {
	return transforms.getTransform();
}

const b2Transform& GameObject::getGlobalTransform() const {
	return transforms.getGlobalTransform();
}

const b2Transform& GameObject::getParentGlobalTransform() const {
	if (parent) {
		return parent->getGlobalTransform();
	} else {
		b2Transform tr;
		tr.SetIdentity();
		return tr;
	}
}

GameObject* GameObject::getParent() const {
	return parent;
}

CompoundVector<GameObject*> GameObject::getParentChain() const {
	const GameObject* cur_obj = this;
	CompoundVector<GameObject*> result;
	while (cur_obj) {
		if (cur_obj->parent) {
			result.add(cur_obj->parent);
			cur_obj = cur_obj->parent;
		} else {
			break;
		}
	}
	return result;
}

const CompoundVector<GameObject*>& GameObject::getChildren() const {
	return children;
}

CompoundVector<GameObject*> GameObject::getAllChildren() const {
	CompoundVector<GameObject*> result;
	result.insert(result.end(), children.begin(), children.end());
	for (size_t i = 0; i < children.size(); i++) {
		const CompoundVector<GameObject*>& child_children = children[i]->getAllChildren();
		result.insert(result.end(), child_children.begin(), child_children.end());
	}
	return result;
}

GameObject* GameObject::getChild(size_t index) const {
	return children[index];
}

b2Vec2 GameObject::toGlobal(const b2Vec2& pos) {
	b2Transform gt = getGlobalTransform();
	return b2Mul(gt, pos);
}

b2Vec2 GameObject::toLocal(const b2Vec2& pos) {
	b2Transform gt = getGlobalTransform();
	return b2MulT(gt, pos);
}

float GameObject::toGlobalAngle(float angle) {
	b2Transform gt = getGlobalTransform();
	return gt.q.GetAngle() + angle;
}

float GameObject::toLocalAngle(float angle) {
	b2Transform gt = getGlobalTransform();
	return angle - gt.q.GetAngle();
}

b2Vec2 GameObject::toParentLocal(const b2Vec2& pos) {
	b2Transform pgt = getParentGlobalTransform();
	return b2MulT(pgt, pos);
}

float GameObject::toParentLocalAngle(float angle) {
	b2Transform pgt = getParentGlobalTransform();
	return angle - pgt.q.GetAngle();
}

ptrdiff_t GameObject::getChildIndex(const GameObject* object) const {
	return children.getIndex(const_cast<GameObject*>(object));
}

void GameObject::setParent(GameObject* new_parent) {
	try {
		if (new_parent == this) {
			throw std::runtime_error("Cannot parent object to itself: id " + std::to_string(id));
		}
		CompoundVector<GameObject*> parent_chain = new_parent->getParentChain();
		if (parent_chain.contains(this)) {
			std::string chain_str;
			chain_str += std::to_string(id);
			chain_str += " -> " + std::to_string(new_parent->id);
			for (size_t i = 0; i < parent_chain.size(); i++) {
				chain_str += " -> " + std::to_string(parent_chain[i]->id);
				if (parent_chain[i] == this) {
					break;
				}
			}
			throw std::runtime_error("Loop in parent hierarchy: " + chain_str);
		}
		if (new_parent && object_list) {
			assert(object_list->contains(new_parent));
		}
		GameObject* old_parent = this->parent;
		if (old_parent) {
			old_parent->children.remove(this);
		}
		if (new_parent) {
			new_parent->children.add(this);
			parent_id = new_parent->id;
		} else {
			parent_id = -1;
		}
		b2Transform global_transform = getGlobalTransform();
		this->parent = new_parent;
		setGlobalTransform(global_transform);
		if (object_list) {
			if (new_parent) {
				object_list->top_objects.remove(this);
			} else {
				object_list->top_objects.add(this);
			}
		}
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

void GameObject::setName(const std::string& new_name) {
	if (new_name == name) {
		return;
	}
	if (object_list) {
		object_list->names.remove(name, this);
	}
	this->name = new_name;
	if (object_list) {
		object_list->names.add(new_name, this);
	}
}

void GameObject::updateVisual() {
	b2Vec2 position = getGlobalPosition();
	float angle = getGlobalRotation();
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

void GameObject::renderMask(sf::RenderTarget& mask) {
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

void GameObject::setGlobalTransform(const b2Transform& transform) {
	transforms.setGlobalTransform(transform);
}

void GameObject::setGlobalPosition(const b2Vec2& pos) {
	b2Vec2 parent_local_pos = toParentLocal(pos);
	transforms.setPosition(parent_local_pos);
	transformToRigidbody();
}

void GameObject::setGlobalAngle(float angle) {
	float parent_local_angle = toParentLocalAngle(angle);
	transforms.setAngle(parent_local_angle);
	transformToRigidbody();
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

size_t GameObject::indexLoop(ptrdiff_t index) const {
	return utils::neg_mod(index, (ptrdiff_t)getVertexCount());
}

size_t GameObject::getVertexCount() const {
	return vertices.size();
}

size_t GameObject::getEdgeCount() const {
	if (isClosed()) {
		return vertices.size();
	} else {
		return vertices.size() - 1;
	}
}

const EditableVertex& GameObject::getVertex(size_t index) const {
	return vertices[index];
}

b2Vec2 GameObject::getGlobalVertexPos(size_t index) {
	return toGlobal(vertices[index].pos);
}

void GameObject::setGlobalVertexPos(size_t index, const b2Vec2& new_pos) {
	b2Vec2 local_pos = toLocal(new_pos);
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
	b2Vec2 local_pos = toLocal(pos);
	vertices.insert(vertices.begin() + index, local_pos);
	syncVertices();
}

void GameObject::selectVertex(size_t index) {
	vertices[index].selected = true;
}

bool GameObject::isVertexSelected(size_t index) const {
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

void GameObject::syncVertices(bool save_velocities) {
	if (!save_velocities) {
		internalSyncVertices();
		return;
	}
	// velocities are recalculated when fixtures are added,
	// and might end up being not equal to starting velocities
	b2Vec2 saved_linear_velocity = rigid_body->GetLinearVelocity();
	float saved_angular_velocity = rigid_body->GetAngularVelocity();
	internalSyncVertices();
	rigid_body->SetLinearVelocity(saved_linear_velocity);
	rigid_body->SetAngularVelocity(saved_angular_velocity);
}

void GameObject::transformFromRigidbody() {
	transforms.setGlobalTransform(rigid_body->GetTransform());
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->transformFromRigidbody();
	}
}

void GameObject::transformToRigidbody() {
	rigid_body->SetTransform(getGlobalPosition(), getGlobalRotation());
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->transformToRigidbody();
	}
}

std::vector<b2Vec2> GameObject::getPositions() const {
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

BoxObject::BoxObject(GameObjectList* object_list, b2BodyDef def, b2Vec2 size, sf::Color color) {
	this->object_list = object_list;
	this->color = color;
	rigid_body = object_list->world->CreateBody(&def);
	vertices.push_back(EditableVertex(0.5f * size));
	syncVertices(true);
	transformFromRigidbody();
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

bool BoxObject::isClosed() const {
	return false;
}

sf::Drawable* BoxObject::getDrawable() const {
	return rect_shape.get();
}

sf::Transformable* BoxObject::getTransformable() const {
	return rect_shape.get();
}

void BoxObject::drawMask(sf::RenderTarget& mask) {
	sf::Color orig_color = rect_shape->getFillColor();
	rect_shape->setFillColor(sf::Color::White);
	mask.draw(*rect_shape);
	rect_shape->setFillColor(orig_color);
}

TokenWriter& BoxObject::serialize(TokenWriter& tw) const {
	tw << "object box" << "\n";
	{
		TokenWriterIndent box_indent(tw);
		tw.writeSizetParam("id", id);
		if (parent) {
			tw.writeSizetParam("parent_id", parent->id);
		}
		tw.writeQuotedStringParam("name", name);
		tw.writeb2Vec2Param("size", size);
		tw.writeColorParam("color", color);
		serializeBody(tw, rigid_body) << "\n";
	}
	tw << "/object";
	return tw;
}

std::unique_ptr<BoxObject> BoxObject::deserialize(TokenReader& tr, GameObjectList* object_list) {
	try {
		ptrdiff_t id = -1;
		ptrdiff_t parent_id = -1;
		std::string name = "<unnamed>";
		b2Vec2 size = b2Vec2(1.0f, 1.0f);
		sf::Color color = sf::Color::White;
		BodyDef body_def;
		if (tr.tryEat("object")) {
			tr.eat("box");
		}
		while(tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "id") {
				id = tr.readULL();
			} else if (pname == "parent_id") {
				parent_id = tr.readULL();
			} else if (pname == "name") {
				name = tr.readString();
			} else if (pname == "size") {
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
		std::unique_ptr<BoxObject> box = std::make_unique<BoxObject>(object_list, bdef, size, color);
		box->id = id;
		box->parent_id = parent_id;
		box->name = name;
		box->setDensity(fdef.density, false);
		box->setFriction(fdef.friction, false);
		box->setRestitution(fdef.restitution, false);
		return box;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

void BoxObject::internalSyncVertices() {
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

BallObject::BallObject(GameObjectList* object_list, b2BodyDef def, float radius, sf::Color color, sf::Color notch_color) {
	this->object_list = object_list;
	this->radius = radius;
	this->color = color;
	this->notch_color = notch_color;
	rigid_body = object_list->world->CreateBody(&def);
	vertices.push_back(EditableVertex(b2Vec2(radius, 0.0f)));
	syncVertices(true);
	transformFromRigidbody();
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

bool BallObject::isClosed() const {
	return false;
}

sf::Drawable* BallObject::getDrawable() const {
	return circle_notch_shape.get();
}

sf::Transformable* BallObject::getTransformable() const {
	return circle_notch_shape.get();
}

void BallObject::drawMask(sf::RenderTarget& mask) {
	circle_notch_shape->drawMask(mask);
}

TokenWriter& BallObject::serialize(TokenWriter& tw) const {
	tw << "object ball" << "\n";
	{
		TokenWriterIndent ball_indent(tw);
		tw.writeSizetParam("id", id);
		if (parent) {
			tw.writeSizetParam("parent_id", parent->id);
		}
		tw.writeQuotedStringParam("name", name);
		tw.writeFloatParam("radius", radius);
		tw.writeColorParam("color", color);
		tw.writeColorParam("notch_color", notch_color);
		serializeBody(tw, rigid_body) << "\n";
	}
	tw << "/object";
	return tw;
}

std::unique_ptr<BallObject> BallObject::deserialize(TokenReader& tr, GameObjectList* object_list) {
	try {
		ptrdiff_t id = -1;
		ptrdiff_t parent_id = -1;
		std::string name = "<unnamed>";
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
			if (pname == "id") {
				id = tr.readULL();
			} else if (pname == "parent_id") {
				parent_id = tr.readULL();
			} else if (pname == "name") {
				name = tr.readString();
			} else if (pname == "radius") {
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
		std::unique_ptr<BallObject> ball = std::make_unique<BallObject>(object_list, bdef, radius, color, notch_color);
		ball->id = id;
		ball->parent_id = parent_id;
		ball->name = name;
		ball->setDensity(fdef.density, false);
		ball->setFriction(fdef.friction, false);
		ball->setRestitution(fdef.restitution, false);
		return ball;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

void BallObject::internalSyncVertices() {
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

PolygonObject::PolygonObject(
	GameObjectList* object_list,
	b2BodyDef def,
	const std::vector<b2Vec2>& vertices,
	const sf::Color& color
) {
	this->object_list = object_list;
	this->color = color;
	rigid_body = object_list->world->CreateBody(&def);
	for (size_t i = 0; i < vertices.size(); i++) {
		this->vertices.push_back(EditableVertex(vertices[i]));
	}
	polygon = std::make_unique<SplittablePolygon>();
	syncVertices(true);
	transformFromRigidbody();
	polygon->setFillColor(color);
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

bool PolygonObject::isClosed() const {
	return true;
}

SplittablePolygon* PolygonObject::getSplittablePolygon() const {
	return polygon.get();
}

sf::Drawable* PolygonObject::getDrawable() const {
	return polygon.get();
}

sf::Transformable* PolygonObject::getTransformable() const {
	return polygon.get();
}

void PolygonObject::render(sf::RenderTarget& target) {
	polygon->draw_varray = draw_varray;
	GameObject::render(target);
}

void PolygonObject::drawMask(sf::RenderTarget& mask) {
	sf::Color orig_color = polygon->getFillColor();
	polygon->setFillColor(sf::Color::White);
	mask.draw(*polygon);
	polygon->setFillColor(orig_color);
}

TokenWriter& PolygonObject::serialize(TokenWriter& tw) const {
	tw << "object polygon" << "\n";
	{
		TokenWriterIndent car_indent(tw);
		tw.writeSizetParam("id", id);
		if (parent) {
			tw.writeSizetParam("parent_id", parent->id);
		}
		tw.writeQuotedStringParam("name", name);
		tw << "vertices";
		for (size_t i = 0; i < vertices.size(); i++) {
			tw << vertices[i].pos;
		}
		tw << "\n";
		tw.writeColorParam("color", color);
		serializeBody(tw, rigid_body) << "\n";
	}
	tw << "/object";
	return tw;
}

std::unique_ptr<PolygonObject> PolygonObject::deserialize(TokenReader& tr, GameObjectList* object_list) {
	try {
		ptrdiff_t id = -1;
		ptrdiff_t parent_id = -1;
		std::string name = "<unnamed>";
		sf::Color color = sf::Color::White;
		std::vector<b2Vec2> vertices;
		BodyDef body_def;
		std::vector<std::unique_ptr<BallObject>> wheels;
		if (tr.tryEat("object")) {
			tr.eat("polygon");
		}
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "id") {
				id = tr.readULL();
			} else if (pname == "parent_id") {
				parent_id = tr.readULL();
			} else if (pname == "name") {
				name = tr.readString();
			} else if (pname == "vertices") {
				vertices = tr.readb2Vec2Arr();
			} else if (pname == "color") {
				color = tr.readColor();
			} else if (pname == "body") {
				body_def = deserializeBody(tr);
			} else if (pname == "/object") {
				break;
			} else {
				throw std::runtime_error("Unknown PolygonObject parameter name: " + pname);
			}
		}
		b2BodyDef bdef = body_def.body_def;
		b2FixtureDef fdef = body_def.fixture_defs.front();
		std::unique_ptr<PolygonObject> car = std::make_unique<PolygonObject>(object_list, bdef, vertices, color);
		car->id = id;
		car->parent_id = parent_id;
		car->name = name;
		car->setDensity(fdef.density, false);
		car->setFriction(fdef.friction, false);
		car->setRestitution(fdef.restitution, false);
		return car;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

void PolygonObject::internalSyncVertices() {
	polygon->resetVarray(vertices.size());
	for (size_t i = 0; i < vertices.size(); i++) {
		polygon->setPoint(i, tosf(vertices[i].pos));
	}
	polygon->recut();
	destroyFixtures();
	std::vector<SplittablePolygon> convex_polygons = polygon->getConvexPolygons();
	for (size_t polygon_i = 0; polygon_i < convex_polygons.size(); polygon_i++) {
		SplittablePolygon& polygon = convex_polygons[polygon_i];
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
		sf::Vector2f pos = utils::get_circle_vertex<sf::Vector2f>(i, point_count, radius, angle_offset);
		varray_circle[i] = sf::Vertex(pos);
	}
	varray_notch[0] = sf::Vertex(sf::Vector2f(0.0f, 0.0f));
	for (size_t i = 0; i < notch_segment_count + 1; i++) {
		sf::Vector2f pos = utils::get_circle_vertex<sf::Vector2f>(i, point_count, radius, angle_offset);
		varray_notch[i] = sf::Vertex(pos);
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

ChainObject::ChainObject(GameObjectList* object_list, b2BodyDef def, std::vector<b2Vec2> p_vertices, sf::Color color) {
	this->object_list = object_list;
	this->color = color;
	rigid_body = object_list->world->CreateBody(&def);
	sf::VertexArray drawable_vertices(sf::LinesStrip, p_vertices.size());
	for (size_t i = 0; i < p_vertices.size(); i++) {
		drawable_vertices[i].position = tosf(p_vertices[i]);
		EditableVertex ev(p_vertices[i]);
		vertices.push_back(ev);
	}
	line_strip_shape = std::make_unique<LineStripShape>(drawable_vertices);
	syncVertices(true);
	transformFromRigidbody();
	line_strip_shape->setLineColor(color);
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

bool ChainObject::isClosed() const {
	return false;
}

sf::Drawable* ChainObject::getDrawable() const {
	return line_strip_shape.get();
}

sf::Transformable* ChainObject::getTransformable() const {
	return line_strip_shape.get();
}

void ChainObject::drawMask(sf::RenderTarget& mask) {
	line_strip_shape->drawMask(mask);
}

TokenWriter& ChainObject::serialize(TokenWriter& tw) const {
	tw << "object chain" << "\n";
	{
		TokenWriterIndent chain_indent(tw);
		tw.writeSizetParam("id", id);
		if (parent) {
			tw.writeSizetParam("parent_id", parent->id);
		}
		tw.writeQuotedStringParam("name", name);
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

std::unique_ptr<ChainObject> ChainObject::deserialize(TokenReader& tr, GameObjectList* object_list) {
	try {
		ptrdiff_t id = -1;
		ptrdiff_t parent_id = -1;
		std::string name = "<unnamed>";
		std::vector<b2Vec2> vertices;
		sf::Color color = sf::Color::White;
		BodyDef body_def;
		if (tr.tryEat("object")) {
			tr.eat("chain");
		}
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "id") {
				id = tr.readULL();
			} else if (pname == "parent_id") {
				parent_id = tr.readULL();
			} else if (pname == "name") {
				name = tr.readString();
			} else if (pname == "vertices") {
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
		std::unique_ptr<ChainObject> chain = std::make_unique<ChainObject>(object_list, bdef, vertices, color);
		chain->id = id;
		chain->parent_id = parent_id;
		chain->name = name;
		chain->setDensity(fdef.density, false);
		chain->setFriction(fdef.friction, false);
		chain->setRestitution(fdef.restitution, false);
		return chain;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

b2ChainShape* ChainObject::getShape() const {
	return static_cast<b2ChainShape*>(rigid_body->GetFixtureList()->GetShape());
}

void ChainObject::internalSyncVertices() {
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

TokenWriter& RevoluteJoint::serialize(TokenWriter& tw) const {
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
		if (tr.tryEat("joint")) {
			tr.eat("revolute");
		}
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
	size_t object_a_id = GameObject::getGameobject(joint->GetBodyA())->id;
	size_t object_b_id = GameObject::getGameobject(joint->GetBodyB())->id;
	//logger << "Destroy joint: " << object_a_id << " " << object_b_id << "\n";
	joint->GetBodyA()->GetWorld()->DestroyJoint(joint);
}

GameObjectTransforms::GameObjectTransforms(const GameObject* object) {
	this->object = object;
	transform.SetIdentity();
	global_transform.SetIdentity();
}

const b2Transform& GameObjectTransforms::getTransform() const {
	return transform;
}

const b2Transform& GameObjectTransforms::getGlobalTransform() const {
	if (!global_transform_valid) {
		recalcGlobalTransform();
	}
	return global_transform;
}

void GameObjectTransforms::invalidateGlobalTransform() {
	global_transform_valid = false;
	for (size_t i = 0; i < object->children.size(); i++) {
		object->children[i]->transforms.invalidateGlobalTransform();
	}
}

void GameObjectTransforms::setTransform(const b2Vec2& position, float angle) {
	transform.Set(position, angle);
	invalidateGlobalTransform();
}

void GameObjectTransforms::setGlobalTransform(const b2Transform& global_transform) {
	b2Transform parent_transform = object->getParentGlobalTransform();
	transform = b2MulT(parent_transform, global_transform);
	this->global_transform = global_transform;
	global_transform_valid = true;
}

void GameObjectTransforms::setPosition(const b2Vec2& position) {
	transform.Set(position, transform.q.GetAngle());
	invalidateGlobalTransform();
}

void GameObjectTransforms::setAngle(float angle) {
	transform.Set(transform.p, angle);
	invalidateGlobalTransform();
}

void GameObjectTransforms::recalcGlobalTransform() const {
	b2Transform parent_global_transform = object->getParentGlobalTransform();
	global_transform = b2Mul(parent_global_transform, transform);
	global_transform_valid = true;
}
