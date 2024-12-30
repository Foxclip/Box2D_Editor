#pragma once

#include "common/data_pointer_common.h"

template<typename T, typename D = DataPointerDefaultDelete<T>>
class DataPointerShared {
public:
	DataPointerShared();
	DataPointerShared(const std::string& name, T* ptr);
	DataPointerShared(const std::string& name, T* ptr, const D& deleter);
	DataPointerShared(DataPointerShared&& dp) noexcept;
	template<typename T2, typename D2>
	DataPointerShared(DataPointerShared<T2, D2>&& dp) noexcept;
	DataPointerShared(const DataPointerShared&);
	~DataPointerShared();
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
	size_t* ref_count = nullptr;
	D deleter;

};
