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
