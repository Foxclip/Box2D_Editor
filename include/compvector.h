#pragma once

#include <vector>
#include <set>
#include <memory>
#include "test.h"

template<typename T, typename U>
concept NotSameAs = !std::same_as<T, U>;

template<typename T>
concept Vectorlike = requires(T x, size_t i) {
	{ x.size() } -> std::convertible_to<size_t>;
	{ x[i] } -> NotSameAs<void>;
};

template<Vectorlike TLeft, Vectorlike TRight>
bool compare(const TLeft& left, const TRight& right);

template<typename T, typename TCmp = std::less<T>>
class CompVector {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	CompVector();
	CompVector(const std::initializer_list<T>& list);
	CompVector(const std::vector<T>& vec);
	size_t size() const;
	bool add(const T& value);
	bool insert(std::vector<T>::const_iterator where, const T& value);
	template<std::incrementable TIter>
	size_t insert(std::vector<T>::const_iterator where, TIter first, TIter last);
	ptrdiff_t remove(const T& value);
	void removeAt(size_t index);
	void reverse();
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
	const std::set<T, TCmp>& getSet() const;
	T& operator[](size_t index);
	const T& operator[](size_t index) const;
	std::set<T, TCmp>::iterator find(const T& value) const;
	bool contains(const T& value) const;
	void clear();
	operator std::vector<T>() const;
	template<Vectorlike TCont>
	bool operator==(const TCont& other) const;

private:
	std::vector<T> vector;
	std::set<T, TCmp> set;
};

template<typename T, typename TCmp = std::less<T*>>
class CompVectorUptr {
public:
	using value_type = T*;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	CompVectorUptr();
	CompVectorUptr(const std::initializer_list<T>& list);
	CompVectorUptr(const std::initializer_list<T*>& list);
	CompVectorUptr(const std::vector<T*>& vec);
	size_t size() const;
	T* add(const T& value);
	T* add(const T* ptr);
	T* add(std::unique_ptr<T> value);
	bool insert(std::vector<T*>::const_iterator where, std::unique_ptr<T> value);
	template<std::incrementable TIter>
	size_t insert(std::vector<T*>::const_iterator where, TIter first, TIter last);
	ptrdiff_t remove(T* value);
	void removeAt(size_t index);
	void reverse();
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
	const CompVector<T*>& getCompVector() const;
	const std::vector<T*>& getVector() const;
	const std::set<T*, TCmp>& getSet() const;
	T* operator[](size_t index);
	T* operator[](size_t index) const;
	std::set<T*, TCmp>::iterator find(const T* value) const;
	bool contains(const T* value) const;
	void clear();
	operator std::vector<T*>() const;
	template<Vectorlike TCont>
	bool operator==(const TCont& other) const;

private:
	std::vector<std::unique_ptr<T>> uptrs;
	CompVector<T*, TCmp> comp;

};

template<Vectorlike TLeft, Vectorlike TRight>
inline bool compare(const TLeft& left, const TRight& right) {
	if (left.size() != right.size()) {
		return false;
	}
	for (size_t i = 0; i < left.size(); i++) {
		if (left[i] != right[i]) {
			return false;
		}
	}
	return true;
}

template<typename T, typename TCmp>
inline CompVector<T, TCmp>::CompVector() { }

template<typename T, typename TCmp>
inline CompVector<T, TCmp>::CompVector(const std::initializer_list<T>& list) {
	for (const T& value : list) {
		add(value);
	}
}

template<typename T, typename TCmp>
inline CompVector<T, TCmp>::CompVector(const std::vector<T>& vec) {
	for (const T& value : vec) {
		add(value);
	}
}

template<typename T, typename TCmp>
inline size_t CompVector<T, TCmp>::size() const {
	return vector.size();
}

template<typename T, typename TCmp>
inline bool CompVector<T, TCmp>::add(const T& value) {
	auto inserted = set.insert(value);
	if (inserted.second) {
		vector.push_back(value);
		return true;
	}
	return false;
}

