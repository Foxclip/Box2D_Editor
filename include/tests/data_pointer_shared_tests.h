#pragma once

#include "test_lib/test.h"

class DataPointerSharedTests : public test::TestModule {
public:
	DataPointerSharedTests(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });

private:
	void nullTest(test::Test& test);
	void basicTest(test::Test& test);
	void structTest(test::Test& test);
	void destructorTest(test::Test& test);
	void derivedDestructorTest(test::Test& test);
	void vectorDestructorTest(test::Test& test);
	void customDeleterTest(test::Test& test);
	void lambdaDeleterTest(test::Test& test);
	void getTest(test::Test& test);
	void copyTest(test::Test& test);
	void resetTest(test::Test& test);
	void resetDeleterTest(test::Test& test);
	void moveConstructorTest(test::Test& test);
	void moveConstructorDeleterTest(test::Test& test);
	void moveConstructorDerivedTest(test::Test& test);
	void moveConstructorDerivedDeleterTest(test::Test& test);

	void checkDataBlock(test::Test& test, void* p_block, size_t p_size);
	void checkNoDataBlock(test::Test& test, void* p_block);

};
