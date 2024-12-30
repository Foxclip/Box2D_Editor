#pragma once

#include "common/data_pointer_common.h"

typedef long ref_count_t;

template<typename T, typename D = DataPointerDefaultDelete<T>>
class DataPointerShared {
public:
	DataPointerShared();
	DataPointerShared(const std::string& name, T* ptr);
	DataPointerShared(const std::string& name, T* ptr, const D& deleter);
	DataPointerShared(const DataPointerShared&);
	template<typename T2, typename D2>
	DataPointerShared(const DataPointerShared<T2, D2>&);
	~DataPointerShared();
	ref_count_t use_count() const;
	T* get() const;
	T* releaseSilent();
	T* release();
	void resetSilent(T* new_ptr);
	void reset(const std::string& name = "", T* new_ptr = nullptr);
	void swap(DataPointerShared& dp);
	D& getDeleter();
	const D& getDeleter() const;
	const std::string& getName() const;
	void setName(const std::string& name);
	T& operator*() const;
	T* operator->() const;
	DataPointerShared& operator=(const DataPointerShared&);
	DataPointerShared& operator=(DataPointerShared&& right);
	template<typename T2, typename D2>
	DataPointerShared& operator=(DataPointerShared<T2, D2>&& right);
	operator bool();
private:
	std::string name = "<unnamed>";
	T* ptr = nullptr;
	ref_count_t* ref_count = nullptr;
	D deleter;

};

template<typename T, typename D>
inline DataPointerShared<T, D>::DataPointerShared() : deleter(D()) {
	this->ptr = nullptr;
}

template<typename T, typename D>
inline DataPointerShared<T, D>::DataPointerShared(const std::string& name, T* ptr) {
	this->name = name;
	this->ptr = ptr;
	if (ptr) {
		add_to_data_blocks(name, reinterpret_cast<void*>(ptr), sizeof(T));
		ref_count = new ref_count_t(1);
	}
}

template<typename T, typename D>
inline DataPointerShared<T, D>::DataPointerShared(const std::string& name, T* ptr, const D& deleter) : deleter(deleter) {
	this->name = name;
	this->ptr = ptr;
	if (ptr) {
		add_to_data_blocks(name, reinterpret_cast<void*>(ptr), sizeof(T));
		ref_count = new ref_count_t(1);
	}
}

template<typename T, typename D>
inline DataPointerShared<T, D>::~DataPointerShared() {
	if (ptr) {
		--(*ref_count);
		if (*ref_count == 0) {
			remove_from_data_blocks(reinterpret_cast<void*>(ptr));
			deleter(ptr);
			delete ref_count;
		}
	}
}

template<typename T, typename D>
ref_count_t DataPointerShared<T, D>::use_count() const {
	return ptr ? *ref_count : 0;
}