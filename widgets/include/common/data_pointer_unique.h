#pragma once

#include "data_pointer_common.h"

namespace dp {

	template<typename T, typename D = DataPointerDefaultDelete<T>>
	class DataPointerUnique {
	public:
		DataPointerUnique();
		DataPointerUnique(const std::string& name, T* ptr);
		DataPointerUnique(const std::string& name, T* ptr, const D& deleter);
		DataPointerUnique(DataPointerUnique&& dp) noexcept;
		template<typename T2, typename D2>
		DataPointerUnique(DataPointerUnique<T2, D2>&& dp) noexcept;
		~DataPointerUnique();
		T* get() const;
		T* release();
		void reset(const std::string& name = "", T* new_ptr = nullptr);
		void swap(DataPointerUnique& dp);
		D& getDeleter();
		const D& getDeleter() const;
		const std::string& getName() const;
		void setName(const std::string& name);
		T& operator*() const;
		T* operator->() const;
		DataPointerUnique& operator=(DataPointerUnique&& right);
		template<typename T2, typename D2>
		DataPointerUnique& operator=(DataPointerUnique<T2, D2>&& right);
		operator bool();

		DataPointerUnique(const DataPointerUnique&) = delete;
		DataPointerUnique& operator=(const DataPointerUnique&) = delete;

	private:
		template<typename T2, typename D2> friend class DataPointerUnique;
		std::string name = "<unnamed>";
		T* ptr = nullptr;
		D deleter;

		void create(const std::string& new_name, T* new_ptr);
		void dispose();

	};

	template<typename T, typename... Args>
	inline DataPointerUnique<T> make_data_pointer(const std::string& name, Args&&... args) {
		T* ptr = new T(std::forward<Args>(args)...);
		DataPointerUnique<T> result(name, ptr);
		return result;
	}

	template<typename T, typename D>
	inline DataPointerUnique<T, D>::DataPointerUnique() : deleter(D()) {
		this->ptr = nullptr;
	}

	template<typename T, typename D>
	inline DataPointerUnique<T, D>::DataPointerUnique(const std::string& name, T* ptr) : deleter(D()) {
		this->name = name;
		create(name, ptr);
	}

	template<typename T, typename D>
	inline DataPointerUnique<T, D>::DataPointerUnique(const std::string& name, T* ptr, const D& deleter) : deleter(deleter) {
		this->name = name;
		create(name, ptr);
	}

	template<typename T, typename D>
	inline DataPointerUnique<T, D>::DataPointerUnique(DataPointerUnique&& dp) noexcept {
		this->name = dp.name;
		this->ptr = dp.ptr;
		dp.ptr = nullptr;
		this->deleter = dp.deleter;
	}

	template<typename T, typename D>
	template<typename T2, typename D2>
	inline DataPointerUnique<T, D>::DataPointerUnique(DataPointerUnique<T2, D2>&& dp) noexcept {
		this->name = dp.name;
		this->ptr = dp.ptr;
		dp.ptr = nullptr;
		this->deleter = dp.deleter;
	}

	template<typename T, typename D>
	inline DataPointerUnique<T, D>::~DataPointerUnique() {
		dispose();
	}

	template<typename T, typename D>
	inline T* DataPointerUnique<T, D>::get() const {
		return ptr;
	}

	template<typename T, typename D>
	inline T* DataPointerUnique<T, D>::release() {
		T* result = ptr;
		if (ptr) {
			remove_from_data_blocks(reinterpret_cast<void*>(ptr));
		}
		ptr = nullptr;
		return result;
	}

	template<typename T, typename D>
	inline void DataPointerUnique<T, D>::reset(const std::string& new_name, T* new_ptr) {
		if (ptr == new_ptr) {
			return;
		}
		dispose();
		create(new_name, new_ptr);
	}

	template<typename T, typename D>
	inline void DataPointerUnique<T, D>::swap(DataPointerUnique& dp) {
		std::swap(this->name, dp.name);
		std::swap(this->ptr, dp.ptr);
		std::swap(this->deleter, dp.deleter);
	}

	template<typename T, typename D>
	inline D& DataPointerUnique<T, D>::getDeleter() {
		return deleter;
	}

	template<typename T, typename D>
	inline const D& DataPointerUnique<T, D>::getDeleter() const {
		return deleter;
	}

	template<typename T, typename D>
	inline const std::string& DataPointerUnique<T, D>::getName() const {
		return name;
	}

	template<typename T, typename D>
	inline void DataPointerUnique<T, D>::setName(const std::string& name) {
		this->name = name;
		auto it = data_blocks.find(ptr);
		it->second.name = name;
	}

	template<typename T, typename D>
	inline T& DataPointerUnique<T, D>::operator*() const {
		return *ptr;
	}

	template<typename T, typename D>
	inline T* DataPointerUnique<T, D>::operator->() const {
		return ptr;
	}

	template<typename T, typename D>
	inline DataPointerUnique<T, D>& DataPointerUnique<T, D>::operator=(DataPointerUnique&& right) {
		if (right.ptr != this->ptr) {
			this->name = right.name;
			dispose();
			this->ptr = right.ptr;
			right.ptr = nullptr;
			this->deleter = right.deleter;
		}
		return *this;
	}

	template<typename T, typename D>
	template<typename T2, typename D2>
	inline DataPointerUnique<T, D>& DataPointerUnique<T, D>::operator=(DataPointerUnique<T2, D2>&& right) {
		if (right.ptr != this->ptr) {
			this->name = right.name;
			dispose();
			this->ptr = right.ptr;
			right.ptr = nullptr;
			this->deleter = right.deleter;
		}
		return *this;
	}

	template<typename T, typename D>
	inline DataPointerUnique<T, D>::operator bool() {
		return static_cast<bool>(ptr);
	}

	template<typename T, typename D>
	inline void DataPointerUnique<T, D>::create(const std::string& new_name, T* new_ptr) {
		if (new_ptr) {
			add_to_data_blocks(new_name, reinterpret_cast<void*>(new_ptr), sizeof(T));
			this->ptr = new_ptr;
		}
	}

	template<typename T, typename D>
	inline void DataPointerUnique<T, D>::dispose() {
		if (ptr) {
			remove_from_data_blocks(reinterpret_cast<void*>(ptr));
			deleter(ptr);
			ptr = nullptr;
		}
	}

}
