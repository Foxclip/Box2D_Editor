#pragma once

#include <format>
#include <iostream>
#include <map>

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
	DataPointerDefaultDelete() = default;
	template<typename T2>
	DataPointerDefaultDelete(const DataPointerDefaultDelete<T2>& other);
	void operator()(T* ptr);

};

template<typename T, typename D = DataPointerDefaultDelete<T>>
class DataPointer {
public:
	DataPointer();
	DataPointer(const std::string& name, T* ptr);
	DataPointer(const std::string& name, T* ptr, const D& deleter);
	DataPointer(DataPointer&& dp) noexcept;
	template<typename T2, typename D2>
	DataPointer(DataPointer<T2, D2>&& dp) noexcept;
	~DataPointer();
	T* get() const;
	T* releaseSilent();
	T* release();
	void resetSilent(T* new_ptr);
	void reset(T* new_ptr = nullptr);
	void swap(DataPointer& dp);
	D& getDeleter();
	const D& getDeleter() const;
	const std::string& getName() const;
	T& operator*() const;
	T* operator->() const;
	DataPointer& operator=(DataPointer&& right);
	template<typename T2, typename D2>
	DataPointer& operator=(DataPointer<T2, D2>&& right);
	operator bool();

	DataPointer(const DataPointer&) = delete;
	DataPointer& operator=(const DataPointer&) = delete;

private:
	std::string name = "<unnamed>";
	T* ptr = nullptr;
	D deleter;

};

template<typename T, typename... Args>
inline DataPointer<T> make_data_pointer(const std::string& name, Args&&... args) {
	T* ptr = new T(std::forward<Args>(args)...);
	DataPointer<T> result(name, ptr);
	return result;
}

template<typename T>
template<typename T2>
inline DataPointerDefaultDelete<T>::DataPointerDefaultDelete(const DataPointerDefaultDelete<T2>& other) { }

template<typename T>
inline void DataPointerDefaultDelete<T>::operator()(T* ptr) {
	if (ptr) {
		delete ptr;
	}
}

template<typename T, typename D>
inline DataPointer<T, D>::DataPointer() : deleter(D()) {
	this->ptr = nullptr;
}

template<typename T, typename D>
inline DataPointer<T, D>::DataPointer(const std::string& name, T* ptr) : deleter(D()) {
	this->name = name;
	this->ptr = ptr;
	if (ptr) {
		auto inserted = data_blocks.insert({ reinterpret_cast<void*>(ptr), DataBlock(ptr, sizeof(T)) });
		if (!inserted.second) {
			assert(false); // pointer already in data_blocks
		}
	}
}

template<typename T, typename D>
inline DataPointer<T, D>::DataPointer(const std::string& name, T* ptr, const D& deleter) : deleter(deleter) {
	this->name = name;
	this->ptr = ptr;
	if (ptr) {
		auto inserted = data_blocks.insert({ reinterpret_cast<void*>(ptr), DataBlock(ptr, sizeof(T)) });
		if (!inserted.second) {
			assert(false); // pointer already in data_blocks
		}
	}
}

template<typename T, typename D>
inline DataPointer<T, D>::DataPointer(DataPointer&& dp) noexcept {
	this->name = dp.getName();
	resetSilent(dp.releaseSilent());
	this->deleter = dp.getDeleter();
}

template<typename T, typename D>
template<typename T2, typename D2>
inline DataPointer<T, D>::DataPointer(DataPointer<T2, D2>&& dp) noexcept {
	this->name = dp.getName();
	resetSilent(dp.releaseSilent());
	this->deleter = dp.getDeleter();
}

template<typename T, typename D>
inline DataPointer<T, D>::~DataPointer() {
	if (ptr) {
		data_blocks.erase(reinterpret_cast<void*>(ptr));
		deleter(ptr);
		assert(data_blocks.contains(ptr));
	}
}

template<typename T, typename D>
inline T* DataPointer<T, D>::get() const {
	return ptr;
}

template<typename T, typename D>
inline T* DataPointer<T, D>::releaseSilent() {
	T* result = ptr;
	ptr = nullptr;
	return result;
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
inline void DataPointer<T, D>::resetSilent(T* new_ptr) {
	if (ptr) {
		deleter(ptr);
	}
	ptr = new_ptr;
}

template<typename T, typename D>
inline void DataPointer<T, D>::reset(T* new_ptr) {
	T* old_ptr = ptr;
	if (old_ptr == new_ptr) {
		return;
	}
	if (old_ptr) {
		data_blocks.erase(reinterpret_cast<void*>(old_ptr));
		deleter(ptr);
	}
	if (new_ptr) {
		data_blocks.insert({ reinterpret_cast<void*>(new_ptr), DataBlock(new_ptr, sizeof(T)) });
	}
	ptr = new_ptr;
}

template<typename T, typename D>
inline void DataPointer<T, D>::swap(DataPointer& dp) {
	std::swap(this->name, dp.name);
	std::swap(this->ptr, dp.ptr);
	std::swap(this->deleter, dp.deleter);
}

template<typename T, typename D>
inline D& DataPointer<T, D>::getDeleter() {
	return deleter;
}

template<typename T, typename D>
inline const D& DataPointer<T, D>::getDeleter() const {
	return deleter;
}

template<typename T, typename D>
inline const std::string& DataPointer<T, D>::getName() const {
	return name;
}

template<typename T, typename D>
inline T& DataPointer<T, D>::operator*() const {
	return *ptr;
}

template<typename T, typename D>
inline T* DataPointer<T, D>::operator->() const {
	return ptr;
}

template<typename T, typename D>
inline DataPointer<T, D>& DataPointer<T, D>::operator=(DataPointer&& right) {
	this->name = right.getName();
	reset(right.releaseSilent());
	this->deleter = right.getDeleter();
	return *this;
}

template<typename T, typename D>
inline DataPointer<T, D>::operator bool() {
	return static_cast<bool>(ptr);
}

template<typename T, typename D>
template<typename T2, typename D2>
inline DataPointer<T, D>& DataPointer<T, D>::operator=(DataPointer<T2, D2>&& right) {
	this->name = right.getName();
	reset(right.releaseSilent());
	this->deleter = right.getDeleter();
	return *this;
}
