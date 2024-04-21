#pragma once

#include <set>
#include <map>

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
	std::map<TData, TObject*> map;
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
	std::map<TData, std::set<TObject*>> map;

};

template<typename TData, typename TObject>
inline SearchIndexUnique<TData, TObject>::SearchIndexUnique() { }

template<typename TData, typename TObject>
inline bool SearchIndexUnique<TData, TObject>::add(const TData& data, TObject* ptr) {
	auto inserted = map.insert({ data, ptr });
	return inserted.second;
}

template<typename TData, typename TObject>
inline size_t SearchIndexUnique<TData, TObject>::size() const {
	return map.size();
}

template<typename TData, typename TObject>
inline TObject* SearchIndexUnique<TData, TObject>::find(const TData& data) const {
	auto it = map.find(data);
	if (it != map.end()) {
		TObject* ptr = (*it).second;
		return ptr;
	}
	return nullptr;
}

template<typename TData, typename TObject>
inline TData SearchIndexUnique<TData, TObject>::min() const {
	return (*map.begin()).first;
}

template<typename TData, typename TObject>
inline TData SearchIndexUnique<TData, TObject>::max() const {
	return (*map.rbegin()).first;
}

template<typename TData, typename TObject>
inline bool SearchIndexUnique<TData, TObject>::contains(const TData& data) const {
	return map.contains(data);
}

template<typename TData, typename TObject>
inline void SearchIndexUnique<TData, TObject>::remove(const TData& data) {
	map.erase(data);
}

template<typename TData, typename TObject>
inline void SearchIndexUnique<TData, TObject>::clear() {
	map = std::map<TData, TObject*>();
}

template<typename TData, typename TObject>
inline SearchIndexMultiple<TData, TObject>::SearchIndexMultiple() { }

template<typename TData, typename TObject>
inline bool SearchIndexMultiple<TData, TObject>::add(const TData& data, TObject* ptr) {
	auto it = map.find(data);
	if (it == map.end()) {
		auto inserted = map.insert({ data, std::set<TObject*>() });
		it = inserted.first;
	}
	auto inserted = it->second.insert(ptr);
	return inserted.second;
}

template<typename TData, typename TObject>
inline size_t SearchIndexMultiple<TData, TObject>::size() const {
	size_t result = 0;
	for (auto& node : map) {
		result += node.second.size();
	}
	return result;
}

template<typename TData, typename TObject>
inline TObject* SearchIndexMultiple<TData, TObject>::find(const TData& data) const {
	auto it = map.find(data);
	if (it != map.end()) {
		TObject* ptr = *it->second.begin();
		return ptr;
	}
	return nullptr;
}

template<typename TData, typename TObject>
inline TData SearchIndexMultiple<TData, TObject>::min() const {
	return (*map.begin()).first;
}

template<typename TData, typename TObject>
inline TData SearchIndexMultiple<TData, TObject>::max() const {
	return (*map.rbegin()).first;
}

template<typename TData, typename TObject>
inline bool SearchIndexMultiple<TData, TObject>::contains(const TData& data) const {
	return map.contains(data);
}

template<typename TData, typename TObject>
inline void SearchIndexMultiple<TData, TObject>::remove(const TData& data, TObject* ptr) {
	auto it = map.find(data);
	if (it != map.end()) {
		std::set<TObject*>& set = it->second;
		set.erase(ptr);
		if (set.empty()) {
			map.erase(it);
		}
	}
}

template<typename TData, typename TObject>
inline void SearchIndexMultiple<TData, TObject>::clear() {
	map = std::map<TData, std::set<TObject*>>();
}

