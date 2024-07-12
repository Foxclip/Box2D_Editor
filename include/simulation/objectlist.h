#pragma once

#include "gameobject.h"
#include "common/compvector.h"
#include "common/event.h"
#include "common/searchindex.h"
#include "common/utils.h"

class GameObjectList {
public:
	std::unique_ptr<b2World> world;
	Event<GameObject*> OnObjectAdded;
	Event<GameObject*> OnObjectRemoved;
	Event<> OnClear;

	b2World* getWorld() const;
	size_t getTopSize() const;
	size_t getAllSize() const;
	size_t getJointsSize() const;
	GameObject* getFromTop(size_t i) const;
	GameObject* getFromAll(size_t i) const;
	GameObject* getById(size_t id) const;
	GameObject* getByName(const std::string& name) const;
	bool contains(GameObject* object) const;
	ptrdiff_t getTopIndex(GameObject* object) const;
	ptrdiff_t getAllIndex(GameObject* object) const;
	Joint* getJoint(size_t i) const;
	const CompVector<GameObject*>& getTopVector() const;
	const CompVector<GameObject*>& getAllVector() const;
	ptrdiff_t getMaxId() const;
	GameObject* add(std::unique_ptr<GameObject> object, bool assign_new_id);
	Joint* addJoint(std::unique_ptr<Joint> joint);
	GameObject* duplicate(const GameObject* object, bool with_children = false);
	CompVector<GameObject*> duplicate(const CompVector<GameObject*>& old_objects);
	void transformFromRigidbody();
	void remove(GameObject* object, bool remove_children);
	void removeJoint(Joint* joint);
	void clear();

private:
	friend class GameObject;
	CompVectorUptr<GameObject> all_objects;
	CompVector<GameObject*> top_objects;
	CompVectorUptr<Joint> joints;
	SearchIndexUnique<size_t, GameObject*> ids;
	SearchIndexMultiple<std::string, GameObject*> names;

	GameObject* duplicateObject(const GameObject* object);
	Joint* duplicateJoint(const Joint* joint, GameObject* new_object_a, GameObject* new_object_b);

};
