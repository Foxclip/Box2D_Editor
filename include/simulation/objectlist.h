#pragma once

#include "gameobject.h"
#include "common/compvector.h"
#include "common/event.h"
#include "common/searchindex.h"
#include "common/utils.h"

class GameObjectList {
public:
	DataPointer<b2World> world;
	Event<GameObject*> OnObjectAdded;
	Event<GameObject*> OnBeforeObjectRemoved;
	Event<GameObject*> OnAfterObjectRemoved;
	Event<GameObject*, GameObject*> OnSetParent;
	Event<GameObject*, size_t> OnObjectMoved;
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
	const CompVector<GameObject*>& getTopObjects() const;
	const CompVector<GameObject*>& getAllObjects() const;
	ptrdiff_t getMaxId() const;
	GameObject* add(DataPointer<GameObject> object, bool assign_new_id);
	Joint* addJoint(DataPointer<Joint> joint);
	GameObject* duplicate(const GameObject* object, bool with_children = false);
	CompVector<GameObject*> duplicate(const CompVector<GameObject*>& old_objects);
	void transformFromRigidbody();
	void moveObjectToIndex(GameObject* object, size_t index);
	void remove(GameObject* object, bool remove_children);
	void removeJoint(Joint* joint);
	void clear();
	GameObject* operator[](size_t index) const;

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
