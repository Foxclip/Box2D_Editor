#pragma once

#include <set>
#include <map>

template<typename TKey, typename TObject>
class SearchIndex {
public:
	virtual bool add(const TKey& key, TObject* ptr) = 0;
	virtual size_t size() const = 0;
	virtual TObject* find(const TKey& key) const = 0;
	virtual TKey min() const = 0;
	virtual TKey max() const = 0;
	virtual bool contains(const TKey& key) const = 0;
	virtual void clear() = 0;
};

template<typename TKey, typename TObject>
class SearchIndexUnique : SearchIndex<TKey, TObject> {
public:
	SearchIndexUnique();
	bool add(const TKey& key, TObject* ptr);
	size_t size() const;
	TObject* find(const TKey& key) const;
	TKey min() const;
	TKey max() const;
	bool contains(const TKey& key) const;
	void remove(const TKey& key);
	void clear();

private:
	std::map<TKey, TObject*> map;
};

template<typename TKey, typename TObject>
class SearchIndexMultiple : SearchIndex<TKey, TObject> {
public:
	SearchIndexMultiple();
	bool add(const TKey& key, TObject* ptr);
	size_t size() const;
	TObject* find(const TKey& key) const;
	TKey min() const;
	TKey max() const;
	bool contains(const TKey& key) const;
	void remove(const TKey& key, TObject* ptr);
	void clear();

private:
	std::map<TKey, std::set<TObject*>> map;

};

template<typename TKey, typename TObject>
inline SearchIndexUnique<TKey, TObject>::SearchIndexUnique() { }

template<typename TKey, typename TObject>
inline bool SearchIndexUnique<TKey, TObject>::add(const TKey& key, TObject* ptr) {
	auto inserted = map.insert({ key, ptr });
	return inserted.second;
}

template<typename TKey, typename TObject>
inline size_t SearchIndexUnique<TKey, TObject>::size() const {
	return map.size();
}

template<typename TKey, typename TObject>
inline TObject* SearchIndexUnique<TKey, TObject>::find(const TKey& key) const {
	auto it = map.find(key);
	if (it != map.end()) {
		TObject* ptr = (*it).second;
		return ptr;
	}
	return nullptr;
}

template<typename TKey, typename TObject>
inline TKey SearchIndexUnique<TKey, TObject>::min() const {
	return (*map.begin()).first;
}

template<typename TKey, typename TObject>
inline TKey SearchIndexUnique<TKey, TObject>::max() const {
	return (*map.rbegin()).first;
}

template<typename TKey, typename TObject>
inline bool SearchIndexUnique<TKey, TObject>::contains(const TKey& key) const {
	return map.contains(key);
}

template<typename TKey, typename TObject>
inline void SearchIndexUnique<TKey, TObject>::remove(const TKey& key) {
	map.erase(key);
}

template<typename TKey, typename TObject>
inline void SearchIndexUnique<TKey, TObject>::clear() {
	map = std::map<TKey, TObject*>();
}

template<typename TKey, typename TObject>
inline SearchIndexMultiple<TKey, TObject>::SearchIndexMultiple() { }

template<typename TKey, typename TObject>
inline bool SearchIndexMultiple<TKey, TObject>::add(const TKey& key, TObject* ptr) {
	auto it = map.find(key);
	if (it == map.end()) {
		auto inserted = map.insert({ key, std::set<TObject*>() });
		it = inserted.first;
	}
	auto inserted = it->second.insert(ptr);
	return inserted.second;
}

template<typename TKey, typename TObject>
inline size_t SearchIndexMultiple<TKey, TObject>::size() const {
	size_t result = 0;
	for (auto& node : map) {
		result += node.second.size();
	}
	return result;
}

template<typename TKey, typename TObject>
inline TObject* SearchIndexMultiple<TKey, TObject>::find(const TKey& key) const {
	auto it = map.find(key);
	if (it != map.end()) {
		TObject* ptr = *it->second.begin();
		return ptr;
	}
	return nullptr;
}

template<typename TKey, typename TObject>
inline TKey SearchIndexMultiple<TKey, TObject>::min() const {
	return (*map.begin()).first;
}

template<typename TKey, typename TObject>
inline TKey SearchIndexMultiple<TKey, TObject>::max() const {
	return (*map.rbegin()).first;
}

template<typename TKey, typename TObject>
inline bool SearchIndexMultiple<TKey, TObject>::contains(const TKey& key) const {
	return map.contains(key);
}

template<typename TKey, typename TObject>
inline void SearchIndexMultiple<TKey, TObject>::remove(const TKey& key, TObject* ptr) {
	auto it = map.find(key);
	if (it != map.end()) {
		std::set<TObject*>& set = it->second;
		set.erase(ptr);
		if (set.empty()) {
			map.erase(it);
		}
	}
}

template<typename TKey, typename TObject>
inline void SearchIndexMultiple<TKey, TObject>::clear() {
	map = std::map<TKey, std::set<TObject*>>();
}

