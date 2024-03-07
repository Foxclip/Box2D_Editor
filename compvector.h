#pragma once

#include <vector>
#include <set>
#include <memory>

template<typename T>
class CompoundVector {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	CompoundVector();
	CompoundVector(const std::initializer_list<T>& list);
	CompoundVector(const std::vector<T>& vec);
	size_t size() const;
	bool add(const T& value);
	bool insert(std::vector<T>::const_iterator where, const T& value);
	template<std::incrementable TIter>
	size_t insert(std::vector<T>::const_iterator where, TIter first, TIter last);
	ptrdiff_t remove(const T& value);
	void removeByIndex(size_t index);
	std::vector<T>::const_iterator begin() const;
	std::vector<T>::const_iterator end() const;
	std::vector<T>::const_reverse_iterator rbegin() const;
	std::vector<T>::const_reverse_iterator rend() const;
	T& front();
	const T& front() const;
	T& back();
	const T& back() const;
	T& at(size_t index);
	const T& at(size_t index) const;
	ptrdiff_t getIndex(const T& value) const;
	const std::vector<T>& getVector() const;
	const std::set<T>& getSet() const;
	T& operator[](size_t index);
	const T& operator[](size_t index) const;
	std::set<T>::iterator find(const T& value) const;
	bool contains(const T& value) const;
	void clear();
	bool operator==(const CompoundVector<T>& other) const;

private:
	std::vector<T> vector;
	std::set<T> set;
};

template<typename T>
class CompoundVectorUptr {
public:
	using value_type = T*;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	CompoundVectorUptr();
	CompoundVectorUptr(const std::initializer_list<T>& list);
	CompoundVectorUptr(const std::initializer_list<T*>& list);
	CompoundVectorUptr(const std::vector<T*>& vec);
	size_t size() const;
	bool add(const T& value);
	bool add(const T* ptr);
	bool add(std::unique_ptr<T> value);
	ptrdiff_t remove(T* value);
	void removeByIndex(size_t index);
	std::vector<T*>::const_iterator begin() const;
	std::vector<T*>::const_iterator end() const;
	std::vector<T*>::const_reverse_iterator rbegin() const;
	std::vector<T*>::const_reverse_iterator rend() const;
	T* front();
	T* front() const;
	T* back();
	T* back() const;
	T* at(size_t index);
	T* at(size_t index) const;
	ptrdiff_t getIndex(const T* value) const;
	const CompoundVector<T*>& getCompVector() const;
	const std::vector<T*>& getVector() const;
	const std::set<T*>& getSet() const;
	T* operator[](size_t index);
	T* operator[](size_t index) const;
	std::set<T*>::iterator find(const T* value) const;
	bool contains(const T* value) const;
	void clear();

private:
	std::vector<std::unique_ptr<T>> uptrs;
	CompoundVector<T*> comp;

};

template<typename T>
inline CompoundVector<T>::CompoundVector() { }

template<typename T>
inline CompoundVector<T>::CompoundVector(const std::initializer_list<T>& list) {
	for (const T& value : list) {
		add(value);
	}
}

template<typename T>
inline CompoundVector<T>::CompoundVector(const std::vector<T>& vec) {
	for (const T& value : vec) {
		add(value);
	}
}

template<typename T>
inline size_t CompoundVector<T>::size() const {
	return vector.size();
}

template<typename T>
inline bool CompoundVector<T>::add(const T& value) {
	auto inserted = set.insert(value);
	if (inserted.second) {
		vector.push_back(value);
		return true;
	}
	return false;
}

template<typename T>
inline bool CompoundVector<T>::insert(std::vector<T>::const_iterator where, const T& value) {
	auto inserted = set.insert(value);
	if (inserted.second) {
		vector.insert(where, value);
		return true;
	}
	return false;
}

template<typename T>
template<std::incrementable TIter>
inline size_t CompoundVector<T>::insert(std::vector<T>::const_iterator where, TIter first, TIter last) {
	size_t offset = where - vector.begin();
	TIter iter_other(first);
	size_t inserted_count = 0;
	while (iter_other != last) {
		if (insert(vector.begin() + offset, *iter_other)) {
			inserted_count++;
			offset++;
		}
		iter_other++;
	}
	return inserted_count;
}


template<typename T>
inline ptrdiff_t CompoundVector<T>::remove(const T& value) {
	size_t removed = set.erase(value);
	if (removed > 0) {
		ptrdiff_t index = getIndex(value);
		vector.erase(vector.begin() + index);
		return index;
	}
	return -1;
}

template<typename T>
inline void CompoundVector<T>::removeByIndex(size_t index) {
	T value = vector[index];
	vector.erase(vector.begin() + index);
	set.erase(value);
}

template<typename T>
inline std::vector<T>::const_iterator CompoundVector<T>::begin() const {
	return vector.begin();
}

template<typename T>
inline std::vector<T>::const_iterator CompoundVector<T>::end() const {
	return vector.end();
}

template<typename T>
inline std::vector<T>::const_reverse_iterator CompoundVector<T>::rbegin() const {
	return vector.rbegin();
}

