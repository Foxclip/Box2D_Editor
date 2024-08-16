#include "tests/example/test_module_2.h"

ExampleTestModule2::ExampleTestModule2(
	test::TestManager& manager, const std::vector<TestModule*>& required_modules
) : TestModule("ExampleModule2", manager, required_modules) { }

void ExampleTestModule2::createTestLists() {
	test::TestList* list_1 = createTestList("List1");
	test::Test* test_1_1 = list_1->addTest("first", [&](test::Test& test) { test1_1(test); });
}

void ExampleTestModule2::test1_1(test::Test& test) { }
