#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_application.h"

WidgetTestsApplication::WidgetTestsApplication(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : WidgetTest(name, parent, required_nodes) {
    test::Test* basic_test = addTest("basic", [&](test::Test& test) { basicTest(test); });
    test::Test* init_test = addTest("init", { basic_test }, [&](test::Test& test) { initTest(test); });
    test::Test* start_test = addTest("start", { init_test }, [&](test::Test& test) { startTest(test); });
    test::Test* advance_test = addTest("advance", { start_test }, [&](test::Test& test) { advanceTest(test); });
    test::Test* close_test = addTest("close", { start_test }, [&](test::Test& test) { closeTest(test); });
    test::Test* mouse_events_test = addTest("mouse_events", { advance_test }, [&](test::Test& test) { mouseEventsTest(test); });
    test::Test* keyboard_events_test = addTest("keyboard_events", { advance_test }, [&](test::Test& test) { keyboardEventsTest(test); });
}

void WidgetTestsApplication::basicTest(test::Test& test) {
    fw::Application application(getWindow());
}

void WidgetTestsApplication::initTest(test::Test& test) {
    TestApplication application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    T_ASSERT(T_CHECK(application.initialized));
    T_COMPARE(application.getWindowSize(), sf::Vector2u(800, 600), &WidgetTests::sfVec2uToStr);
}

void WidgetTestsApplication::startTest(test::Test& test) {
    TestApplication application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    T_ASSERT(T_CHECK(application.started));
}

void WidgetTestsApplication::advanceTest(test::Test& test) {
    TestApplication application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    T_CHECK(application.initialized);
    T_CHECK(application.started);
    T_CHECK(application.frame_begin);
    T_CHECK(application.frame_end);
    T_CHECK(application.process_widgets);
    T_CHECK(!application.process_window_event);
    T_CHECK(!application.process_keyboard_event);
    T_CHECK(!application.process_left_press);
    T_CHECK(!application.process_left_release);
    T_CHECK(!application.process_mouse_move);
    T_CHECK(!application.process_mouse_scroll_x);
    T_CHECK(!application.process_mouse_scroll_y);
    T_CHECK(application.process_keyboard);
    T_CHECK(application.process_mouse);
    T_CHECK(application.after_process_input);
    T_CHECK(application.process_world);
    T_CHECK(application.rendered);
}

void WidgetTestsApplication::closeTest(test::Test& test) {
    TestApplication application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    application.close();
    T_CHECK(application.closed);
}

void WidgetTestsApplication::mouseEventsTest(test::Test& test) {
    TestApplication application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    {
        sf::Vector2i pos(100, 100);
        application.mouseMove(pos);
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
        T_CHECK(application.process_mouse_move);
    }
    {
        sf::Vector2i pos(100, 100);
        application.mouseLeftPress();
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
        T_COMPARE(application.left_click_pos, pos, &WidgetTests::sfVec2iToStr);
    }
    {
        sf::Vector2i pos(150, 150);
        application.mouseMove(pos);
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
    }
    {
        sf::Vector2i pos(200, 200);
        application.mouseMove(pos);
        application.mouseLeftRelease();
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
        T_COMPARE(application.left_release_pos, pos, &WidgetTests::sfVec2iToStr);
    }
    {
        sf::Vector2i pos(100, 100);
        application.mouseMove(pos);
        application.mouseRightPress();
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
        T_COMPARE(application.right_click_pos, pos, &WidgetTests::sfVec2iToStr);
    }
    {
        sf::Vector2i pos(150, 150);
        application.mouseMove(pos);
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
    }
    {
        sf::Vector2i pos(200, 200);
        application.mouseMove(pos);
        application.mouseRightRelease();
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
        T_COMPARE(application.right_release_pos, pos, &WidgetTests::sfVec2iToStr);
    }
    {
        float delta = 5.5f;
        application.mouseScrollX(delta);
        application.advance();
        T_COMPARE(application.scroll_x_delta, delta);
    }
    {
        float delta = 5.5f;
        application.mouseScrollY(delta);
        application.advance();
        T_COMPARE(application.scroll_y_delta, delta);
    }
}

void WidgetTestsApplication::keyboardEventsTest(test::Test& test) {
    TestApplication application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    T_CHECK(!application.space_key_pressed);
    {
        application.keyPress(sf::Keyboard::Space);
        application.advance();
        T_CHECK(application.space_key_pressed);
    }
    {
        application.keyRelease(sf::Keyboard::Space);
        application.advance();
        T_CHECK(!application.space_key_pressed);
    }
}

TestApplication::TestApplication(sf::RenderWindow& window) : Application(window) { }

void TestApplication::onInit() {
    initialized = true;
}

void TestApplication::onStart() {
    started = true;
}

void TestApplication::onFrameBegin() {
    frame_begin = true;
}

void TestApplication::onFrameEnd() {
    frame_end = true;
}

void TestApplication::onProcessWidgets() {
    process_widgets = true;
}

void TestApplication::onProcessWindowEvent(const sf::Event& event) {
    process_window_event = true;
}

void TestApplication::onProcessKeyboardEvent(const sf::Event& event) {
    process_keyboard_event = true;
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            space_key_pressed = true;
        }
    } else if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::Space) {
            space_key_pressed = false;
        }
    }
}

void TestApplication::onProcessLeftPress() {
    process_left_press = true;
    left_click_pos = getMousePos();
}

void TestApplication::onProcessRightPress() {
    process_right_press = true;
    right_click_pos = getMousePos();
}

void TestApplication::onProcessLeftRelease() {
    process_left_release = true;
    left_release_pos = getMousePos();
}

void TestApplication::onProcessRightRelease() {
    process_right_release = true;
    right_release_pos = getMousePos();
}

void TestApplication::onProcessMouseMove() {
    process_mouse_move = true;
}

void TestApplication::onProcessMouseScrollX(float delta) {
    process_mouse_scroll_x = true;
    scroll_x_delta = delta;
}

void TestApplication::onProcessMouseScrollY(float delta) {
    process_mouse_scroll_y = true;
    scroll_y_delta = delta;
}

void TestApplication::onProcessKeyboard() {
    process_keyboard = true;
}

void TestApplication::onProcessMouse() {
    process_mouse = true;
}

void TestApplication::onAfterProcessInput() {
    after_process_input = true;
}

void TestApplication::onProcessWorld() {
    process_world = true;
}

void TestApplication::onRender() {
    rendered = true;
}

void TestApplication::onClose() {
    closed = true;
}
