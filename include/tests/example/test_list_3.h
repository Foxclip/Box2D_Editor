#include "tests/test.h"

class TestList3 : public test::TestList {
public:
	TestList3(const std::string& name, test::TestModule& module, const std::vector<TestList*>& required_lists = { });
	void test3_1(test::Test& test);
	void test3_2(test::Test& test);
	void test3_3(test::Test& test);
};
