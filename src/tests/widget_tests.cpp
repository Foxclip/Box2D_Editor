#include "tests/widget_tests.h"
#include <utils.h>

WidgetTests::WidgetTests(test::TestManager& manager) : TestModule("Widgets", manager) { }

void WidgetTests::createTestLists() {
	createApplicationList();
}

void WidgetTests::createApplicationList() {
	test::TestList* list = createTestList("Application");

    test::Test* basic_test = list->addTest(
        "basic",
        [&](test::Test& test) {
            fw::Application application;
        }
    );
    test::Test* init_test = list->addTest(
        "init",
        {
            basic_test
        },
        [&](test::Test& test) {
            TestApplication application;
            application.init("Test window", 800, 600, 0);
            tAssert(tCheck(application.initialized));
            tCompare(application.getWindowSize(), sf::Vector2u(800, 600), &WidgetTests::sfVec2uToStr);
        }
    );
    test::Test* start_test = list->addTest(
        "start",
        {
            init_test
        },
        [&](test::Test& test) {
            TestApplication application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            tAssert(tCheck(application.started));
        }
    );
    test::Test* advance_test = list->addTest(
        "advance",
        {
            start_test
        },
        [&](test::Test& test) {
            TestApplication application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            application.advance();
            tCheck(application.initialized);
            tCheck(application.started);
            tCheck(application.frame_begin);
            tCheck(application.frame_end);
            tCheck(application.process_widgets);
            tCheck(!application.process_window_event);
            tCheck(!application.process_keyboard_event);
            tCheck(!application.before_process_mouse_event);
            tCheck(!application.process_left_click);
            tCheck(!application.process_left_release);
            tCheck(!application.process_mouse_scroll);
            tCheck(application.process_keyboard);
            tCheck(application.process_mouse);
            tCheck(application.after_process_input);
            tCheck(application.process_world);
            tCheck(application.rendered);
        }
    );
    test::Test* close_test = list->addTest(
        "close",
        {
            start_test
        },
        [&](test::Test& test) {
            TestApplication application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            application.advance();
            application.close();
            tCheck(application.closed);
        }
    );
    test::Test* mouse_events_test = list->addTest(
        "mouse_events",
        {
            advance_test
        },
        [&](test::Test& test) {
            TestApplication application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            {
                sf::Vector2i pos(100, 100);
                application.setExternalMousePos(pos);
                sf::Event event;
                event.type = sf::Event::MouseButtonPressed;
                event.mouseButton.button = sf::Mouse::Left;
                event.mouseButton.x = pos.x;
                event.mouseButton.y = pos.y;
                application.addExternalEvent(event);
                application.advance();
                tCompare(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
                tCompare(application.click_pos, pos, &WidgetTests::sfVec2iToStr);
            }
            {
                sf::Vector2i pos(150, 150);
                application.setExternalMousePos(pos);
                sf::Event event;
                event.type = sf::Event::MouseMoved;
                event.mouseButton.x = pos.x;
                event.mouseButton.y = pos.y;
                application.addExternalEvent(event);
                application.advance();
                tCompare(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
            }
            {
                sf::Vector2i pos(200, 200);
                application.setExternalMousePos(pos);
                sf::Event event;
                event.type = sf::Event::MouseButtonReleased;
                event.mouseButton.button = sf::Mouse::Left;
                event.mouseButton.x = pos.x;
                event.mouseButton.y = pos.y;
                application.addExternalEvent(event);
                application.advance();
                tCompare(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
                tCompare(application.release_pos, pos, &WidgetTests::sfVec2iToStr);
            }
        }
    );
    test::Test* keyboard_events_test = list->addTest(
        "keyboard_events",
        {
            advance_test
        },
        [&](test::Test& test) {
            TestApplication application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            tCheck(!application.space_key_pressed);
            {
                sf::Event event;
                event.type = sf::Event::KeyPressed;
                event.key.code = sf::Keyboard::Space;
                application.addExternalEvent(event);
                application.advance();
                tCheck(application.space_key_pressed);
            }
            {
                sf::Event event;
                event.type = sf::Event::KeyReleased;
                event.key.code = sf::Keyboard::Space;
                application.addExternalEvent(event);
                application.advance();
                tCheck(!application.space_key_pressed);
            }
        }
    );
}

std::string WidgetTests::sfVec2uToStr(const sf::Vector2u& vec) {
    return "(" + utils::vec_to_str(vec) + ")";
}

std::string WidgetTests::sfVec2iToStr(const sf::Vector2i& vec) {
    return "(" + utils::vec_to_str(vec) + ")";
}

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

void TestApplication::beforeProcessMouseEvent(const sf::Event& event) {
    before_process_mouse_event = true;
}

void TestApplication::onProcessLeftClick() {
    process_left_click = true;
    click_pos = mousePos;
}

void TestApplication::onProcessLeftRelease() {
    process_left_release = true;
    release_pos = mousePos;
}

void TestApplication::onProcessMouseScroll(const sf::Event& event) {
    process_mouse_scroll = true;
}

void TestApplication::onProcessKeyboard() {
    process_keyboard = true;
}

void TestApplication::onProcessMouse() {
    process_mouse = true;
}

void TestApplication::afterProcessInput() {
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
