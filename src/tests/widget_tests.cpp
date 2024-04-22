#include "tests/widget_tests.h"
#include <utils.h>

WidgetTests::WidgetTests(test::TestManager& manager) : TestModule("Widgets", manager) { }

void WidgetTests::createTestLists() {
	createApplicationList();
    createWidgetsList();
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

void WidgetTests::createWidgetsList() {
    test::TestList* list = createTestList("Widgets");

    test::Test* root_widget_test = list->addTest(
        "root_widget",
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            application.setExternalMousePos(sf::Vector2i(400, 300));
            application.advance();
            fw::RectangleWidget* root_widget = dynamic_cast<fw::RectangleWidget*>(application.getWidgets().getRootWidget());
            tAssert(tCheck(root_widget, "Root widget is not a RectangleWidget"));
            tCompare(application.getWidgets().getSize(), 1);
            tCompare(root_widget->getName(), "root");
            tCompare(root_widget->getFullName(), "root");
            tCheck(!root_widget->isVisualPositionQuantized());
            tCheck(root_widget->isVisible());
            fw::WidgetVisibility wv = root_widget->checkVisibility();
            tCheck(wv.addedToRoot);
            tCheck(wv.allParentsVisible);
            tCheck(wv.hasUnclippedRegion);
            tCheck(wv.nonZeroSize);
            tCheck(wv.onScreen);
            tCheck(!wv.opaque);
            tCheck(wv.visibleSetting);
            tCheck(root_widget->isClickThrough());
            tCheck(root_widget->isMouseOver());
            tCheck(!root_widget->isFocusable());
            tCheck(!root_widget->isFocused());
            tCheck(root_widget->getClipChildren());
            tCheck(!root_widget->getForceCustomCursor());
            tCheck(!root_widget->getParent());
            tCompare(root_widget->getParentChain().size(), 0);
            tCompare(root_widget->getChildren().size(), 0);
            sf::FloatRect bounds = sf::FloatRect(sf::Vector2f(), fw::to2f(application.getWindowSize()));
            auto rect_to_str = &WidgetTests::floatRectToStr;
            tCompare(root_widget->getLocalBounds(), bounds, rect_to_str);
            tCompare(root_widget->getParentLocalBounds(), bounds, rect_to_str);
            tCompare(root_widget->getGlobalBounds(), bounds, rect_to_str);
            tCompare(root_widget->getVisualLocalBounds(), bounds, rect_to_str);
            tCompare(root_widget->getVisualParentLocalBounds(), bounds, rect_to_str);
            tCompare(root_widget->getVisualGlobalBounds(), bounds, rect_to_str);
            tCompare(root_widget->getUnclippedRegion(), bounds, rect_to_str);
            tCompare(root_widget->getQuantizedUnclippedRegion(), bounds, rect_to_str);
            tCompare(root_widget->getWidth(), bounds.width);
            tCompare(root_widget->getHeight(), bounds.height);
            tCompare(root_widget->getGlobalWidth(), bounds.width);
            tCompare(root_widget->getGlobalHeight(), bounds.height);
            auto vec2f_to_str = &WidgetTests::sfVec2fToStr;
            tCompare(root_widget->getSize(), fw::to2f(application.getWindowSize()), vec2f_to_str);
            sf::Vector2f top_left = bounds.getPosition();
            sf::Vector2f top_right = bounds.getPosition() + sf::Vector2f(bounds.width, 0.0f);
            sf::Vector2f bottom_left = bounds.getPosition() + sf::Vector2f(0.0f, bounds.height);
            sf::Vector2f bottom_right = bounds.getPosition() + bounds.getSize();
            tCompare(root_widget->getTopLeft(), top_left, vec2f_to_str);
            tCompare(root_widget->getTopRight(), top_right, vec2f_to_str);
            tCompare(root_widget->getBottomLeft(), bottom_left, vec2f_to_str);
            tCompare(root_widget->getBottomRight(), bottom_right, vec2f_to_str);
            tCompare(root_widget->getGlobalTopLeft(), top_left, vec2f_to_str);
            tCompare(root_widget->getGlobalTopRight(), top_right, vec2f_to_str);
            tCompare(root_widget->getGlobalBottomLeft(), bottom_left, vec2f_to_str);
            tCompare(root_widget->getGlobalBottomRight(), bottom_right, vec2f_to_str);
            tCompare(root_widget->getVisualGlobalTopLeft(), top_left, vec2f_to_str);
            tCompare(root_widget->getVisualGlobalTopRight(), top_right, vec2f_to_str);
            tCompare(root_widget->getVisualGlobalBottomLeft(), bottom_left, vec2f_to_str);
            tCompare(root_widget->getVisualGlobalBottomRight(), bottom_right, vec2f_to_str);
            tCompare(root_widget->getFillColor(), sf::Color::Transparent, &WidgetTests::colorToStr);
        }
    );
}

std::string WidgetTests::sfVec2fToStr(const sf::Vector2f& vec) {
    return "(" + utils::vec_to_str(vec) + ")";
}

std::string WidgetTests::sfVec2iToStr(const sf::Vector2i& vec) {
    return "(" + utils::vec_to_str(vec) + ")";
}

std::string WidgetTests::sfVec2uToStr(const sf::Vector2u& vec) {
    return "(" + utils::vec_to_str(vec) + ")";
}

std::string WidgetTests::cursorTypeToStr(sf::Cursor::Type type) {
    switch (type) {
        case sf::Cursor::Arrow:                  return "Arrow";
        case sf::Cursor::ArrowWait:              return "ArrowWait";
        case sf::Cursor::Wait:                   return "Wait";
        case sf::Cursor::Text:                   return "Text";
        case sf::Cursor::Hand:                   return "Hand";
        case sf::Cursor::SizeHorizontal:         return "SizeHorizontal";
        case sf::Cursor::SizeVertical:           return "SizeVertical";
        case sf::Cursor::SizeTopLeftBottomRight: return "SizeTopLeftBottomRight";
        case sf::Cursor::SizeBottomLeftTopRight: return "SizeBottomLeftTopRight";
        case sf::Cursor::SizeLeft:               return "SizeLeft";
        case sf::Cursor::SizeRight:              return "SizeRight";
        case sf::Cursor::SizeTop:                return "SizeTop";
        case sf::Cursor::SizeBottom:             return "SizeBottom";
        case sf::Cursor::SizeTopLeft:            return "SizeTopLeft";
        case sf::Cursor::SizeBottomRight:        return "SizeBottomRight";
        case sf::Cursor::SizeBottomLeft:         return "SizeBottomLeft";
        case sf::Cursor::SizeTopRight:           return "SizeTopRight";
        case sf::Cursor::SizeAll:                return "SizeAll";
        case sf::Cursor::Cross:                  return "Cross";
        case sf::Cursor::Help:                   return "Help";
        case sf::Cursor::NotAllowed:             return "NotAllowed";
        default:                                 mAssert(false, "Unknown cursor type"); return "";
    }
}

std::string WidgetTests::floatRectToStr(const sf::FloatRect& rect) {
    return "pos: " + sfVec2fToStr(rect.getPosition()) + " size: " + sfVec2fToStr(rect.getSize());
}

std::string WidgetTests::colorToStr(const sf::Color& color) {
    return "(" + utils::color_to_str(color) + ")";
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
