#pragma once

#include <set>
#include <map>

template<typename TKey, typename TData>
class SearchIndex {
public:
	virtual bool add(const TKey& key, TData* data) = 0;
	virtual size_t size() const = 0;
	virtual TData* find(const TKey& key) const = 0;
	virtual TKey min() const = 0;
	virtual TKey max() const = 0;
	virtual bool contains(const TKey& key) const = 0;
	virtual void clear() = 0;
};

template<typename TKey, typename TData>
class SearchIndexUnique : SearchIndex<TKey, TData> {
public:
	SearchIndexUnique();
	bool add(const TKey& key, TData* data);
	size_t size() const;
	TData* find(const TKey& key) const;
	TKey min() const;
	TKey max() const;
	bool contains(const TKey& key) const;
	void remove(const TKey& key);
	void clear();

private:
	std::map<TKey, TData*> map;
};

template<typename TKey, typename TData>
class SearchIndexMultiple : SearchIndex<TKey, TData> {
public:
	SearchIndexMultiple();
	bool add(const TKey& key, TData* data);
	size_t size() const;
	TData* find(const TKey& key) const;
	TKey min() const;
	TKey max() const;
	bool contains(const TKey& key) const;
	void remove(const TKey& key, TData* data);
	void clear();

private:
	std::map<TKey, std::set<TData*>> map;

};

template<typename TKey, typename TData>
inline SearchIndexUnique<TKey, TData>::SearchIndexUnique() { }

template<typename TKey, typename TData>
inline bool SearchIndexUnique<TKey, TData>::add(const TKey& key, TData* data) {
	auto inserted = map.insert({ key, data });
	return inserted.second;
}

template<typename TKey, typename TData>
inline size_t SearchIndexUnique<TKey, TData>::size() const {
	return map.size();
}

template<typename TKey, typename TData>
inline TData* SearchIndexUnique<TKey, TData>::find(const TKey& key) const {
	auto it = map.find(key);
	if (it != map.end()) {
		TData* data = (*it).second;
		return data;
	}
	return nullptr;
}

template<typename TKey, typename TData>
inline TKey SearchIndexUnique<TKey, TData>::min() const {
	return (*map.begin()).first;
}

template<typename TKey, typename TData>
inline TKey SearchIndexUnique<TKey, TData>::max() const {
	return (*map.rbegin()).first;
}

template<typename TKey, typename TData>
inline bool SearchIndexUnique<TKey, TData>::contains(const TKey& key) const {
	return map.contains(key);
}

template<typename TKey, typename TData>
inline void SearchIndexUnique<TKey, TData>::remove(const TKey& key) {
	map.erase(key);
}

template<typename TKey, typename TData>
inline void SearchIndexUnique<TKey, TData>::clear() {
	map = std::map<TKey, TData*>();
}

template<typename TKey, typename TData>
inline SearchIndexMultiple<TKey, TData>::SearchIndexMultiple() { }

template<typename TKey, typename TData>
inline bool SearchIndexMultiple<TKey, TData>::add(const TKey& key, TData* data) {
	auto it = map.find(key);
	if (it == map.end()) {
		auto inserted = map.insert({ key, std::set<TData*>() });
		it = inserted.first;
	}
	auto inserted = it->second.insert(data);
	return inserted.second;
}

template<typename TKey, typename TData>
inline size_t SearchIndexMultiple<TKey, TData>::size() const {
	size_t result = 0;
	for (auto& node : map) {
		result += node.second.size();
	}
	return result;
}

template<typename TKey, typename TData>
inline TData* SearchIndexMultiple<TKey, TData>::find(const TKey& key) const {
	auto it = map.find(key);
	if (it != map.end()) {
		TData* data = *it->second.begin();
		return data;
	}
	return nullptr;
}

template<typename TKey, typename TData>
inline TKey SearchIndexMultiple<TKey, TData>::min() const {
	return (*map.begin()).first;
}

template<typename TKey, typename TData>
inline TKey SearchIndexMultiple<TKey, TData>::max() const {
	return (*map.rbegin()).first;
}

template<typename TKey, typename TData>
inline bool SearchIndexMultiple<TKey, TData>::contains(const TKey& key) const {
	return map.contains(key);
}

template<typename TKey, typename TData>
inline void SearchIndexMultiple<TKey, TData>::remove(const TKey& key, TData* data) {
	auto it = map.find(key);
	if (it != map.end()) {
		std::set<TData*>& set = it->second;
		set.erase(data);
		if (set.empty()) {
			map.erase(it);
		}
	}
}

template<typename TKey, typename TData>
inline void SearchIndexMultiple<TKey, TData>::clear() {
	map = std::map<TKey, std::set<TData*>>();
}

