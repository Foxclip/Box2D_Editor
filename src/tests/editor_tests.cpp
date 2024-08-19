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
	test::Test* pan_test = list->addTest("pan", { advance_test }, [&](test::Test& test) { panTest(test); });
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

void EditorTests::panTest(test::Test& test) {
	Editor editor(window);
	editor.init(test.name);
	editor.start(true);
	editor.outliner_widget->setSize(150.0f, 100.0f);
	editor.advance();

	sf::Vector2f box_offset = sf::Vector2f(100.0f, 50.0f);
	BoxObject* box0 = editor.getSimulation().createBox(
		"box0", b2Vec2(0.0f, 0.0f), 0.0f, b2Vec2(1.0f, 1.0f), sf::Color::Green
	);
	sf::Vector2f box_pos_1 = editor.getObjectScreenPos(box0);
	sf::Vector2f box_pos_2 = box_pos_1 - box_offset;
	editor.mouseMove(box_pos_1);
	editor.mouseRightPress();
	editor.advance();
	editor.mouseMove(box_pos_2);
	editor.advance();
	{
		sf::Vector2f box_pos_2_actual = editor.getObjectScreenPos(box0);
		T_VEC2_APPROX_COMPARE(box_pos_2_actual, box_pos_2);
	}
	editor.mouseRightRelease();
	editor.advance();
	{
		sf::Vector2f box_pos_2_actual = editor.getObjectScreenPos(box0);
		T_VEC2_APPROX_COMPARE(box_pos_2_actual, box_pos_2);
	}
}


void EditorTests::selectTest(test::Test& test) {
	Editor editor(window);
	editor.init(test.name);
	editor.start(true);
	editor.outliner_widget->setSize(150.0f, 100.0f);
	editor.advance();

	BoxObject* box0 = editor.getSimulation().createBox(
		"box0", b2Vec2(-2.0f, 0.0f), 0.0f, b2Vec2(1.0f, 1.0f), sf::Color::Green
	);
	BoxObject* box1 = editor.getSimulation().createBox(
		"box1", b2Vec2(0.0f, 0.0f), 0.0f, b2Vec2(1.0f, 1.0f), sf::Color::Green
	);
	BoxObject* box2 = editor.getSimulation().createBox(
		"box2", b2Vec2(2.0f, 0.0f), 0.0f, b2Vec2(1.0f, 1.0f), sf::Color::Green
	);
	const CompVector<GameObject*>& selected_objects = editor.getSelectTool().getSelectedObjects();
	T_ASSERT(T_COMPARE(selected_objects.size(), 0));

	sf::Vector2f box_0_pos = editor.getObjectScreenPos(box0);
	editor.mouseMove(box_0_pos);
	editor.mouseLeftPress();
	editor.advance();
	T_COMPARE(selected_objects.size(), 0);
	editor.mouseLeftRelease();
	editor.advance();
	if (T_COMPARE(selected_objects.size(), 1)) {
		T_CHECK(selected_objects[0] == box0);
	}

	clickObject(editor, box1);
	if (T_COMPARE(selected_objects.size(), 1)) {
		T_CHECK(selected_objects[0] == box1);
	}

	clickObject(editor, box2);
	if (T_COMPARE(selected_objects.size(), 1)) {
		T_CHECK(selected_objects[0] == box2);
	}

	sf::Vector2f click_pos = editor.worldToScreen(box2->getGlobalPosition() + box2->size);
	editor.mouseMove(click_pos);
	editor.mouseLeftPress();
	editor.advance();
	T_COMPARE(selected_objects.size(), 1);
	editor.mouseLeftRelease();
	editor.advance();
	T_COMPARE(selected_objects.size(), 0);
}

void EditorTests::clickObject(Editor& editor, GameObject* object) {
	sf::Vector2f pos = editor.getObjectScreenPos(object);
	editor.mouseMove(pos);
	editor.mouseLeftPress();
	editor.advance();
	editor.mouseLeftRelease();
	editor.advance();
}
