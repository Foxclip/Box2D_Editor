#include "tests/data_pointer_tests.h"
#include "tests/data_pointer_unique_tests.h"
#include "tests/data_pointer_shared_tests.h"
#include "common/data_pointer_common.h"
#include "common/utils.h"

DataPointerTests::DataPointerTests(
	const std::string& name,
	test::TestModule* parent,
	const std::vector<TestNode*>& required_nodes
) : TestModule(name, parent, required_nodes) {
	DataPointerUniqueTests* unique_tests = addModule<DataPointerUniqueTests>("DataPointerUnique");
	DataPointerSharedTests* shared_tests = addModule<DataPointerSharedTests>("DataPointerShared");
}

void DataPointerTests::checkDataBlock(test::Test& test, void* p_block, size_t p_size) {
	if (T_CHECK(dp::data_blocks.size() > 0)) {
		auto it = dp::data_blocks.find(p_block);
		if (T_CHECK(it != dp::data_blocks.end())) {
			void* ptr = it->second.ptr;
			size_t size = it->second.size;
			T_COMPARE(ptr, p_block, &utils::pointer_to_str);
			T_COMPARE(size, p_size);
		}
	}
}

void DataPointerTests::checkNoDataBlock(test::Test& test, void* p_block) {
	auto it = dp::data_blocks.find(p_block);
	T_CHECK(it == dp::data_blocks.end());
}
