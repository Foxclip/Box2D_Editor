#include <numbers>
#include "simulation/gameobject.h"
#include "simulation/objectlist.h"

const auto tob2 = utils::tob2;
const auto tosf = utils::tosf;

EditableVertex::EditableVertex(b2Vec2 pos) {
	this->pos = pos;
	this->orig_pos = pos;
	this->selected = false;
}

bool EditableVertex::operator==(const EditableVertex& other) const {
	return pos == other.pos;
}

GameObject::GameObject() { }

GameObject::~GameObject() {
	if (rigid_body) {
		rigid_body->GetWorld()->DestroyBody(rigid_body);
		rigid_body = nullptr;
	}
}

ptrdiff_t GameObject::getId() const {
	return id;
}

ptrdiff_t GameObject::getParentId() const {
	if (!parent) {
		return -1;
	}
	return parent->getId();
}

const std::string& GameObject::getName() const {
	return name;
}

const sf::Color& GameObject::getColor() const {
	return color;
}

b2BodyType GameObject::getBodyType() const {
	return rigid_body->GetType();
}

b2Body* GameObject::getRigidBody() const {
	return rigid_body;
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
	return transform.getTransform();
}

const b2Transform& GameObject::getGlobalTransform() const {
	return transform.getGlobalTransform();
}

