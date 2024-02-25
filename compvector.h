#pragma once

#include <vector>
#include <set>
#include <memory>

template<typename T>
class CompoundVector {
public:
	CompoundVector();
	size_t size() const;
	bool add(T value);
	bool remove(T value);
	void removeByIndex(size_t index);
	std::vector<T>::iterator begin();
	std::vector<T>::const_iterator begin() const;
	std::vector<T>::iterator end();
	std::vector<T>::const_iterator end() const;
	std::set<T>::iterator sbegin();
	std::set<T>::const_iterator sbegin() const;
	std::set<T>::iterator send();
	std::set<T>::const_iterator send() const;
	T& front();
	const T& front() const;
	T& back();
	const T& back() const;
	T& get(size_t index);
	const T& get(size_t index) const;
	ptrdiff_t getIndex(T value) const;
	const std::vector<T>& getVector() const;
	const std::vector<T>& getSet() const;
	T& operator[](size_t index);
	const T& operator[](size_t index) const;
	std::set<T>::iterator find(const T& value) const;
	bool contains(T value) const;
	void clear();

private:
	std::vector<T> vector;
	std::set<T> set;
};

template<typename T>
class CompoundVectorUptr {
public:
	CompoundVectorUptr();
	size_t size() const;
	bool add(std::unique_ptr<T> value);
	bool remove(T* value);
	void removeByIndex(size_t index);
	std::vector<T*>::iterator begin();
	std::vector<T*>::const_iterator begin() const;
	std::vector<T*>::iterator end();
	std::vector<T*>::const_iterator end() const;
	std::set<T*>::iterator sbegin();
	std::set<T*>::const_iterator sbegin() const;
	std::set<T*>::iterator send();
	std::set<T*>::const_iterator send() const;
	T* front();
	const T* front() const;
	T* back();
	const T* back() const;
	T* get(size_t index);
	const T* get(size_t index) const;
	ptrdiff_t getIndex(T* value) const;
	const std::vector<T*>& getVector() const;
	const std::set<T*>& getSet() const;
	T* operator[](size_t index);
	const T* operator[](size_t index) const;
	std::set<T*>::iterator find(T* value);
	bool contains(T* value) const;
	void clear();

private:
	std::vector<std::unique_ptr<T>> uptrs;
	CompoundVector<T*> comp;

};

template<typename T>
inline CompoundVector<T>::CompoundVector() { }

template<typename T>
inline size_t CompoundVector<T>::size() const {
	return vector.size();
}

template<typename T>
inline bool CompoundVector<T>::add(T value) {
	auto inserted = set.insert(value);
	if (inserted.second) {
		vector.push_back(value);
		return true;
	}
	return false;
}

template<typename T>
inline bool CompoundVector<T>::remove(T value) {
	size_t removed = set.erase(value);
	if (removed > 0) {
		ptrdiff_t index = getIndex(value);
		vector.erase(vector.begin() + index);
		return true;
	}
	return false;
}

template<typename T>
inline void CompoundVector<T>::removeByIndex(size_t index) {
	T value = vector[index];
	vector.erase(vector.begin() + index);
	set.erase(value);
}

template<typename T>
inline std::vector<T>::iterator CompoundVector<T>::begin() {
	return vector.begin();
}

template<typename T>
inline std::vector<T>::const_iterator CompoundVector<T>::begin() const {
	return vector.begin();
}

template<typename T>
inline std::vector<T>::iterator CompoundVector<T>::end() {
	return vector.end();
}

template<typename T>
inline std::vector<T>::const_iterator CompoundVector<T>::end() const {
	return vector.end();
}

template<typename T>
inline std::set<T>::iterator CompoundVector<T>::sbegin() {
	return set.begin();
}

template<typename T>
inline std::set<T>::const_iterator CompoundVector<T>::sbegin() const {
	return set.begin();
}

template<typename T>
inline std::set<T>::iterator CompoundVector<T>::send() {
	return set.end();
}

template<typename T>
inline std::set<T>::const_iterator CompoundVector<T>::send() const {
	return set.end();
}

template<typename T>
inline T& CompoundVector<T>::front() {
	return vector.front();
}

template<typename T>
inline const T& CompoundVector<T>::front() const {
	return vector.front();
}

template<typename T>
inline T& CompoundVector<T>::back() {
	return vector.back();
}

template<typename T>
inline const T& CompoundVector<T>::back() const {
	return vector.back();
}

template<typename T>
inline T& CompoundVector<T>::get(size_t index) {
	return vector[index];
}

template<typename T>
inline const T& CompoundVector<T>::get(size_t index) const {
	return vector[index];
}

