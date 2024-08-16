#include "tests/editor_tests.h"

EditorTests::EditorTests(
	test::TestManager& manager, const std::vector<TestModule*>& required_modules
) : TestModule("Editor", manager, required_modules) { }

void EditorTests::createTestLists() {
	test::TestList* list = createTestList("Editor");
	test::Test* basic_test = list->addTest("basic", [&](test::Test& test) { basicTest(test); });
}

void EditorTests::basicTest(test::Test& test) {
	Editor editor;
}
