#include "tests/editor_tests.h"
#include "editor/UI/outliner.h"

EditorTests::EditorTests(
	test::TestManager& manager, const std::vector<TestModule*>& required_modules
) : TestModule("Editor", manager, required_modules) { }

void EditorTests::createTestLists() {
	test::TestList* list = createTestList("Editor");
	test::Test* basic_test = list->addTest("basic", [&](test::Test& test) { basicTest(test); });
	test::Test* init_test = list->addTest("init", { basic_test }, [&](test::Test& test) { initTest(test); });
	test::Test* advance_test = list->addTest("advance", { init_test }, [&](test::Test& test) { advanceTest(test); });
	test::Test* select_test = list->addTest("select", { advance_test }, [&](test::Test& test) { selectTest(test); });
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

void EditorTests::selectTest(test::Test& test) {
	Editor editor(window);
	editor.init(test.name);
	editor.start(true);
	editor.advance();
	editor.outliner_widget->setSize(150.0f, 100.0f);

	BoxObject* box0 = editor.getSimulation().createBox(
		"box0", b2Vec2(0.0f, 0.0f), 0.0f, b2Vec2(1.0f, 1.0f), sf::Color::Green
	);
	const CompVector<GameObject*>& selected_objects = editor.getSelectTool().getSelectedObjects();
	T_ASSERT(T_COMPARE(selected_objects.size(), 0));
	sf::Vector2f box_pos = editor.getObjectScreenPos(box0);
	editor.mouseMove(box_pos);
	editor.mouseLeftPress();
	editor.advance();
	T_COMPARE(selected_objects.size(), 0);
	editor.mouseLeftRelease();
	editor.advance();
	if (T_COMPARE(selected_objects.size(), 1)) {
		T_CHECK(selected_objects[0] == box0);
	}

	sf::Vector2f click_pos = editor.worldToScreen(box0->getGlobalPosition() + box0->size);
	editor.mouseMove(click_pos);
	editor.mouseLeftPress();
	editor.advance();
	T_COMPARE(selected_objects.size(), 1);
	editor.mouseLeftRelease();
	editor.advance();
	T_COMPARE(selected_objects.size(), 0);
}
