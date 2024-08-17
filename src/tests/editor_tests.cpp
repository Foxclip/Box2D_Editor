#include "tests/editor_tests.h"

EditorTests::EditorTests(
	test::TestManager& manager, const std::vector<TestModule*>& required_modules
) : TestModule("Editor", manager, required_modules) { }

void EditorTests::createTestLists() {
	test::TestList* list = createTestList("Editor");
	test::Test* basic_test = list->addTest("basic", [&](test::Test& test) { basicTest(test); });
	test::Test* init_test = list->addTest("init", { basic_test }, [&](test::Test& test) { initTest(test); });
	test::Test* advance_test = list->addTest("advance", { init_test }, [&](test::Test& test) { advanceTest(test); });
}

void EditorTests::basicTest(test::Test& test) {
	Editor editor;
}

void EditorTests::initTest(test::Test& test) {
	Editor editor;
	editor.init(test.name);
}

void EditorTests::advanceTest(test::Test& test) {
	Editor editor;
	editor.init(test.name);
	editor.start(true);
	editor.advance();
}
