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
	test::Test* multi_select_test = list->addTest("multi_select", { select_test }, [&](test::Test& test) { multiSelectTest(test); });
	test::Test* move_test = list->addTest("move", { select_test }, [&](test::Test& test) { moveTest(test); });
	test::Test* pan_move_test = list->addTest("pan_move", { pan_test, move_test }, [&](test::Test& test) { panMoveTest(test); });
	test::Test* serialize_empty_test = list->addTest("serialize_empty", { advance_test }, [&](test::Test& test) { serializeEmptyTest(test); });
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
	editor.init(test.name, false);
}

void EditorTests::advanceTest(test::Test& test) {
	Editor editor(window);
	editor.init(test.name, false);
	editor.start(true);
	editor.advance();
}

void EditorTests::panTest(test::Test& test) {
	Editor editor(window);
	editor.init(test.name, false);
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
	editor.init(test.name, false);
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

void EditorTests::multiSelectTest(test::Test& test) {
	Editor editor(window);
	editor.init(test.name, false);
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

	clickObject(editor, box0);
	if (T_COMPARE(selected_objects.size(), 1)) {
		T_CHECK(selected_objects[0] == box0);
	}
	clickObject(editor, box1, true);
	if (T_COMPARE(selected_objects.size(), 2)) {
		T_CHECK(selected_objects[0] == box0);
		T_CHECK(selected_objects[1] == box1);
	}
	clickObject(editor, box2, true);
	if (T_COMPARE(selected_objects.size(), 3)) {
		T_CHECK(selected_objects[0] == box0);
		T_CHECK(selected_objects[1] == box1);
		T_CHECK(selected_objects[2] == box2);
	}
	clickObject(editor, box1, true);
	if (T_COMPARE(selected_objects.size(), 2)) {
		T_CHECK(selected_objects[0] == box0);
		T_CHECK(selected_objects[1] == box2);
	}
	clickObject(editor, box2, false);
	if (T_COMPARE(selected_objects.size(), 1)) {
		T_CHECK(selected_objects[0] == box2);
	}
	sf::Vector2f click_pos = editor.worldToScreen(box2->getGlobalPosition() + box2->size);
	clickMouse(editor, click_pos);
	T_COMPARE(selected_objects.size(), 0);
}

void EditorTests::moveTest(test::Test& test) {
	Editor editor(window);
	editor.init(test.name, false);
	editor.start(true);
	editor.outliner_widget->setSize(150.0f, 100.0f);
	editor.advance();

	sf::Vector2f box_offset = sf::Vector2f(100.0f, 50.0f);
	BoxObject* box0 = editor.getSimulation().createBox(
		"box0", b2Vec2(0.0f, 0.0f), 0.0f, b2Vec2(1.0f, 1.0f), sf::Color::Green
	);
	clickObject(editor, box0);
	sf::Vector2f box_pos_1 = editor.getObjectScreenPos(box0);
	sf::Vector2f box_pos_2 = box_pos_1 - box_offset;
	editor.mouseMove(box_pos_1);
	tapKey(editor, sf::Keyboard::G);
	editor.mouseMove(box_pos_2);
	editor.advance();
	editor.mouseLeftPress();
	editor.advance();
	{
		sf::Vector2f box_pos_2_actual = editor.getObjectScreenPos(box0);
		T_VEC2_APPROX_COMPARE(box_pos_2_actual, box_pos_2);
	}
	editor.mouseLeftRelease();
	editor.advance();
	{
		sf::Vector2f box_pos_2_actual = editor.getObjectScreenPos(box0);
		T_VEC2_APPROX_COMPARE(box_pos_2_actual, box_pos_2);
	}
}

void EditorTests::panMoveTest(test::Test& test) {
	Editor editor(window);
	editor.init(test.name, false);
	editor.start(true);
	editor.outliner_widget->setSize(150.0f, 100.0f);
	editor.advance();

	sf::Vector2f box_offset = sf::Vector2f(100.0f, 100.0f);
	BoxObject* box0 = editor.getSimulation().createBox(
		"box0", b2Vec2(0.0f, 0.0f), 0.0f, b2Vec2(1.0f, 1.0f), sf::Color::Green
	);
	clickObject(editor, box0);
	b2Vec2 box_world_pos = box0->getGlobalPosition();
	sf::Vector2f box_pos_1 = editor.getObjectScreenPos(box0);
	sf::Vector2f box_pos_2 = box_pos_1 - box_offset;
	editor.mouseMove(box_pos_1);
	tapKey(editor, sf::Keyboard::G);
	editor.mouseRightPress();
	editor.advance();
	editor.mouseMove(box_pos_2);
	editor.advance();
	{
		sf::Vector2f box_screen_pos_actual = editor.getObjectScreenPos(box0);
		T_VEC2_APPROX_COMPARE(box_screen_pos_actual, box_pos_2);
		b2Vec2 box_world_pos_actual = box0->getGlobalPosition();
		T_VEC2_APPROX_COMPARE(box_world_pos_actual, box_world_pos);
	}
	editor.mouseRightRelease();
	editor.advance();
	editor.mouseLeftPress();
	editor.advance();
	editor.mouseLeftRelease();
	editor.advance();
	{
		sf::Vector2f box_screen_pos_actual = editor.getObjectScreenPos(box0);
		T_VEC2_APPROX_COMPARE(box_screen_pos_actual, box_pos_2);
		b2Vec2 box_world_pos_actual = box0->getGlobalPosition();
		T_VEC2_APPROX_COMPARE(box_world_pos_actual, box_world_pos);
	}
}

void EditorTests::serializeEmptyTest(test::Test& test) {
	Editor editor(window);
	editor.init(test.name, false);
	editor.start(true);
	editor.advance();

	Camera& camera = editor.getCamera();
	camera.setPosition(1.0f, 2.0f);
	camera.setZoom(4.4f);
	std::string str = editor.serialize();
	editor.createBox("added box", b2Vec2(10.0f, 15.0f), 45.0f, b2Vec2(1.0f, 1.0f), sf::Color::Red);
	b2Vec2 prev_pos = camera.getPosition();
	float prev_zoom = camera.getZoom();
	camera.setPosition(2.0f, 3.0f);
	camera.setZoom(5.5f);
	editor.deserialize(str, true);
	auto objects = editor.getAllObjects();
	T_COMPARE(objects.size(), 0);
	T_VEC2_APPROX_COMPARE(camera.getPosition(), prev_pos);
	T_COMPARE(camera.getZoom(), prev_zoom);
}

void EditorTests::clickMouse(Editor& editor, const sf::Vector2f& pos) {
	editor.mouseMove(pos);
	editor.mouseLeftPress();
	editor.advance();
	editor.mouseLeftRelease();
	editor.advance();
}

void EditorTests::clickObject(Editor& editor, GameObject* object, bool shift, bool ctrl) {
	sf::Vector2f pos = editor.getObjectScreenPos(object);
	editor.mouseMove(pos);
	if (shift) {
		editor.keyPress(sf::Keyboard::LShift);
	}
	if (ctrl) {
		editor.keyPress(sf::Keyboard::LControl);
	}
	editor.mouseLeftPress();
	editor.advance();
	editor.mouseLeftRelease();
	editor.advance();
	if (ctrl) {
		editor.keyRelease(sf::Keyboard::LControl);
	}
	if (shift) {
		editor.keyRelease(sf::Keyboard::LShift);
	}
	editor.advance();
}

void EditorTests::tapKey(Editor& editor, sf::Keyboard::Key key) {
	editor.keyPress(key);
	editor.advance();
	editor.keyRelease(key);
	editor.advance();
}