template<typename T>
inline ptrdiff_t CompoundVector<T>::getIndex(T value) const {
	for (size_t i = 0; i < vector.size(); i++) {
		if (vector[i] == value) {
			return i;
		}
	}
	return -1;
}

template<typename T>
inline const std::vector<T>& CompoundVector<T>::getVector() const {
	return vector;
}

template<typename T>
inline const std::vector<T>& CompoundVector<T>::getSet() const {
	return set;
}

template<typename T>
inline T& CompoundVector<T>::operator[](size_t index) {
	return get(index);
}

template<typename T>
inline const T& CompoundVector<T>::operator[](size_t index) const {
	return get(index);
}

template<typename T>
inline std::set<T>::iterator CompoundVector<T>::find(const T& value) const {
	return set.find(value);
}

template<typename T>
inline bool CompoundVector<T>::contains(T value) const {
	return set.contains(value);
}

template<typename T>
inline void CompoundVector<T>::clear() {
	vector = std::vector<T>();
	set = std::set<T>();
}

template<typename T>
inline CompoundVectorUptr<T>::CompoundVectorUptr() { }

template<typename T>
inline size_t CompoundVectorUptr<T>::size() const {
	return comp.size();
}

template<typename T>
inline bool CompoundVectorUptr<T>::add(std::unique_ptr<T> value) {
	T* ptr = value.get();
	bool added = comp.add(ptr);
	if (added) {
		uptrs.push_back(std::move(value));
		return true;
	}
	return false;
}

template<typename T>
inline bool CompoundVectorUptr<T>::remove(T* value) {
	bool removed = comp.remove(value);
	if (removed) {
		ptrdiff_t index = getIndex(value);
		uptrs.erase(uptrs.begin() + index);
		return true;
	}
	return false;
}

template<typename T>
inline void CompoundVectorUptr<T>::removeByIndex(size_t index) {
	uptrs.erase(uptrs.begin() + index);
	comp.removeByIndex(index);
}

template<typename T>
inline std::vector<T*>::iterator CompoundVectorUptr<T>::begin() {
	return comp.begin();
}

template<typename T>
inline std::vector<T*>::const_iterator CompoundVectorUptr<T>::begin() const {
	return comp.begin();
}

template<typename T>
inline std::vector<T*>::iterator CompoundVectorUptr<T>::end() {
	return comp.end();
}

template<typename T>
inline std::vector<T*>::const_iterator CompoundVectorUptr<T>::end() const {
	return comp.end();
}

template<typename T>
inline std::set<T*>::iterator CompoundVectorUptr<T>::sbegin() {
	return comp.sbegin();
}

template<typename T>
inline std::set<T*>::const_iterator CompoundVectorUptr<T>::sbegin() const {
	return comp.sbegin();
}

template<typename T>
inline std::set<T*>::iterator CompoundVectorUptr<T>::send() {
	return comp.send();
}

template<typename T>
inline std::set<T*>::const_iterator CompoundVectorUptr<T>::send() const {
	return comp.send();
}

template<typename T>
inline T* CompoundVectorUptr<T>::front() {
	return comp.front().get();
}

template<typename T>
inline const T* CompoundVectorUptr<T>::front() const {
	return comp.front().get();
}

template<typename T>
inline T* CompoundVectorUptr<T>::back() {
	return comp.back().get();
}

template<typename T>
inline const T* CompoundVectorUptr<T>::back() const {
	return comp.back().get();
}

template<typename T>
inline T* CompoundVectorUptr<T>::get(size_t index) {
	return comp[index].get();
}

template<typename T>
inline const T* CompoundVectorUptr<T>::get(size_t index) const {
	return comp[index].get();
}

template<typename T>
inline ptrdiff_t CompoundVectorUptr<T>::getIndex(T* value) const {
	for (size_t i = 0; i < comp.size(); i++) {
		if (comp[i] == value) {
			return i;
		}
	}
	return -1;
}

template<typename T>
inline const std::vector<T*>& CompoundVectorUptr<T>::getVector() const {
	return comp.getVector();
}

template<typename T>
inline const std::set<T*>& CompoundVectorUptr<T>::getSet() const {
	return comp.getSet();
}

template<typename T>
inline T* CompoundVectorUptr<T>::operator[](size_t index) {
	return comp[index];
}

template<typename T>
inline const T* CompoundVectorUptr<T>::operator[](size_t index) const {
	return comp[index];
}

template<typename T>
inline std::set<T*>::iterator CompoundVectorUptr<T>::find(T* value) {
	return comp.find(value);
}

template<typename T>
inline bool CompoundVectorUptr<T>::contains(T* value) const {
	return comp.contains(value);
}

template<typename T>
inline void CompoundVectorUptr<T>::clear() {
	uptrs = std::vector<std::unique_ptr<T>>();
	comp.clear();
}
