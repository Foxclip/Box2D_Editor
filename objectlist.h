#pragma once

#include "gameobject.h"
#include "compvector.h"
#include "searchindex.h"

class GameObjectList {
public:
	b2World* world = nullptr;

	size_t getTopSize() const;
	size_t getAllSize() const;
	size_t getJointsSize() const;
	GameObject* getFromTop(size_t i) const;
	GameObject* getFromAll(size_t i) const;
	GameObject* getById(size_t id) const;
	GameObject* getByName(const std::string& name) const;
	ptrdiff_t getTopIndex(GameObject* object) const;
	Joint* getJoint(size_t i) const;
	const std::vector<GameObject*>& getTopVector() const;
	const std::vector<GameObject*>& getAllVector() const;
	ptrdiff_t getMaxId() const;
	GameObject* add(std::unique_ptr<GameObject> object, bool assign_new_id);
	Joint* addJoint(std::unique_ptr<Joint> joint);
	GameObject* duplicate(const GameObject* object, bool with_children = false);
	std::vector<GameObject*> duplicate(const CompoundVector<GameObject*>& old_objects);
	void setParent(GameObject* child, GameObject* new_parent);
	void setName(GameObject* object, const std::string& new_name);
	void transformFromRigidbody();
	void remove(GameObject* object, bool remove_children);
	void removeJoint(Joint* joint);
	void clear();

private:
	CompoundVectorUptr<GameObject> all_objects;
	CompoundVector<GameObject*> top_objects;
	CompoundVectorUptr<Joint> joints;
	SearchIndexUnique<size_t, GameObject> ids;
	SearchIndexMultiple<std::string, GameObject> names;

	GameObject* duplicateObject(const GameObject* object);
	Joint* duplicateJoint(const Joint* joint, GameObject* new_a, GameObject* new_b);

};
