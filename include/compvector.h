#pragma once

#include <vector>
#include <set>
#include <memory>

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
	const CompoundVector<T*>& getCompVector() const;
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
	CompoundVector<T*, TCmp> comp;

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
inline CompoundVector<T, TCmp>::CompoundVector() { }

template<typename T, typename TCmp>
inline CompoundVector<T, TCmp>::CompoundVector(const std::initializer_list<T>& list) {
	for (const T& value : list) {
		add(value);
	}
}

template<typename T, typename TCmp>
inline CompoundVector<T, TCmp>::CompoundVector(const std::vector<T>& vec) {
	for (const T& value : vec) {
		add(value);
	}
}

template<typename T, typename TCmp>
inline size_t CompoundVector<T, TCmp>::size() const {
	return vector.size();
}

template<typename T, typename TCmp>
inline bool CompoundVector<T, TCmp>::add(const T& value) {
	auto inserted = set.insert(value);
	if (inserted.second) {
		vector.push_back(value);
		return true;
	}
	return false;
}

template<typename T, typename TCmp>
inline bool CompoundVector<T, TCmp>::insert(std::vector<T>::const_iterator where, const T& value) {
	auto inserted = set.insert(value);
	if (inserted.second) {
		vector.insert(where, value);
		return true;
	}
	return false;
}

