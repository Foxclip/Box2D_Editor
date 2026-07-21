#include "common/data_pointer_common.h"

namespace dp {

	std::map<void*, DataBlock> data_blocks;

	DataBlock::DataBlock(const std::string& name, void* ptr, size_t size) {
		this->name = name;
		this->ptr = ptr;
		this->size = size;
	}

	void add_to_data_blocks(const DataBlock& block) {
		auto inserted = data_blocks.insert({ block.ptr, block });
		if (!inserted.second) {
			assert(false); // pointer already in data_blocks
		}
	}

	void add_to_data_blocks(const std::string& name, void* ptr, size_t size) {
		add_to_data_blocks(DataBlock(name, ptr, size));
	}

	void remove_from_data_blocks(void* ptr) {
		assert(data_blocks.contains(ptr));
		data_blocks.erase(ptr);
	}

	std::string pointer_to_str(void* ptr) {
		ptrdiff_t ptr_address = reinterpret_cast<ptrdiff_t>(ptr);
		std::string str = std::format("0x{:0>16x}", ptr_address);
		return str;
	}

	std::string data_block_to_str(const DataBlock& block) {
		std::string result = "Name: " + block.name + " ptr: " + pointer_to_str(block.ptr) + " size: " + std::to_string(block.size);
		return result;
	}

	bool check_data_block(void* ptr) {
		return data_blocks.contains(ptr);
	}

	std::string data_block_to_str(void* ptr) {
		std::string result = "<NOT FOUND>";
		auto it = data_blocks.find(ptr);
		if (it != data_blocks.end()) {
			DataBlock& block = it->second;
			result = data_block_to_str(block);
		}
		return result;
	}

	void print_data_blocks() {
		auto it = data_blocks.begin();
		for (size_t i = 0; i < data_blocks.size(); i++) {
			DataBlock& block = it->second;
			std::cout << i << ": " << data_block_to_str(block) << "\n";
			it++;
		}
	}

}
