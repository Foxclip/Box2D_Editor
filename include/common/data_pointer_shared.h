#pragma once

#include "common/data_pointer_common.h"

typedef long ref_count_t;

template<typename T, typename D = DataPointerDefaultDelete<T>>
class DataPointerShared {
public:
	DataPointerShared();
	DataPointerShared(const std::string& name, T* ptr);
	DataPointerShared(const std::string& name, T* ptr, const D& deleter);
	DataPointerShared(const DataPointerShared& dp);
	template<typename T2, typename D2>
	DataPointerShared(const DataPointerShared<T2, D2>& dp);
	DataPointerShared(DataPointerShared&& dp) noexcept;
	template<typename T2, typename D2>
	DataPointerShared(DataPointerShared<T2, D2>&& dp) noexcept;
	~DataPointerShared();
	ref_count_t use_count() const;
	ref_count_t* get_ref_count() const;
	T* get() const;
	void reset(const std::string& new_name = "", T* new_ptr = nullptr);
	void swap(DataPointerShared& dp);
	D& getDeleter();
	const D& getDeleter() const;
	const std::string& getName() const;
	void setName(const std::string& name);
	T& operator*() const;
	T* operator->() const;
	DataPointerShared& operator=(const DataPointerShared& right);
	DataPointerShared& operator=(DataPointerShared&& right);
	template<typename T2, typename D2>
	DataPointerShared& operator=(DataPointerShared<T2, D2>&& right);
	operator bool();
private:
	template<typename T2, typename D2> friend class DataPointerShared;
	std::string name = "<unnamed>";
	T* ptr = nullptr;
	ref_count_t* ref_count = nullptr;
	D deleter;

	void create(const std::string& new_name, T* new_ptr);
	void dispose();

};

template<typename T, typename... Args>
inline DataPointerShared<T> make_shared_data_pointer(const std::string& name, Args&&... args) {
	T* ptr = new T(std::forward<Args>(args)...);
	DataPointerShared<T> result(name, ptr);
	return result;
}

template<typename T, typename D>
inline DataPointerShared<T, D>::DataPointerShared() : deleter(D()) {
	this->ptr = nullptr;
}

template<typename T, typename D>
inline DataPointerShared<T, D>::DataPointerShared(const std::string& name, T* ptr) {
	this->name = name;
	create(name, ptr);
}

template<typename T, typename D>
inline DataPointerShared<T, D>::DataPointerShared(const std::string& name, T* ptr, const D& deleter) : deleter(deleter) {
	this->name = name;
	create(name, ptr);
}

template<typename T, typename D>
inline DataPointerShared<T, D>::DataPointerShared(const DataPointerShared& dp) {
	this->name = dp.name;
	this->ptr = dp.ptr;
	this->deleter = dp.deleter;
	this->ref_count = dp.ref_count;
	if (ptr) {
		++(*ref_count);
	}
}

template<typename T, typename D>
template<typename T2, typename D2>
inline DataPointerShared<T, D>::DataPointerShared(const DataPointerShared<T2, D2>& dp) {
	this->name = dp.name;
	this->ptr = dp.ptr;
	this->deleter = dp.deleter;
	this->ref_count = dp.ref_count;
	if (ptr) {
		++(*ref_count);
	}
}

template<typename T, typename D>
inline DataPointerShared<T, D>::DataPointerShared(DataPointerShared&& dp) noexcept {
	this->name = dp.name;
	dispose();
	this->ptr = dp.ptr;
	dp.ptr = nullptr;
	this->deleter = dp.deleter;
	this->ref_count = dp.ref_count;
}

template<typename T, typename D>
template<typename T2, typename D2>
inline DataPointerShared<T, D>::DataPointerShared(DataPointerShared<T2, D2>&& dp) noexcept {
	this->name = dp.name;
	dispose();
	this->ptr = dp.ptr;
	dp.ptr = nullptr;
	this->deleter = dp.deleter;
	this->ref_count = dp.ref_count;
}

template<typename T, typename D>
inline DataPointerShared<T, D>::~DataPointerShared() {
	dispose();
}

template<typename T, typename D>
ref_count_t DataPointerShared<T, D>::use_count() const {
	return ptr ? *ref_count : 0;
}

template<typename T, typename D>
inline ref_count_t* DataPointerShared<T, D>::get_ref_count() const {
	return ref_count;
}

template<typename T, typename D>
inline T* DataPointerShared<T, D>::get() const {
	return ptr;
}

template<typename T, typename D>
inline void DataPointerShared<T, D>::reset(const std::string& new_name, T* new_ptr) {
	if (ptr == new_ptr) {
		return;
	}
	dispose();
	create(new_name, new_ptr);
}

template<typename T, typename D>
inline void DataPointerShared<T, D>::swap(DataPointerShared& dp) {
	std::swap(this->name, dp.name);
	std::swap(this->ptr, dp.ptr);
	std::swap(this->ref_count, dp.ref_count);
	std::swap(this->deleter, dp.deleter);
}

template<typename T, typename D>
inline D& DataPointerShared<T, D>::getDeleter() {
	return deleter;
}

template<typename T, typename D>
inline const D& DataPointerShared<T, D>::getDeleter() const {
	return deleter;
}

template<typename T, typename D>
inline const std::string& DataPointerShared<T, D>::getName() const {
	return name;
}

template<typename T, typename D>
inline void DataPointerShared<T, D>::setName(const std::string& name) {
	this->name = name;
	auto it = data_blocks.find(ptr);
	it->second.name = name;
}

template<typename T, typename D>
inline T& DataPointerShared<T, D>::operator*() const {
	return *ptr;
}

template<typename T, typename D>
inline T* DataPointerShared<T, D>::operator->() const {
	return ptr;
}

template<typename T, typename D>
inline DataPointerShared<T, D>& DataPointerShared<T, D>::operator=(const DataPointerShared& right) {
	if (right.ptr != this->ptr) {
		this->name = right.name;
		dispose();
		this->ptr = right.ptr;
		this->deleter = right.deleter;
		this->ref_count = right.ref_count;
		if (ptr) {
			++(*ref_count);
		}
	}
	return *this;
}

template<typename T, typename D>
inline DataPointerShared<T, D>& DataPointerShared<T, D>::operator=(DataPointerShared&& right) {
	if (right.ptr != this->ptr) {
		this->name = right.name;
		dispose();
		this->ptr = right.ptr;
		right.ptr = nullptr;
		this->deleter = right.deleter;
		this->ref_count = right.ref_count;
	}
	return *this;
}

template<typename T, typename D>
template<typename T2, typename D2>
inline DataPointerShared<T, D>& DataPointerShared<T, D>::operator=(DataPointerShared<T2, D2>&& right) {
	if (right.ptr != this->ptr) {
		this->name = right.name;
		dispose();
		this->ptr = right.ptr;
		right.ptr = nullptr;
		this->deleter = right.deleter;
		this->ref_count = right.ref_count;
	}
	return *this;
}

template<typename T, typename D>
inline void DataPointerShared<T, D>::create(const std::string& new_name, T* new_ptr) {
	if (new_ptr) {
		add_to_data_blocks(new_name, reinterpret_cast<void*>(new_ptr), sizeof(T));
		ref_count = new ref_count_t(1);
		this->ptr = new_ptr;
	}
}

template<typename T, typename D>
inline void DataPointerShared<T, D>::dispose() {
	if (ptr) {
		--(*ref_count);
		if (*ref_count == 0) {
			remove_from_data_blocks(reinterpret_cast<void*>(ptr));
			deleter(ptr);
			delete ref_count;
		}
		ptr = nullptr;
	}
}