template<typename T, typename TCmp>
inline bool CompVector<T, TCmp>::insert(std::vector<T>::const_iterator where, const T& value) {
	auto inserted = set.insert(value);
	if (inserted.second) {
		vector.insert(where, value);
		return true;
	}
	return false;
}

template<typename T, typename TCmp>
template<std::incrementable TIter>
inline size_t CompVector<T, TCmp>::insert(std::vector<T>::const_iterator where, TIter first, TIter last) {
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

template<typename T, typename TCmp>
inline ptrdiff_t CompVector<T, TCmp>::remove(const T& value) {
	size_t removed = set.erase(value);
	if (removed > 0) {
		ptrdiff_t index = getIndex(value);
		vector.erase(vector.begin() + index);
		return index;
	}
	return -1;
}

template<typename T, typename TCmp>
inline void CompVector<T, TCmp>::removeAt(size_t index) {
	T value = vector[index];
	vector.erase(vector.begin() + index);
	set.erase(value);
}

template<typename T, typename TCmp>
inline void CompVector<T, TCmp>::reverse() {
	std::reverse(vector.begin(), vector.end());
}

template<typename T, typename TCmp>
inline std::vector<T>::const_iterator CompVector<T, TCmp>::begin() const {
	return vector.begin();
}

template<typename T, typename TCmp>
inline std::vector<T>::const_iterator CompVector<T, TCmp>::end() const {
	return vector.end();
}

template<typename T, typename TCmp>
inline std::vector<T>::const_reverse_iterator CompVector<T, TCmp>::rbegin() const {
	return vector.rbegin();
}

template<typename T, typename TCmp>
inline std::vector<T>::const_reverse_iterator CompVector<T, TCmp>::rend() const {
	return vector.rend();
}

template<typename T, typename TCmp>
inline T& CompVector<T, TCmp>::front() {
	return vector.front();
}

template<typename T, typename TCmp>
inline const T& CompVector<T, TCmp>::front() const {
	return vector.front();
}

template<typename T, typename TCmp>
inline T& CompVector<T, TCmp>::back() {
	return vector.back();
}

template<typename T, typename TCmp>
inline const T& CompVector<T, TCmp>::back() const {
	return vector.back();
}

template<typename T, typename TCmp>
inline T& CompVector<T, TCmp>::at(size_t index) {
	return vector[index];
}

template<typename T, typename TCmp>
inline const T& CompVector<T, TCmp>::at(size_t index) const {
	return vector[index];
}

template<typename T, typename TCmp>
inline ptrdiff_t CompVector<T, TCmp>::getIndex(const T& value) const {
	for (size_t i = 0; i < vector.size(); i++) {
		if (vector[i] == value) {
			return i;
		}
	}
	return -1;
}

template<typename T, typename TCmp>
inline const std::vector<T>& CompVector<T, TCmp>::getVector() const {
	return vector;
}

template<typename T, typename TCmp>
inline const std::set<T, TCmp>& CompVector<T, TCmp>::getSet() const {
	return set;
}

template<typename T, typename TCmp>
inline T& CompVector<T, TCmp>::operator[](size_t index) {
	return at(index);
}

template<typename T, typename TCmp>
inline const T& CompVector<T, TCmp>::operator[](size_t index) const {
	return at(index);
}

template<typename T, typename TCmp>
inline std::set<T, TCmp>::iterator CompVector<T, TCmp>::find(const T& value) const {
	return set.find(value);
}

template<typename T, typename TCmp>
inline bool CompVector<T, TCmp>::contains(const T& value) const {
	return set.contains(value);
}

template<typename T, typename TCmp>
inline void CompVector<T, TCmp>::clear() {
	vector = std::vector<T>();
	set = std::set<T, TCmp>();
}

template<typename T, typename TCmp>
inline CompVector<T, TCmp>::operator std::vector<T>() const {
	return vector;
}

template<typename T, typename TCmp>
template<Vectorlike TCont>
inline bool CompVector<T, TCmp>::operator==(const TCont& other) const {
	return compare(*this, other);
}

template<typename T, typename TCmp>
inline CompVectorUptr<T, TCmp>::CompVectorUptr() { }

template<typename T, typename TCmp>
inline CompVectorUptr<T, TCmp>::CompVectorUptr(const std::initializer_list<T>& list) {
	for (const T& value : list) {
		std::unique_ptr<T> uptr = std::make_unique<T>(value);
		add(std::move(uptr));
	}
}

template<typename T, typename TCmp>
inline CompVectorUptr<T, TCmp>::CompVectorUptr(const std::initializer_list<T*>& list) {
	for (const T* ptr : list) {
		add(ptr);
	}
}

template<typename T, typename TCmp>
inline CompVectorUptr<T, TCmp>::CompVectorUptr(const std::vector<T*>& vec) {
	for (const T* value : vec) {
		std::unique_ptr<T> uptr = std::unique_ptr<T>(const_cast<T*>(value));
		add(std::move(uptr));
	}
}

template<typename T, typename TCmp>
inline size_t CompVectorUptr<T, TCmp>::size() const {
	return comp.size();
}

template<typename T, typename TCmp>
inline T* CompVectorUptr<T, TCmp>::add(const T& value) {
	std::unique_ptr<T> uptr = std::make_unique<T>(value);
	T* ptr = uptr.get();
	comp.add(ptr);
	uptrs.push_back(std::move(uptr));
	return ptr;
}

template<typename T, typename TCmp>
inline T* CompVectorUptr<T, TCmp>::add(const T* ptr) {
	T* nptr = const_cast<T*>(ptr);
	bool added = comp.add(nptr);
	if (added) {
		std::unique_ptr<T> uptr = std::unique_ptr<T>(nptr);
		uptrs.push_back(std::move(uptr));
		return nptr;
	}
	return nullptr;
}

template<typename T, typename TCmp>
inline T* CompVectorUptr<T, TCmp>::add(std::unique_ptr<T> value) {
	T* ptr = value.get();
	bool added = comp.add(ptr);
	if (added) {
		uptrs.push_back(std::move(value));
		return ptr;
	}
	return nullptr;
}

template<typename T, typename TCmp>
inline bool CompVectorUptr<T, TCmp>::insert(std::vector<T*>::const_iterator where, std::unique_ptr<T> value) {
	size_t offset = where - comp.begin();
	bool inserted = comp.insert(where, value.get());
	if (inserted) {
		uptrs.insert(uptrs.begin() + offset, std::move(value));
		return true;
	}
	value.release();
	return false;
}

template<typename T, typename TCmp>
template<std::incrementable TIter>
inline size_t CompVectorUptr<T, TCmp>::insert(std::vector<T*>::const_iterator where, TIter first, TIter last) {
	size_t offset = where - comp.begin();
	TIter iter_other(first);
	size_t inserted_count = 0;
	while (iter_other != last) {
		if (insert(comp.begin() + offset, std::move(const_cast<std::unique_ptr<T>&>(*iter_other)))) {
			inserted_count++;
			offset++;
		}
		iter_other++;
	}
	return inserted_count;
}

template<typename T, typename TCmp>
template<Vectorlike TCont>
inline bool CompVectorUptr<T, TCmp>::operator==(const TCont& other) const {
	return compare(*this, other);
}

template<typename T, typename TCmp>
inline ptrdiff_t CompVectorUptr<T, TCmp>::remove(T* value) {
	ptrdiff_t removed_index = comp.remove(value);
	if (removed_index >= 0) {
		uptrs.erase(uptrs.begin() + removed_index);
		return removed_index;
	}
	return -1;
}

template<typename T, typename TCmp>
inline void CompVectorUptr<T, TCmp>::removeAt(size_t index) {
	uptrs.erase(uptrs.begin() + index);
	comp.removeAt(index);
}

template<typename T, typename TCmp>
inline void CompVectorUptr<T, TCmp>::reverse() {
	std::reverse(uptrs.begin(), uptrs.end());
	comp.reverse();
}

template<typename T, typename TCmp>
inline std::vector<T*>::const_iterator CompVectorUptr<T, TCmp>::begin() const {
	return comp.begin();
}

template<typename T, typename TCmp>
inline std::vector<T*>::const_iterator CompVectorUptr<T, TCmp>::end() const {
	return comp.end();
}

template<typename T, typename TCmp>
inline std::vector<T*>::const_reverse_iterator CompVectorUptr<T, TCmp>::rbegin() const {
	return comp.rbegin();
}

template<typename T, typename TCmp>
inline std::vector<T*>::const_reverse_iterator CompVectorUptr<T, TCmp>::rend() const {
	return comp.rend();
}

template<typename T, typename TCmp>
inline T* CompVectorUptr<T, TCmp>::front() {
	return comp.front();
}

template<typename T, typename TCmp>
inline T* CompVectorUptr<T, TCmp>::front() const {
	return comp.front();
}

template<typename T, typename TCmp>
inline T* CompVectorUptr<T, TCmp>::back() {
	return comp.back();
}

template<typename T, typename TCmp>
inline T* CompVectorUptr<T, TCmp>::back() const {
	return comp.back();
}

template<typename T, typename TCmp>
inline T* CompVectorUptr<T, TCmp>::at(size_t index) {
	return comp[index];
}

template<typename T, typename TCmp>
inline T* CompVectorUptr<T, TCmp>::at(size_t index) const {
	return comp[index];
}

template<typename T, typename TCmp>
inline ptrdiff_t CompVectorUptr<T, TCmp>::getIndex(const T* value) const {
	for (size_t i = 0; i < comp.size(); i++) {
		if (comp[i] == value) {
			return i;
		}
	}
	return -1;
}

template<typename T, typename TCmp>
inline const CompVector<T*>& CompVectorUptr<T, TCmp>::getCompVector() const {
	return comp;
}

template<typename T, typename TCmp>
inline const std::vector<T*>& CompVectorUptr<T, TCmp>::getVector() const {
	return comp.getVector();
}

template<typename T, typename TCmp>
inline const std::set<T*, TCmp>& CompVectorUptr<T, TCmp>::getSet() const {
	return comp.getSet();
}

template<typename T, typename TCmp>
inline T* CompVectorUptr<T, TCmp>::operator[](size_t index) {
	return comp[index];
}

template<typename T, typename TCmp>
inline T* CompVectorUptr<T, TCmp>::operator[](size_t index) const {
	return comp[index];
}

template<typename T, typename TCmp>
inline std::set<T*, TCmp>::iterator CompVectorUptr<T, TCmp>::find(const T* value) const {
	return comp.find(const_cast<T*>(value));
}

template<typename T, typename TCmp>
inline bool CompVectorUptr<T, TCmp>::contains(const T* value) const {
	return comp.contains(const_cast<T*>(value));
}

template<typename T, typename TCmp>
inline void CompVectorUptr<T, TCmp>::clear() {
	uptrs = std::vector<std::unique_ptr<T>>();
	comp.clear();
}

template<typename T, typename TCmp>
inline CompVectorUptr<T, TCmp>::operator std::vector<T*>() const {
	return comp.getVector();
}

#ifndef NDEBUG

class TestModule;

class CompVectorTests : public test::TestModule {
public:
	struct CompVectorTestList : public test::TestList {
		CompVectorTestList();
	};
	struct CompVectorUptrTestList : public test::TestList {
		CompVectorUptrTestList();
	};
	CompVectorTests();

protected:
	void createTestLists() override;
};

#endif // NDEBUG
