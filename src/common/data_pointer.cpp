#include "common/data_pointer.h"

std::map<void*, DataBlock> data_blocks;

DataBlock::DataBlock() { }

DataBlock::DataBlock(void* ptr, size_t size) {
	this->ptr = ptr;
	this->size = size;
}
