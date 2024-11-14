#include "test_lib/example/test_list_3.h"

TestList3::TestList3(
	const std::string& name,
	test::TestModule& module,
	const std::vector<TestList*>& required_lists
) : TestList(name, module, required_lists) {

	test::Test* test_1_1 = addTest("first", [&](test::Test& test) { test3_1(test); });
	test::Test* test_1_2 = addTest("second", { test_1_1 }, [&](test::Test& test) { test3_2(test); });
	test::Test* test_1_3 = addTest("third", { test_1_2 }, [&](test::Test& test) { test3_3(test); });

}

void TestList3::test3_1(test::Test& test) { }

void TestList3::test3_2(test::Test& test) { }

void TestList3::test3_3(test::Test& test) { }
