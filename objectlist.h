#pragma once

#include "gameobject.h"

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
	GameObject* getFromTop(size_t i) const;
	GameObject* getFromAll(size_t i) const;
	GameObject* getById(size_t id) const;
	Joint* getJoint(size_t i) const;
	const std::vector<GameObject*>& getTopVector() const;
	const std::vector<GameObject*>& getAllVector() const;
	ptrdiff_t getMaxId() const;
	GameObject* add(std::unique_ptr<GameObject> gameobject, bool assign_new_id = false);
	Joint* addJoint(std::unique_ptr<Joint> joint);
	GameObject* duplicate(const GameObject* object);
	Joint* duplicateJoint(const Joint* joint, GameObject* new_a, GameObject* new_b);
	bool remove(GameObject* object);
	bool removeJoint(Joint* joint);
	void clear();

private:
	std::vector<std::unique_ptr<GameObject>> uptrs;
	std::vector<std::unique_ptr<Joint>> joint_uptrs;
	std::vector<GameObject*> top_objects;
	std::vector<GameObject*> all_objects;
	std::vector<Joint*> joints;
	std::set<ObjectId> ids;

	void addToAll(GameObject* object, bool assign_new_id = false);
	bool removeFromAll(GameObject* object);

};
