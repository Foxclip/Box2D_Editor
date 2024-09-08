#pragma once

#include <format>
#include <iostream>
#include <map>

std::string pointer_to_str(void* ptr);

struct DataBlock {
	DataBlock();
	DataBlock(void* ptr, size_t size);
	void* ptr = nullptr;
	size_t size = 0;
};

extern std::map<void*, DataBlock> data_blocks;

template<typename T>
class DataPointerDefaultDelete {
public:
	void operator()(T* ptr);

};

template<typename T, typename D = DataPointerDefaultDelete<T>>
class DataPointer {
public:
	DataPointer(T* ptr);
	DataPointer(T* ptr, const D& deleter);
	DataPointer(DataPointer&& dp) noexcept;
	~DataPointer();
	T* get() const;
	T* release();
	void reset(T* new_ptr);
	T& operator*() const;
	T* operator->() const;

	DataPointer(const DataPointer&) = delete;
	DataPointer& operator=(const DataPointer&) = delete;

private:
	T* ptr = nullptr;
	D deleter;

};

template<typename T, typename... Args>
DataPointer<T> make_data_pointer(Args&&... args);

template<typename T>
inline void DataPointerDefaultDelete<T>::operator()(T* ptr) {
	//std::cout << "Deleting DataPointer (default): " << pointer_to_str(reinterpret_cast<void*>(ptr)) << "\n";
	if (ptr) {
		delete ptr;
	}
}

template<typename T, typename D>
inline DataPointer<T, D>::DataPointer(T* ptr) {
	//std::cout << "Creating DataPointer: " << pointer_to_str(reinterpret_cast<void*>(ptr)) << "\n";
	this->ptr = ptr;
	if (ptr) {
		data_blocks.insert({ reinterpret_cast<void*>(ptr), DataBlock(ptr, sizeof(T)) });
	}
	this->deleter = DataPointerDefaultDelete<T>();
}

template<typename T, typename D>
inline DataPointer<T, D>::DataPointer(T* ptr, const D& deleter) {
	this->ptr = ptr;
	if (ptr) {
		data_blocks.insert({ reinterpret_cast<void*>(ptr), DataBlock(ptr, sizeof(T)) });
	}
	this->deleter = deleter;
}

template<typename T, typename D>
inline DataPointer<T, D>::DataPointer(DataPointer&& dp) noexcept {
	this->ptr = dp.ptr;
	dp.ptr = nullptr;
}

template<typename T, typename D>
inline DataPointer<T, D>::~DataPointer() {
	if (ptr) {
		data_blocks.erase(reinterpret_cast<void*>(ptr));
	}
	deleter(ptr);
}

template<typename T, typename D>
inline T* DataPointer<T, D>::get() const {
	return ptr;
}

template<typename T, typename D>
inline T* DataPointer<T, D>::release() {
	T* result = ptr;
	if (ptr) {
		data_blocks.erase(reinterpret_cast<void*>(ptr));
	}
	ptr = nullptr;
	return result;
}

template<typename T, typename D>
inline void DataPointer<T, D>::reset(T* new_ptr) {
	T* old_ptr = ptr;
	if (old_ptr == new_ptr) {
		return;
	}
	if (old_ptr) {
		data_blocks.erase(reinterpret_cast<void*>(old_ptr));
	}
	if (new_ptr) {
		data_blocks.insert({ reinterpret_cast<void*>(new_ptr), DataBlock(new_ptr, sizeof(T)) });
	}
	ptr = new_ptr;
}

template<typename T, typename D>
inline T& DataPointer<T, D>::operator*() const {
	return *ptr;
}

template<typename T, typename D>
inline T* DataPointer<T, D>::operator->() const {
	return ptr;
}

template<typename T, typename... Args>
inline DataPointer<T> make_data_pointer(Args&&... args) {
	T* ptr = new T(std::forward<Args>(args)...);
	DataPointer<T> result(ptr);
	return result;
}
