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

void EditorTests::beforeRunModule() {
	sf::ContextSettings cs_window;
	window.create(sf::VideoMode(800, 600), "Editor tests", sf::Style::Default, cs_window);
	font = fw::Font("fonts/verdana.ttf");
}

void EditorTests::afterRunModule() {
	window.close();
}

void EditorTests::basicTest(test::Test& test) {
	Editor editor(window);
}

void EditorTests::initTest(test::Test& test) {
	Editor editor(window);
	editor.init(test.name);
}

void EditorTests::advanceTest(test::Test& test) {
	Editor editor(window);
	editor.init(test.name);
	editor.start(true);
	editor.advance();
}
