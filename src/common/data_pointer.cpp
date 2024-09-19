#include "common/data_pointer.h"

std::map<void*, DataBlock> data_blocks;

DataBlock::DataBlock() { }

DataBlock::DataBlock(void* ptr, size_t size) {
	this->ptr = ptr;
	this->size = size;
}

void add_to_data_blocks(const DataBlock& block) {
	auto inserted = data_blocks.insert({ block.ptr, block });
	if (!inserted.second) {
		assert(false); // pointer already in data_blocks
	}
}

void add_to_data_blocks(void* ptr, size_t size) {
	add_to_data_blocks(DataBlock(ptr, size));
}

void remove_from_data_blocks(void* ptr) {
	assert(data_blocks.contains(ptr));
	data_blocks.erase(ptr);
}
