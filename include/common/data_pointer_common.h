#pragma once

#include <format>
#include <iostream>
#include <map>
#include <cassert>
#include <string>

struct DataBlock {
	DataBlock(const std::string& name, void* ptr, size_t size);
	std::string name = "<unnamed>";
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

void add_to_data_blocks(const DataBlock& block);
void add_to_data_blocks(const std::string& name, void* ptr, size_t size);
void remove_from_data_blocks(void* ptr);
std::string pointer_to_str(void* ptr);
std::string data_block_to_str(const DataBlock& block);
std::string data_block_to_str(void* ptr);
void print_data_blocks();
bool check_data_block(void* ptr);

template<typename T>
template<typename T2>
inline DataPointerDefaultDelete<T>::DataPointerDefaultDelete(const DataPointerDefaultDelete<T2>& other) { }

template<typename T>
inline void DataPointerDefaultDelete<T>::operator()(T* ptr) {
	if (ptr) {
		delete ptr;
	}
}
