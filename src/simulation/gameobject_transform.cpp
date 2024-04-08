#include "simulation/gameobject_transform.h"
#include "simulation/gameobject.h"

GameObjectTransform::GameObjectTransform(const GameObject* object) {
	this->object = object;
	transform.SetIdentity();
	global_transform.SetIdentity();
}

const b2Transform& GameObjectTransform::getTransform() const {
	return transform;
}

const b2Transform& GameObjectTransform::getGlobalTransform() const {
	if (!global_transform_valid) {
		recalcGlobalTransform();
	}
	return global_transform;
}

void GameObjectTransform::invalidateGlobalTransform() {
	global_transform_valid = false;
	for (size_t i = 0; i < object->children.size(); i++) {
		object->children[i]->transforms.invalidateGlobalTransform();
	}
}

void GameObjectTransform::setTransform(const b2Vec2& position, float angle) {
	transform.Set(position, angle);
	invalidateGlobalTransform();
}

void GameObjectTransform::setGlobalTransform(const b2Transform& global_transform) {
	b2Transform parent_transform = object->getParentGlobalTransform();
	transform = b2MulT(parent_transform, global_transform);
	this->global_transform = global_transform;
	global_transform_valid = true;
}

void GameObjectTransform::setPosition(const b2Vec2& position) {
	transform.Set(position, transform.q.GetAngle());
	invalidateGlobalTransform();
}

void GameObjectTransform::setAngle(float angle) {
	transform.Set(transform.p, angle);
	invalidateGlobalTransform();
}

bool GameObjectTransform::operator==(const GameObjectTransform& other) const {
	return transform.q == other.transform.q && transform.p == other.transform.p;
}

void GameObjectTransform::recalcGlobalTransform() const {
	b2Transform parent_global_transform = object->getParentGlobalTransform();
	global_transform = b2Mul(parent_global_transform, transform);
	global_transform_valid = true;
}

bool operator==(const b2Rot& left, const b2Rot& right) {
	return left.c == right.c && left.s == right.s;
}
