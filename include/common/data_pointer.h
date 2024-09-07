#pragma once

#include <format>
#include <iostream>

std::string pointer_to_str(void* ptr);

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
	this->deleter = DataPointerDefaultDelete<T>();
}

template<typename T, typename D>
inline DataPointer<T, D>::DataPointer(T* ptr, const D& deleter) {
	this->ptr = ptr;
	this->deleter = deleter;
}

template<typename T, typename D>
inline DataPointer<T, D>::DataPointer(DataPointer&& dp) noexcept {
	this->ptr = dp.ptr;
	dp.ptr = nullptr;
}

template<typename T, typename D>
inline DataPointer<T, D>::~DataPointer() {
	deleter(ptr);
}

template<typename T, typename D>
inline T* DataPointer<T, D>::get() const {
	return ptr;
}

template<typename T, typename D>
inline T* DataPointer<T, D>::release() {
	T* result = ptr;
	ptr = nullptr;
	return result;
}

template<typename T, typename D>
inline void DataPointer<T, D>::reset(T* new_ptr) {
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
