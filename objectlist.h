#pragma once

#include "gameobject.h"
#include "compvector.h"

struct ObjectId {
public:
	size_t id;
	const GameObject* ptr;

	ObjectId(size_t id, const GameObject* ptr);
	bool operator<(const ObjectId& other) const;

private:
};

class GameObjectList {
public:
	b2World* world = nullptr;

	size_t getTopSize() const;
	size_t getAllSize() const;
	size_t getJointsSize() const;
	GameObject* getFromTop(size_t i);
	GameObject* getFromAll(size_t i);
	GameObject* getById(size_t id);
	ptrdiff_t getTopIndex(GameObject* object) const;
	Joint* getJoint(size_t i);
	const std::vector<GameObject*>& getTopVector() const;
	std::vector<GameObject*> getAllVector() const;
	ptrdiff_t getMaxId() const;
	GameObject* add(std::unique_ptr<GameObject> object, bool assign_new_id);
	Joint* addJoint(std::unique_ptr<Joint> joint);
	GameObject* duplicate(const GameObject* object);
	Joint* duplicateJoint(const Joint* joint, GameObject* new_a, GameObject* new_b);
	void remove(GameObject* object, bool remove_children);
	void removeJoint(Joint* joint);
	void clear();

private:
	CompoundVectorUptr<GameObject> all_objects;
	CompoundVector<GameObject*> top_objects;
	CompoundVectorUptr<Joint> joints;
	std::set<ObjectId> ids;
};