template<typename T, typename TCmp>
template<std::incrementable TIter>
inline size_t CompoundVector<T, TCmp>::insert(std::vector<T>::const_iterator where, TIter first, TIter last) {
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
inline ptrdiff_t CompoundVector<T, TCmp>::remove(const T& value) {
	size_t removed = set.erase(value);
	if (removed > 0) {
		ptrdiff_t index = getIndex(value);
		vector.erase(vector.begin() + index);
		return index;
	}
	return -1;
}

template<typename T, typename TCmp>
inline void CompoundVector<T, TCmp>::removeAt(size_t index) {
	T value = vector[index];
	vector.erase(vector.begin() + index);
	set.erase(value);
}

template<typename T, typename TCmp>
inline void CompoundVector<T, TCmp>::reverse() {
	std::reverse(vector.begin(), vector.end());
}

template<typename T, typename TCmp>
inline std::vector<T>::const_iterator CompoundVector<T, TCmp>::begin() const {
	return vector.begin();
}

template<typename T, typename TCmp>
inline std::vector<T>::const_iterator CompoundVector<T, TCmp>::end() const {
	return vector.end();
}

template<typename T, typename TCmp>
inline std::vector<T>::const_reverse_iterator CompoundVector<T, TCmp>::rbegin() const {
	return vector.rbegin();
}

template<typename T, typename TCmp>
inline std::vector<T>::const_reverse_iterator CompoundVector<T, TCmp>::rend() const {
	return vector.rend();
}

template<typename T, typename TCmp>
inline T& CompoundVector<T, TCmp>::front() {
	return vector.front();
}

template<typename T, typename TCmp>
inline const T& CompoundVector<T, TCmp>::front() const {
	return vector.front();
}

template<typename T, typename TCmp>
inline T& CompoundVector<T, TCmp>::back() {
	return vector.back();
}

template<typename T, typename TCmp>
inline const T& CompoundVector<T, TCmp>::back() const {
	return vector.back();
}

template<typename T, typename TCmp>
inline T& CompoundVector<T, TCmp>::at(size_t index) {
	return vector[index];
}

template<typename T, typename TCmp>
inline const T& CompoundVector<T, TCmp>::at(size_t index) const {
	return vector[index];
}

template<typename T, typename TCmp>
inline ptrdiff_t CompoundVector<T, TCmp>::getIndex(const T& value) const {
	for (size_t i = 0; i < vector.size(); i++) {
		if (vector[i] == value) {
			return i;
		}
	}
	return -1;
}

template<typename T, typename TCmp>
inline const std::vector<T>& CompoundVector<T, TCmp>::getVector() const {
	return vector;
}

template<typename T, typename TCmp>
inline const std::set<T, TCmp>& CompoundVector<T, TCmp>::getSet() const {
	return set;
}

template<typename T, typename TCmp>
inline T& CompoundVector<T, TCmp>::operator[](size_t index) {
	return at(index);
}

template<typename T, typename TCmp>
inline const T& CompoundVector<T, TCmp>::operator[](size_t index) const {
	return at(index);
}

template<typename T, typename TCmp>
inline std::set<T, TCmp>::iterator CompoundVector<T, TCmp>::find(const T& value) const {
	return set.find(value);
}

template<typename T, typename TCmp>
inline bool CompoundVector<T, TCmp>::contains(const T& value) const {
	return set.contains(value);
}

template<typename T, typename TCmp>
inline void CompoundVector<T, TCmp>::clear() {
	vector = std::vector<T>();
	set = std::set<T, TCmp>();
}

template<typename T, typename TCmp>
inline CompoundVector<T, TCmp>::operator std::vector<T>() const {
	return vector;
}

template<typename T, typename TCmp>
template<Vectorlike TCont>
inline bool CompoundVector<T, TCmp>::operator==(const TCont& other) const {
	return compare(*this, other);
}

template<typename T, typename TCmp>
inline CompoundVectorUptr<T, TCmp>::CompoundVectorUptr() { }

template<typename T, typename TCmp>
inline CompoundVectorUptr<T, TCmp>::CompoundVectorUptr(const std::initializer_list<T>& list) {
	for (const T& value : list) {
		std::unique_ptr<T> uptr = std::make_unique<T>(value);
		add(std::move(uptr));
	}
}

template<typename T, typename TCmp>
inline CompoundVectorUptr<T, TCmp>::CompoundVectorUptr(const std::initializer_list<T*>& list) {
	for (const T* ptr : list) {
		add(ptr);
	}
}

template<typename T, typename TCmp>
inline CompoundVectorUptr<T, TCmp>::CompoundVectorUptr(const std::vector<T*>& vec) {
	for (const T* value : vec) {
		std::unique_ptr<T> uptr = std::unique_ptr<T>(const_cast<T*>(value));
		add(std::move(uptr));
	}
}

template<typename T, typename TCmp>
inline size_t CompoundVectorUptr<T, TCmp>::size() const {
	return comp.size();
}

template<typename T, typename TCmp>
inline bool CompoundVectorUptr<T, TCmp>::add(const T& value) {
	const T* ptr = &value;
	bool added = comp.add(const_cast<T*>(ptr));
	if (added) {
		std::unique_ptr<T> uptr = std::make_unique<T>(value);
		uptrs.push_back(std::move(uptr));
		return true;
	}
	return false;
}

template<typename T, typename TCmp>
inline bool CompoundVectorUptr<T, TCmp>::add(const T* ptr) {
	bool added = comp.add(const_cast<T*>(ptr));
	if (added) {
		std::unique_ptr<T> uptr = std::unique_ptr<T>(const_cast<T*>(ptr));
		uptrs.push_back(std::move(uptr));
		return true;
	}
	return false;
}

template<typename T, typename TCmp>
inline bool CompoundVectorUptr<T, TCmp>::add(std::unique_ptr<T> value) {
	T* ptr = value.get();
	bool added = comp.add(ptr);
	if (added) {
		uptrs.push_back(std::move(value));
		return true;
	}
	return false;
}

template<typename T, typename TCmp>
inline bool CompoundVectorUptr<T, TCmp>::insert(std::vector<T*>::const_iterator where, std::unique_ptr<T> value) {
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
inline size_t CompoundVectorUptr<T, TCmp>::insert(std::vector<T*>::const_iterator where, TIter first, TIter last) {
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
inline bool CompoundVectorUptr<T, TCmp>::operator==(const TCont& other) const {
	return compare(*this, other);
}

template<typename T, typename TCmp>
inline ptrdiff_t CompoundVectorUptr<T, TCmp>::remove(T* value) {
	ptrdiff_t removed_index = comp.remove(value);
	if (removed_index >= 0) {
		uptrs.erase(uptrs.begin() + removed_index);
		return removed_index;
	}
	return -1;
}

template<typename T, typename TCmp>
inline void CompoundVectorUptr<T, TCmp>::removeAt(size_t index) {
	uptrs.erase(uptrs.begin() + index);
	comp.removeAt(index);
}

template<typename T, typename TCmp>
inline void CompoundVectorUptr<T, TCmp>::reverse() {
	std::reverse(uptrs.begin(), uptrs.end());
	comp.reverse();
}

template<typename T, typename TCmp>
inline std::vector<T*>::const_iterator CompoundVectorUptr<T, TCmp>::begin() const {
	return comp.begin();
}

template<typename T, typename TCmp>
inline std::vector<T*>::const_iterator CompoundVectorUptr<T, TCmp>::end() const {
	return comp.end();
}

template<typename T, typename TCmp>
inline std::vector<T*>::const_reverse_iterator CompoundVectorUptr<T, TCmp>::rbegin() const {
	return comp.rbegin();
}

template<typename T, typename TCmp>
inline std::vector<T*>::const_reverse_iterator CompoundVectorUptr<T, TCmp>::rend() const {
	return comp.rend();
}

template<typename T, typename TCmp>
inline T* CompoundVectorUptr<T, TCmp>::front() {
	return comp.front();
}

template<typename T, typename TCmp>
inline T* CompoundVectorUptr<T, TCmp>::front() const {
	return comp.front();
}

template<typename T, typename TCmp>
inline T* CompoundVectorUptr<T, TCmp>::back() {
	return comp.back();
}

template<typename T, typename TCmp>
inline T* CompoundVectorUptr<T, TCmp>::back() const {
	return comp.back();
}

template<typename T, typename TCmp>
inline T* CompoundVectorUptr<T, TCmp>::at(size_t index) {
	return comp[index];
}

template<typename T, typename TCmp>
inline T* CompoundVectorUptr<T, TCmp>::at(size_t index) const {
	return comp[index];
}

template<typename T, typename TCmp>
inline ptrdiff_t CompoundVectorUptr<T, TCmp>::getIndex(const T* value) const {
	for (size_t i = 0; i < comp.size(); i++) {
		if (comp[i] == value) {
			return i;
		}
	}
	return -1;
}

template<typename T, typename TCmp>
inline const CompoundVector<T*>& CompoundVectorUptr<T, TCmp>::getCompVector() const {
	return comp;
}

template<typename T, typename TCmp>
inline const std::vector<T*>& CompoundVectorUptr<T, TCmp>::getVector() const {
	return comp.getVector();
}

template<typename T, typename TCmp>
inline const std::set<T*, TCmp>& CompoundVectorUptr<T, TCmp>::getSet() const {
	return comp.getSet();
}

template<typename T, typename TCmp>
inline T* CompoundVectorUptr<T, TCmp>::operator[](size_t index) {
	return comp[index];
}

template<typename T, typename TCmp>
inline T* CompoundVectorUptr<T, TCmp>::operator[](size_t index) const {
	return comp[index];
}

template<typename T, typename TCmp>
inline std::set<T*, TCmp>::iterator CompoundVectorUptr<T, TCmp>::find(const T* value) const {
	return comp.find(const_cast<T*>(value));
}

template<typename T, typename TCmp>
inline bool CompoundVectorUptr<T, TCmp>::contains(const T* value) const {
	return comp.contains(const_cast<T*>(value));
}

template<typename T, typename TCmp>
inline void CompoundVectorUptr<T, TCmp>::clear() {
	uptrs = std::vector<std::unique_ptr<T>>();
	comp.clear();
}

template<typename T, typename TCmp>
inline CompoundVectorUptr<T, TCmp>::operator std::vector<T*>() const {
	return comp.getVector();
}

#ifndef NDEBUG

class CompoundVectorTest {
public:
	CompoundVectorTest();
	void test_CompoundVector();
	void test_CompoundVectorUptr();
};

#endif // NDEBUG
