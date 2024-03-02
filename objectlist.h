#pragma once

#include "gameobject.h"
#include "compvector.h"

template<typename T>
struct ObjectData {
public:
	T data;
	const GameObject* ptr = nullptr;

	ObjectData(T data, const GameObject* ptr) {
		this->data = data;
		this->ptr = ptr;
	}
	bool operator<(const ObjectData& other) const {
		return data < other.data;
	}

private:
};

template<typename TSet, typename TData>
concept is_object_data_set = requires {
	std::same_as<TSet, std::set<ObjectData<TData>>>
	|| std::same_as<TSet, std::multiset<ObjectData<TData>>>;
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
	GameObject* getByName(const std::string& name) const;
	ptrdiff_t getTopIndex(GameObject* object) const;
	Joint* getJoint(size_t i) const;
	const std::vector<GameObject*>& getTopVector() const;
	const std::vector<GameObject*>& getAllVector() const;
	ptrdiff_t getMaxId() const;
	GameObject* add(std::unique_ptr<GameObject> object, bool assign_new_id);
	Joint* addJoint(std::unique_ptr<Joint> joint);
	GameObject* duplicate(const GameObject* object);
	std::vector<GameObject*> duplicateObjects(const CompoundVector<GameObject*>& old_objects);
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
	std::set<ObjectData<size_t>> ids;
	std::multiset<ObjectData<std::string>> names;

	template<typename TSet, typename TData>
	requires is_object_data_set<TSet, TData>
	GameObject* getByData(
		const TSet& set, const TData& data
	) const;
	Joint* duplicateJoint(const Joint* joint, GameObject* new_a, GameObject* new_b);

};

template<typename TSet, typename TData>
requires is_object_data_set<TSet, TData>
inline GameObject* GameObjectList::getByData(const TSet& set, const TData& data) const {
	auto it = set.find(ObjectData<TData>(data, nullptr));
	if (it != set.end()) {
		return const_cast<GameObject*>((*it).ptr);
	}
	return nullptr;
}
