#include "test_lib/test.h"

class ExampleTestModule2 : public test::TestModule {
public:
	ExampleTestModule2(test::TestManager& manager, const std::vector<TestModule*>& required_modules = { });
	void createTestLists() override;

private:
	void test1_1(test::Test& test);

};
