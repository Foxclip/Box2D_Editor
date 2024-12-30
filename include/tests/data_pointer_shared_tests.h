#pragma once

#include "test_lib/test.h"

class DataPointerSharedTests : public test::TestModule {
public:
	DataPointerSharedTests(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });

private:
	void nullTest(test::Test& test);
	void basicTest(test::Test& test);

	void checkDataBlock(test::Test& test, void* p_block, size_t p_size);
	void checkNoDataBlock(test::Test& test, void* p_block);

};
