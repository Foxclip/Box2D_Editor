#include "test_lib/test.h"

class ExampleTestModule1 : public test::TestModule {
public:
	ExampleTestModule1(test::TestManager& manager, const std::vector<TestModule*>& required_modules = { });
	void createTestLists() override;

private:
	void test1_1(test::Test& test);
	void test1_2(test::Test& test);
	void test1_3(test::Test& test);

	void test2_1(test::Test& test);
	void test2_2(test::Test& test);
	void test2_3(test::Test& test);

};
