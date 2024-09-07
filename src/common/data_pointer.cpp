#include "common/data_pointer.h"

std::map<void*, DataBlock> data_blocks;

std::string pointer_to_str(void* ptr) {
	ptrdiff_t ptr_address = reinterpret_cast<ptrdiff_t>(ptr);
	std::string str = std::format("0x{:0>16x}", ptr_address);
	return str;
}

DataBlock::DataBlock() { }

DataBlock::DataBlock(void* ptr, size_t size) {
	this->ptr = ptr;
	this->size = size;
}
