#include "tests/data_pointer_shared_tests.h"
#include "common/data_pointer_shared.h"
#include <functional>
#include "common/utils.h"

DataPointerSharedTests::DataPointerSharedTests(
	const std::string& name,
	test::TestModule* parent,
	const std::vector<TestNode*>& required_nodes
) : TestModule(name, parent, required_nodes) {
	test::TestModule* list = addModule("DataPointer");
	list->OnAfterRunTest = [&]() {
		data_blocks.clear();
	};
	test::Test* null_test = list->addTest("null", [&](test::Test& test) { nullTest(test); });
	test::Test* basic_test = list->addTest("basic", [&](test::Test& test) { basicTest(test); });
}

void DataPointerSharedTests::nullTest(test::Test& test) {
	DataPointerShared<int> dp("Null", nullptr);
	T_COMPARE(dp.use_count(), 0);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerSharedTests::basicTest(test::Test& test) {
	int* x = new int(5);
	DataPointerShared<int> dp("x", x);
	T_COMPARE(dp.use_count(), 1);

	T_WRAP_CONTAINER(checkDataBlock(test, x, sizeof(int)));
}

void DataPointerSharedTests::checkDataBlock(test::Test& test, void* p_block, size_t p_size) {
	if (T_CHECK(data_blocks.size() > 0)) {
		auto it = data_blocks.find(p_block);
		if (T_CHECK(it != data_blocks.end())) {
			void* ptr = it->second.ptr;
			size_t size = it->second.size;
			T_COMPARE(ptr, p_block, &utils::pointer_to_str);
			T_COMPARE(size, p_size);
		}
	}
}

void DataPointerSharedTests::checkNoDataBlock(test::Test& test, void* p_block) {
	auto it = data_blocks.find(p_block);
	T_CHECK(it == data_blocks.end());
}
