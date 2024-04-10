#pragma once
#include <set>

template<typename TData, typename TObject>
struct ObjectData {
	TData data;
	TObject* ptr = nullptr;

	ObjectData(TData data, TObject* ptr) {
		this->data = data;
		this->ptr = ptr;
	}
	bool operator<(const ObjectData& other) const {
		return data < other.data;
	}

};

template<typename TData, typename TObject>
struct ObjectDataNode {
	TData data;
	std::set<TObject*> set;

	ObjectDataNode(TData data) {
		this->data = data;
	}
	bool operator<(const ObjectDataNode& other) const {
		return data < other.data;
	}

};

template<typename TData, typename TObject>
class SearchIndex {
public:
	virtual bool add(const TData& data, TObject* ptr) = 0;
	virtual size_t size() const = 0;
	virtual TObject* find(const TData& data) const = 0;
	virtual TData min() const = 0;
	virtual TData max() const = 0;
	virtual bool contains(const TData& data) const = 0;
	virtual void clear() = 0;
};

template<typename TData, typename TObject>
class SearchIndexUnique : SearchIndex<TData, TObject> {
public:
	SearchIndexUnique();
	bool add(const TData& data, TObject* ptr);
	size_t size() const;
	TObject* find(const TData& data) const;
	TData min() const;
	TData max() const;
	bool contains(const TData& data) const;
	void remove(const TData& data);
	void clear();

private:
	std::set<ObjectData<TData, TObject>> set;
};

template<typename TData, typename TObject>
class SearchIndexMultiple : SearchIndex<TData, TObject> {
public:
	SearchIndexMultiple();
	bool add(const TData& data, TObject* ptr);
	size_t size() const;
	TObject* find(const TData& data) const;
	TData min() const;
	TData max() const;
	bool contains(const TData& data) const;
	void remove(const TData& data, TObject* ptr);
	void clear();

private:
	std::set<ObjectDataNode<TData, TObject>> set;

	ObjectDataNode<TData, TObject>* findNode(const TData& data) const;
};

template<typename TData, typename TObject>
inline SearchIndexUnique<TData, TObject>::SearchIndexUnique() { }

template<typename TData, typename TObject>
inline bool SearchIndexUnique<TData, TObject>::add(const TData& data, TObject* ptr) {
	auto inserted = set.insert(ObjectData<TData, TObject>(data, ptr));
	return inserted.second;
}

template<typename TData, typename TObject>
inline size_t SearchIndexUnique<TData, TObject>::size() const {
	return set.size();
}

template<typename TData, typename TObject>
inline TObject* SearchIndexUnique<TData, TObject>::find(const TData& data) const {
	auto it = set.find(ObjectData<TData, TObject>(data, nullptr));
	if (it != set.end()) {
		TObject* ptr = const_cast<TObject*>(it->ptr);
		return ptr;
	}
	return nullptr;
}

template<typename TData, typename TObject>
inline TData SearchIndexUnique<TData, TObject>::min() const {
	return set.begin()->data;
}

template<typename TData, typename TObject>
inline TData SearchIndexUnique<TData, TObject>::max() const {
	return set.rbegin()->data;
}

template<typename TData, typename TObject>
inline bool SearchIndexUnique<TData, TObject>::contains(const TData& data) const {
	return find(data);
}

template<typename TData, typename TObject>
inline void SearchIndexUnique<TData, TObject>::remove(const TData& data) {
	set.erase(ObjectData<TData, TObject>(data, nullptr));
}

template<typename TData, typename TObject>
inline void SearchIndexUnique<TData, TObject>::clear() {
	set = std::set<ObjectData<TData, TObject>>();
}

template<typename TData, typename TObject>
inline SearchIndexMultiple<TData, TObject>::SearchIndexMultiple() { }

template<typename TData, typename TObject>
inline bool SearchIndexMultiple<TData, TObject>::add(const TData& data, TObject* ptr) {
	ObjectDataNode<TData, TObject>* node = findNode(data);
	if (!node) {
		auto inserted = set.insert(ObjectDataNode<TData, TObject>(data));
		node = const_cast<ObjectDataNode<TData, TObject>*>(&*(inserted.first));
	}
	auto inserted = node->set.insert(ptr);
	return inserted.second;
}

template<typename TData, typename TObject>
inline size_t SearchIndexMultiple<TData, TObject>::size() const {
	size_t result = 0;
	for (const ObjectDataNode<TData, TObject>& node : set) {
		result += node.set.size();
	}
	return result;
}

template<typename TData, typename TObject>
inline TObject* SearchIndexMultiple<TData, TObject>::find(const TData& data) const {
	ObjectDataNode<TData, TObject>* node = findNode(data);
	if (node) {
		TObject* ptr = *node->set.begin();
		return ptr;
	}
	return nullptr;
}

template<typename TData, typename TObject>
inline TData SearchIndexMultiple<TData, TObject>::min() const {
	return set.begin()->data;
}

template<typename TData, typename TObject>
inline TData SearchIndexMultiple<TData, TObject>::max() const {
	return set.rbegin()->data;
}

template<typename TData, typename TObject>
inline bool SearchIndexMultiple<TData, TObject>::contains(const TData& data) const {
	return find(data);
}

template<typename TData, typename TObject>
inline void SearchIndexMultiple<TData, TObject>::remove(const TData& data, TObject* ptr) {
	ObjectDataNode<TData, TObject>* node = findNode(data);
	if (node) {
		node->set.erase(ptr);
		if (node->set.empty()) {
			set.erase(*node);
		}
	}
}

template<typename TData, typename TObject>
inline void SearchIndexMultiple<TData, TObject>::clear() {
	set = std::set<ObjectDataNode<TData, TObject>>();
}

template<typename TData, typename TObject>
inline ObjectDataNode<TData, TObject>* SearchIndexMultiple<TData, TObject>::findNode(const TData& data) const {
	auto it = set.find(ObjectDataNode<TData, TObject>(data));
	if (it != set.end()) {
		ObjectDataNode<TData, TObject>* ptr = const_cast<ObjectDataNode<TData, TObject>*>(&*it);
		return ptr;
	}
	return nullptr;
}
