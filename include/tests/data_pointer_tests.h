#pragma once

#include "test_lib/test.h"

class DataPointerTests : public test::TestModule {
public:
	DataPointerTests(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });

	static void checkDataBlock(test::Test& test, void* p_block, size_t p_size);
	static void checkNoDataBlock(test::Test& test, void* p_block);

};