template<typename T>
inline std::vector<T>::const_reverse_iterator CompoundVector<T>::rend() const {
	return vector.rend();
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
inline T& CompoundVector<T>::at(size_t index) {
	return vector[index];
}

template<typename T>
inline const T& CompoundVector<T>::at(size_t index) const {
	return vector[index];
}

template<typename T>
inline ptrdiff_t CompoundVector<T>::getIndex(const T& value) const {
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
inline const std::set<T>& CompoundVector<T>::getSet() const {
	return set;
}

template<typename T>
inline T& CompoundVector<T>::operator[](size_t index) {
	return at(index);
}

template<typename T>
inline const T& CompoundVector<T>::operator[](size_t index) const {
	return at(index);
}

template<typename T>
inline std::set<T>::iterator CompoundVector<T>::find(const T& value) const {
	return set.find(value);
}

template<typename T>
inline bool CompoundVector<T>::contains(const T& value) const {
	return set.contains(value);
}

template<typename T>
inline void CompoundVector<T>::clear() {
	vector = std::vector<T>();
	set = std::set<T>();
}

template<typename T>
inline bool CompoundVector<T>::operator==(const CompoundVector<T>& other) const {
	if (size() != other.size()) {
		return false;
	}
	for (size_t i = 0; i < size(); i++) {
		if (other[i] != at(i)) {
			return false;
		}
	}
	return true;
}

template<typename T>
inline CompoundVectorUptr<T>::CompoundVectorUptr() { }

template<typename T>
inline CompoundVectorUptr<T>::CompoundVectorUptr(const std::initializer_list<T>& list) {
	for (const T& value : list) {
		std::unique_ptr<T> uptr = std::make_unique<T>(value);
		add(std::move(uptr));
	}
}

template<typename T>
inline CompoundVectorUptr<T>::CompoundVectorUptr(const std::initializer_list<T*>& list) {
	for (const T* ptr : list) {
		add(ptr);
	}
}

template<typename T>
inline CompoundVectorUptr<T>::CompoundVectorUptr(const std::vector<T*>& vec) {
	for (const T* value : vec) {
		std::unique_ptr<T> uptr = std::unique_ptr<T>(const_cast<T*>(value));
		add(std::move(uptr));
	}
}

template<typename T>
inline size_t CompoundVectorUptr<T>::size() const {
	return comp.size();
}

template<typename T>
inline bool CompoundVectorUptr<T>::add(const T& value) {
	const T* ptr = &value;
	bool added = comp.add(const_cast<T*>(ptr));
	if (added) {
		std::unique_ptr<T> uptr = std::make_unique<T>(value);
		uptrs.push_back(std::move(uptr));
		return true;
	}
	return false;
}

template<typename T>
inline bool CompoundVectorUptr<T>::add(const T* ptr) {
	bool added = comp.add(const_cast<T*>(ptr));
	if (added) {
		std::unique_ptr<T> uptr = std::unique_ptr<T>(const_cast<T*>(ptr));
		uptrs.push_back(std::move(uptr));
		return true;
	}
	return false;
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
inline ptrdiff_t CompoundVectorUptr<T>::remove(T* value) {
	ptrdiff_t removed_index = comp.remove(value);
	if (removed_index >= 0) {
		uptrs.erase(uptrs.begin() + removed_index);
		return removed_index;
	}
	return -1;
}

template<typename T>
inline void CompoundVectorUptr<T>::removeByIndex(size_t index) {
	uptrs.erase(uptrs.begin() + index);
	comp.removeByIndex(index);
}

template<typename T>
inline std::vector<T*>::const_iterator CompoundVectorUptr<T>::begin() const {
	return comp.begin();
}

template<typename T>
inline std::vector<T*>::const_iterator CompoundVectorUptr<T>::end() const {
	return comp.end();
}

template<typename T>
inline std::vector<T*>::const_reverse_iterator CompoundVectorUptr<T>::rbegin() const {
	return comp.rbegin();
}

template<typename T>
inline std::vector<T*>::const_reverse_iterator CompoundVectorUptr<T>::rend() const {
	return comp.rend();
}

template<typename T>
inline T* CompoundVectorUptr<T>::front() {
	return comp.front();
}

template<typename T>
inline T* CompoundVectorUptr<T>::front() const {
	return comp.front();
}

template<typename T>
inline T* CompoundVectorUptr<T>::back() {
	return comp.back();
}

template<typename T>
inline T* CompoundVectorUptr<T>::back() const {
	return comp.back();
}

template<typename T>
inline T* CompoundVectorUptr<T>::at(size_t index) {
	return comp[index];
}

template<typename T>
inline T* CompoundVectorUptr<T>::at(size_t index) const {
	return comp[index];
}

template<typename T>
inline ptrdiff_t CompoundVectorUptr<T>::getIndex(const T* value) const {
	for (size_t i = 0; i < comp.size(); i++) {
		if (comp[i] == value) {
			return i;
		}
	}
	return -1;
}

template<typename T>
inline const CompoundVector<T*>& CompoundVectorUptr<T>::getCompVector() const {
	return comp;
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
inline T* CompoundVectorUptr<T>::operator[](size_t index) const {
	return comp[index];
}

template<typename T>
inline std::set<T*>::iterator CompoundVectorUptr<T>::find(const T* value) const {
	return comp.find(const_cast<T*>(value));
}

template<typename T>
inline bool CompoundVectorUptr<T>::contains(const T* value) const {
	return comp.contains(const_cast<T*>(value));
}

template<typename T>
inline void CompoundVectorUptr<T>::clear() {
	uptrs = std::vector<std::unique_ptr<T>>();
	comp.clear();
}

#ifndef NDEBUG

class CompoundVectorTest {
public:
	CompoundVectorTest();
};

extern CompoundVectorTest compound_vector_test;

#endif // NDEBUG