b2Transform GameObject::getParentGlobalTransform() const {
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

CompVector<GameObject*> GameObject::getParentChain() const {
	const GameObject* cur_obj = this;
	CompVector<GameObject*> result;
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

const CompVector<GameObject*>& GameObject::getChildren() const {
	return children;
}

CompVector<GameObject*> GameObject::getAllChildren() const {
	CompVector<GameObject*> result;
	result.insert(result.end(), children.begin(), children.end());
	for (size_t i = 0; i < children.size(); i++) {
		const CompVector<GameObject*>& child_children = children[i]->getAllChildren();
		result.insert(result.end(), child_children.begin(), child_children.end());
	}
	return result;
}

GameObject* GameObject::getChild(size_t index) const {
	return children[index];
}

size_t GameObject::getIndex() const {
	if (parent) {
		return parent->children.getIndex(const_cast<GameObject*>(this));
	} else {
		return object_list->top_objects.getIndex(const_cast<GameObject*>(this));
	}
}

const CompVector<Joint*>& GameObject::getJoints() const {
	return joints;
}

Joint* GameObject::getJoint(size_t index) const {
	return joints[index];
}

b2AABB GameObject::getApproxFixtureAABB(b2Fixture* fixture) const {
	b2AABB result;
	bool first = true;
	size_t child_shape_count = 1;
	if (b2ChainShape* chain = dynamic_cast<b2ChainShape*>(fixture->GetShape())) {
		child_shape_count = chain->m_count - 1;
	}
	for (size_t i = 0; i < child_shape_count; i++) {
		b2AABB aabb = fixture->GetAABB(static_cast<int32>(i));
		if (first) {
			result = aabb;
			first = false;
		} else {
			result.Combine(aabb);
		}
	}
	return result;
}

b2AABB GameObject::getExactFixtureAABB(b2Fixture* fixture) const {
	b2AABB result;
	bool first = true;
	b2Shape* shape = fixture->GetShape();
	auto init_aabb = [&](const b2Vec2& vertex) {
		result.lowerBound.x = vertex.x;
		result.lowerBound.y = vertex.y;
		result.upperBound.x = vertex.x;
		result.upperBound.y = vertex.y;
	};
	auto extend_aabb = [&](const b2Vec2& vertex) {
		result.lowerBound.x = std::min(result.lowerBound.x, vertex.x);
		result.lowerBound.y = std::min(result.lowerBound.y, vertex.y);
		result.upperBound.x = std::max(result.upperBound.x, vertex.x);
		result.upperBound.y = std::max(result.upperBound.y, vertex.y);
	};
	if (b2PolygonShape* polygon = dynamic_cast<b2PolygonShape*>(shape)) {
		init_aabb(toGlobal(polygon->m_vertices[0]));
		for (size_t i = 1; i < polygon->m_count; i++) {
			extend_aabb(toGlobal(polygon->m_vertices[i]));
		}
	} else if (b2CircleShape* circle = dynamic_cast<b2CircleShape*>(shape)) {
		b2Vec2 global_center = toGlobal(circle->m_p);
		result.lowerBound.x = global_center.x - circle->m_radius;
		result.lowerBound.y = global_center.y - circle->m_radius;
		result.upperBound.x = global_center.x + circle->m_radius;
		result.upperBound.y = global_center.y + circle->m_radius;
	} else if (b2EdgeShape* edge = dynamic_cast<b2EdgeShape*>(shape)) {
		init_aabb(toGlobal(edge->m_vertex1));
		extend_aabb(toGlobal(edge->m_vertex2));
	} else if (b2ChainShape* chain = dynamic_cast<b2ChainShape*>(shape)) {
		init_aabb(toGlobal(chain->m_vertices[0]));
		for (size_t i = 1; i < chain->m_count; i++) {
			extend_aabb(toGlobal(chain->m_vertices[i]));
		}
	}
	return result;
}

b2AABB GameObject::getApproxAABB() const {
	return getAABB(false);
}

b2AABB GameObject::getExactAABB() const {
	return getAABB(true);
}

b2Vec2 GameObject::toGlobal(const b2Vec2& pos) const {
	b2Transform gt = getGlobalTransform();
	return b2Mul(gt, pos);
}

b2Vec2 GameObject::toLocal(const b2Vec2& pos) const {
	b2Transform gt = getGlobalTransform();
	return b2MulT(gt, pos);
}

float GameObject::toGlobalAngle(float angle) const {
	b2Transform gt = getGlobalTransform();
	return gt.q.GetAngle() + angle;
}

float GameObject::toLocalAngle(float angle) const {
	b2Transform gt = getGlobalTransform();
	return angle - gt.q.GetAngle();
}

b2Vec2 GameObject::toParentLocal(const b2Vec2& pos) const {
	b2Transform pgt = getParentGlobalTransform();
	return b2MulT(pgt, pos);
}

float GameObject::toParentLocalAngle(float angle) const {
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
		CompVector<GameObject*> parent_chain = new_parent->getParentChain();
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
			object_list->OnSetParent(this, new_parent);
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

void GameObject::renderMask(const std::function<void(const sf::Drawable& drawable)>& draw_func) {
	updateVisual();
	drawMask(draw_func);
}

void GameObject::setDrawVarray(bool value) {
	draw_varray = value;
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

void GameObject::setGlobalTransform(const b2Transform& p_transform) {
	transform.setGlobalTransform(p_transform);
}

void GameObject::setPosition(const b2Vec2& pos) {
	transform.setPosition(pos);
	transformToRigidbody();
}

void GameObject::setGlobalPosition(const b2Vec2& pos) {
	b2Vec2 parent_local_pos = toParentLocal(pos);
	transform.setPosition(parent_local_pos);
	transformToRigidbody();
}

void GameObject::setAngle(float angle) {
	transform.setAngle(angle);
}

void GameObject::setGlobalAngle(float angle) {
	float parent_local_angle = toParentLocalAngle(angle);
	transform.setAngle(parent_local_angle);
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

void GameObject::moveToIndex(size_t index) {
	if (parent) {
		parent->moveChildToIndex(this, index);
	} else {
		object_list->moveObjectToIndex(this, index);
	}
}

void GameObject::moveChildToIndex(GameObject* child, size_t index) {
	mAssert(children.contains(child));
	children.moveValueToIndex(child, index);
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

const std::vector<EditableVertex>& GameObject::getVertices() const {
	return vertices;
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
	transform.setGlobalTransform(rigid_body->GetTransform());
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

std::string GameObject::serialize() const {
	TokenWriter tw;
	serialize(tw);
	return tw.toStr();
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

b2AABB GameObject::getAABB(bool exact) const {
	b2AABB result;
	bool first = true;
	result.lowerBound = b2Vec2_zero;
	result.upperBound = b2Vec2_zero;
	for (b2Fixture* f = rigid_body->GetFixtureList(); f; f = f->GetNext()) {
		b2AABB aabb;
		if (exact) {
			aabb = getExactFixtureAABB(f);
		} else {
			aabb = getApproxFixtureAABB(f);
		}
		if (first) {
			result = aabb;
			first = false;
		} else {
			result.Combine(aabb);
		}
	}
	return result;
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

bool GameObject::compare(const GameObject& other, bool compare_id) const {
	if (const BoxObject* box = dynamic_cast<const BoxObject*>(this)) {
		if (!box->isEqual(&other)) {
			return false;
		}
	} else if (const BallObject* ball = dynamic_cast<const BallObject*>(this)) {
		if (!ball->isEqual(&other)) {
			return false;
		}
	} else if (const PolygonObject* polygon = dynamic_cast<const PolygonObject*>(this)) {
		if (!polygon->isEqual(&other)) {
			return false;
		}
	} else if (const ChainObject* chain = dynamic_cast<const ChainObject*>(this)) {
		if (!chain->isEqual(&other)) {
			return false;
		}
	} else {
		mAssert(false, "Unknown GameObject type:" + std::string(typeid(other).name()));
	}
	if (compare_id) {
		if (id != other.id) {
			return false;
		}
		if (parent_id != other.parent_id) {
			return false;
		}
		if (children.size() != other.children.size()) {
			return false;
		}
		for (size_t i = 0; i < children.size(); i++) {
			if (children[i]->id != other.children[i]->id) {
				return false;
			}
		}
	}
	if (color != other.color) {
		return false;
	}
	if (name != other.name) {
		return false;
	}
	if (transform != other.transform) {
		return false;
	}
	if (vertices != other.vertices) {
		return false;
	}
	return true;
}

bool GameObject::operator==(const GameObject& other) const {
	return compare(other);
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

GameObject::GameObjectType BoxObject::getType() const {
	return GameObjectType::Box;
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

void BoxObject::drawMask(const std::function<void(const sf::Drawable& drawable)>& draw_func) {
	sf::Color orig_color = rect_shape->getFillColor();
	rect_shape->setFillColor(sf::Color::White);
	draw_func(*rect_shape);
	rect_shape->setFillColor(orig_color);
}

TokenWriter& BoxObject::serialize(TokenWriter& tw) const {
	tw << "object box" << "\n";
	{
		TokenWriterIndent box_indent(tw);
		tw.writeSizetParam("id", id);
		if (parent) {
			tw.writeSizetParam("parent_id", parent->getId());
		}
		tw.writeQuotedStringParam("name", name);
		tw.writeb2Vec2Param("size", size);
		tw.writeColorParam("color", color);
		serializeBody(tw, rigid_body) << "\n";
	}
	tw << "/object";
	return tw;
}

dp::DataPointerUnique<BoxObject> BoxObject::deserialize(const std::string& str, GameObjectList* object_list) {
	TokenReader tr(str);
	dp::DataPointerUnique<BoxObject> uptr = deserialize(tr, object_list);
	return uptr;
}

dp::DataPointerUnique<BoxObject> BoxObject::deserialize(TokenReader& tr, GameObjectList* object_list) {
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
		dp::DataPointerUnique<BoxObject> box = dp::make_data_pointer<BoxObject>("BoxObject " + name, object_list, bdef, size, color);
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
	rect_shape = dp::make_data_pointer<sf::RectangleShape>("BoxObject " + name + " rect_shape", tosf(size));
	rect_shape->setOrigin(size.x / 2.0f, size.y / 2.0f);
	rect_shape->setFillColor(color);
}

bool BoxObject::isEqual(const GameObject* other) const {
	const BoxObject* box = dynamic_cast<const BoxObject*>(other);
	if (!box) {
		return false;
	}
	if (size != box->size) {
		return false;
	}
	return true;
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

GameObject::GameObjectType BallObject::getType() const {
	return GameObjectType::Ball;
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

void BallObject::drawMask(const std::function<void(const sf::Drawable& drawable)>& draw_func) {
	sf::Color orig_circle_color = circle_notch_shape->getCircleColor();
	sf::Color orig_notch_color = circle_notch_shape->getNotchColor();
	circle_notch_shape->setCircleColor(sf::Color::White);
	circle_notch_shape->setNotchColor(sf::Color::White);
	draw_func(*circle_notch_shape);
	circle_notch_shape->setCircleColor(orig_circle_color);
	circle_notch_shape->setNotchColor(orig_notch_color);
}

TokenWriter& BallObject::serialize(TokenWriter& tw) const {
	tw << "object ball" << "\n";
	{
		TokenWriterIndent ball_indent(tw);
		tw.writeSizetParam("id", id);
		if (parent) {
			tw.writeSizetParam("parent_id", parent->getId());
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

dp::DataPointerUnique<BallObject> BallObject::deserialize(const std::string& str, GameObjectList* object_list) {
	TokenReader tr(str);
	dp::DataPointerUnique<BallObject> uptr = deserialize(tr, object_list);
	return uptr;
}

dp::DataPointerUnique<BallObject> BallObject::deserialize(TokenReader& tr, GameObjectList* object_list) {
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
		dp::DataPointerUnique<BallObject> ball = dp::make_data_pointer<BallObject>("BallObject " + name, object_list, bdef, radius, color, notch_color);
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
	circle_notch_shape = dp::make_data_pointer<CircleNotchShape>("BallObject " + name + " circle_notch_shape", radius, 30, 4);
	circle_notch_shape->setCircleColor(color);
	circle_notch_shape->setNotchColor(notch_color);
}

bool BallObject::isEqual(const GameObject* other) const {
	const BallObject* ball = dynamic_cast<const BallObject*>(other);
	if (!ball) {
		return false;
	}
	if (radius != ball->radius) {
		return false;
	}
	return true;
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
	polygon = dp::make_data_pointer<SplittablePolygon>("PolygonObject " + name + " SplittablePolygon");
	syncVertices(true);
	transformFromRigidbody();
	polygon->setFillColor(color);
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

GameObject::GameObjectType PolygonObject::getType() const {
	return GameObjectType::Polygon;
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

void PolygonObject::drawMask(const std::function<void(const sf::Drawable& drawable)>& draw_func) {
	sf::Color orig_color = polygon->getFillColor();
	polygon->setFillColor(sf::Color::White);
	draw_func(*polygon);
	polygon->setFillColor(orig_color);
}

void PolygonObject::setDrawVarray(bool value) {
	draw_varray = value;
	polygon->draw_varray = value;
}

TokenWriter& PolygonObject::serialize(TokenWriter& tw) const {
	tw << "object polygon" << "\n";
	{
		TokenWriterIndent car_indent(tw);
		tw.writeSizetParam("id", id);
		if (parent) {
			tw.writeSizetParam("parent_id", parent->getId());
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

dp::DataPointerUnique<PolygonObject> PolygonObject::deserialize(const std::string& str, GameObjectList* object_list) {
	TokenReader tr(str);
	dp::DataPointerUnique<PolygonObject> uptr = deserialize(tr, object_list);
	return uptr;
}

dp::DataPointerUnique<PolygonObject> PolygonObject::deserialize(TokenReader& tr, GameObjectList* object_list) {
	try {
		ptrdiff_t id = -1;
		ptrdiff_t parent_id = -1;
		std::string name = "<unnamed>";
		sf::Color color = sf::Color::White;
		std::vector<b2Vec2> vertices;
		BodyDef body_def;
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
		dp::DataPointerUnique<PolygonObject> car = dp::make_data_pointer<PolygonObject>("PolygonObject(car) " + name, object_list, bdef, vertices, color);
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
		bool valid = b2polygon.Set(b2points.data(), (int32)b2points.size());
		if (valid) {
			rigid_body->CreateFixture(&b2polygon, 1.0f);
		}
	}
}

bool PolygonObject::isEqual(const GameObject* other) const {
	const PolygonObject* polygon = dynamic_cast<const PolygonObject*>(other);
	if (!polygon) {
		return false;
	}
	return true;
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
	line_strip_shape = dp::make_data_pointer<LineStripShape>("ChainObject " + name + " line_strip_shape", drawable_vertices);
	syncVertices(true);
	transformFromRigidbody();
	line_strip_shape->setLineColor(color);
	rigid_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

GameObject::GameObjectType ChainObject::getType() const {
	return GameObjectType::Chain;
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

void ChainObject::drawMask(const std::function<void(const sf::Drawable& drawable)>& draw_func) {
	draw_func(*line_strip_shape);
}

TokenWriter& ChainObject::serialize(TokenWriter& tw) const {
	tw << "object chain" << "\n";
	{
		TokenWriterIndent chain_indent(tw);
		tw.writeSizetParam("id", id);
		if (parent) {
			tw.writeSizetParam("parent_id", parent->getId());
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

dp::DataPointerUnique<ChainObject> ChainObject::deserialize(const std::string& str, GameObjectList* object_list) {
	TokenReader tr(str);
	dp::DataPointerUnique<ChainObject> uptr = deserialize(tr, object_list);
	return uptr;
}

dp::DataPointerUnique<ChainObject> ChainObject::deserialize(TokenReader& tr, GameObjectList* object_list) {
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
		dp::DataPointerUnique<ChainObject> chain = dp::make_data_pointer<ChainObject>("ChainObject " + name, object_list, bdef, vertices, color);
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
	new_chain.CreateChain(b2vertices.data(), (int32)b2vertices.size(), b2vertices.front(), b2vertices.back());
	b2Fixture* new_fixture = rigid_body->CreateFixture(&new_chain, 1.0f);
	line_strip_shape->varray = sf::VertexArray(sf::LinesStrip, b2vertices.size());
	for (size_t i = 0; i < b2vertices.size(); i++) {
		line_strip_shape->varray[i].position = tosf(b2vertices[i]);
		line_strip_shape->varray[i].color = color;
	}
}

bool ChainObject::isEqual(const GameObject* other) const {
	const ChainObject* chain = dynamic_cast<const ChainObject*>(other);
	if (!chain) {
		return false;
	}
	return true;
}

