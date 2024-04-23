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
                application.mouseMove(pos);
                application.mouseLeftPress();
                application.advance();
                tCompare(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
                tCompare(application.click_pos, pos, &WidgetTests::sfVec2iToStr);
            }
            {
                sf::Vector2i pos(150, 150);
                application.mouseMove(pos);
                application.mouseMove(pos);
                application.advance();
                tCompare(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
            }
            {
                sf::Vector2i pos(200, 200);
                application.mouseMove(pos);
                application.mouseLeftRelease();
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
                application.keyPress(sf::Keyboard::Space);
                application.advance();
                tCheck(application.space_key_pressed);
            }
            {
                application.keyRelease(sf::Keyboard::Space);
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
            application.mouseMove(400, 300);
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
    test::Test* rectangle_widget_test = list->addTest(
        "rectangle_widget",
        {
            root_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::RectangleWidget* rectangle_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            fw::Widget* root_widget = application.getWidgets().getRootWidget();
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f size(100.0f, 100.0f);
            rectangle_widget->setPosition(position);
            rectangle_widget->setSize(size);
            tAssert(tCheck(rectangle_widget));
            tCompare(application.getWidgets().getSize(), 2);
            tCompare(rectangle_widget->getName(), "rectangle");
            tCompare(rectangle_widget->getFullName(), "root|rectangle");
            tCheck(!rectangle_widget->isVisualPositionQuantized());
            tCheck(rectangle_widget->isVisible());
            fw::WidgetVisibility wv = rectangle_widget->checkVisibility();
            tCheck(wv.addedToRoot);
            tCheck(wv.allParentsVisible);
            tCheck(wv.hasUnclippedRegion);
            tCheck(wv.nonZeroSize);
            tCheck(wv.onScreen);
            tCheck(wv.opaque);
            tCheck(wv.visibleSetting);
            tCheck(rectangle_widget->isClickThrough());
            tCheck(!rectangle_widget->isMouseOver());
            tCheck(!rectangle_widget->isFocusable());
            tCheck(!rectangle_widget->isFocused());
            tCheck(!rectangle_widget->getClipChildren());
            tCheck(!rectangle_widget->getForceCustomCursor());
            tCheck(rectangle_widget->getParent() == root_widget);
            CompVector<fw::Widget*> parent_chain = rectangle_widget->getParentChain();
            tAssert(tCompare(parent_chain.size(), 1));
            tCheck(parent_chain[0] == root_widget);
            tCompare(rectangle_widget->getChildren().size(), 0);
            const CompVector<fw::Widget*>& root_children = root_widget->getChildren();
            tAssert(tCompare(root_children.size(), 1));
            tCheck(root_children[0] == rectangle_widget);
            tCheck(root_widget->getChild(0) == rectangle_widget);
            sf::FloatRect local_bounds = sf::FloatRect(sf::Vector2f(), size);
            sf::FloatRect parent_local_bounds = sf::FloatRect(position, size);
            auto rect_to_str = &WidgetTests::floatRectToStr;
            tCompare(rectangle_widget->getLocalBounds(), local_bounds, rect_to_str);
            tCompare(rectangle_widget->getParentLocalBounds(), parent_local_bounds, rect_to_str);
            tCompare(rectangle_widget->getGlobalBounds(), parent_local_bounds, rect_to_str);
            tCompare(rectangle_widget->getVisualLocalBounds(), local_bounds, rect_to_str);
            tCompare(rectangle_widget->getVisualParentLocalBounds(), parent_local_bounds, rect_to_str);
            tCompare(rectangle_widget->getVisualGlobalBounds(), parent_local_bounds, rect_to_str);
            tCompare(rectangle_widget->getUnclippedRegion(), parent_local_bounds, rect_to_str);
            tCompare(rectangle_widget->getQuantizedUnclippedRegion(), parent_local_bounds, rect_to_str);
            tCompare(rectangle_widget->getWidth(), parent_local_bounds.width);
            tCompare(rectangle_widget->getHeight(), parent_local_bounds.height);
            tCompare(rectangle_widget->getGlobalWidth(), parent_local_bounds.width);
            tCompare(rectangle_widget->getGlobalHeight(), parent_local_bounds.height);
            auto vec2f_to_str = &WidgetTests::sfVec2fToStr;
            tCompare(rectangle_widget->getSize(), size, vec2f_to_str);
            sf::Vector2f top_left = parent_local_bounds.getPosition();
            sf::Vector2f top_right = parent_local_bounds.getPosition() + sf::Vector2f(parent_local_bounds.width, 0.0f);
            sf::Vector2f bottom_left = parent_local_bounds.getPosition() + sf::Vector2f(0.0f, parent_local_bounds.height);
            sf::Vector2f bottom_right = parent_local_bounds.getPosition() + parent_local_bounds.getSize();
            tCompare(rectangle_widget->getTopLeft(), top_left, vec2f_to_str);
            tCompare(rectangle_widget->getTopRight(), top_right, vec2f_to_str);
            tCompare(rectangle_widget->getBottomLeft(), bottom_left, vec2f_to_str);
            tCompare(rectangle_widget->getBottomRight(), bottom_right, vec2f_to_str);
            tCompare(rectangle_widget->getGlobalTopLeft(), top_left, vec2f_to_str);
            tCompare(rectangle_widget->getGlobalTopRight(), top_right, vec2f_to_str);
            tCompare(rectangle_widget->getGlobalBottomLeft(), bottom_left, vec2f_to_str);
            tCompare(rectangle_widget->getGlobalBottomRight(), bottom_right, vec2f_to_str);
            tCompare(rectangle_widget->getVisualGlobalTopLeft(), top_left, vec2f_to_str);
            tCompare(rectangle_widget->getVisualGlobalTopRight(), top_right, vec2f_to_str);
            tCompare(rectangle_widget->getVisualGlobalBottomLeft(), bottom_left, vec2f_to_str);
            tCompare(rectangle_widget->getVisualGlobalBottomRight(), bottom_right, vec2f_to_str);
            tCompare(rectangle_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
        }
    );
    test::Test* set_parent_test = list->addTest(
        "set_parent",
        {
            root_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::Widget* root_widget = application.getWidgets().getRootWidget();
            fw::RectangleWidget* parent_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            fw::RectangleWidget* child_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            sf::Vector2f parent_local_pos_before = parent_widget->getPosition();
            sf::Vector2f parent_global_pos_before = parent_widget->getGlobalPosition();
            sf::Vector2f child_local_pos_before = child_widget->getPosition();
            sf::Vector2f child_global_pos_before = child_widget->getGlobalPosition();
            child_widget->setParent(parent_widget);
            tCheck(child_widget->getParent() == parent_widget);
            CompVector<fw::Widget*> parent_chain = child_widget->getParentChain();
            tAssert(tCompare(parent_chain.size(), 2));
            tCheck(parent_chain[0] == parent_widget);
            tCheck(parent_chain[1] == root_widget);
            tCheck(parent_widget->getParent() == root_widget);
            tAssert(tCompare(child_widget->getChildren().size(), 0));
            const CompVector<fw::Widget*>& parent_children = parent_widget->getChildren();
            tAssert(tCompare(parent_children.size(), 1));
            tCheck(parent_children[0] == child_widget);
            tCheck(parent_widget->getChild(0) == child_widget);
            tAssert(tCompare(parent_widget->getChildren().size(), 1));
            tCheck(parent_widget->getChild(0) == child_widget);
            CompVector<fw::Widget*> root_children = root_widget->getAllChildren();
            tAssert(tCompare(root_children.size(), 2));
            tCheck(root_children[0] == parent_widget);
            tCheck(root_children[1] == child_widget);
            sf::Vector2f parent_local_pos_after = parent_widget->getPosition();
            sf::Vector2f parent_global_pos_after = parent_widget->getGlobalPosition();
            sf::Vector2f child_local_pos_after = child_widget->getPosition();
            sf::Vector2f child_global_pos_after = child_widget->getGlobalPosition();
            tVec2ApproxCompare(parent_local_pos_after, parent_local_pos_before);
            tVec2ApproxCompare(parent_global_pos_after, parent_global_pos_before);
            tVec2ApproxCompare(child_local_pos_after, child_local_pos_before - parent_local_pos_before);
            tVec2ApproxCompare(child_global_pos_after, child_global_pos_before);
        }
    );
    test::Test* widget_mouse_events_1_test = list->addTest(
        "widget_mouse_events_1",
        {
            root_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::RectangleWidget* rectangle_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            bool mouse_entered = false;
            bool mouse_pressed = false;
            bool mouse_released = false;
            bool mouse_exited = false;
            rectangle_widget->OnMouseEnter = [&](const sf::Vector2f& pos) {
                mouse_entered = true;
            };
            rectangle_widget->OnClick = [&](const sf::Vector2f& pos) {
                mouse_pressed = true;
            };
            rectangle_widget->OnRelease = [&](const sf::Vector2f& pos) {
                mouse_released = true;
            };
            rectangle_widget->OnMouseExit = [&](const sf::Vector2f& pos) {
                mouse_exited = true;
            };
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f size(100.0f, 100.0f);
            sf::Vector2i mouse_pos_1(150, 150);
            sf::Vector2i mouse_pos_2(300, 300);
            rectangle_widget->setPosition(position);
            rectangle_widget->setSize(size);
            tCheck(!rectangle_widget->isMouseOver());
            tCheck(!mouse_entered);
            tCheck(!mouse_pressed);
            tCheck(!mouse_released);
            tCheck(!mouse_exited);
            application.mouseMove(mouse_pos_1);
            application.advance();
            tCheck(rectangle_widget->isMouseOver());
            tCheck(mouse_entered);
            tCheck(!mouse_pressed);
            tCheck(!mouse_released);
            tCheck(!mouse_exited);
            application.mouseLeftPress();
            application.advance();
            tCheck(mouse_entered);
            tCheck(mouse_pressed);
            tCheck(!mouse_released);
            tCheck(!mouse_exited);
            application.mouseLeftRelease();
            application.advance();
            tCheck(mouse_entered);
            tCheck(mouse_pressed);
            tCheck(!mouse_released); // clickThrough is on, so release is not processed
            tCheck(!mouse_exited);
            application.mouseMove(mouse_pos_2);
            application.advance();
            tCheck(!rectangle_widget->isMouseOver());
            tCheck(mouse_entered);
            tCheck(mouse_pressed);
            tCheck(!mouse_released);
            tCheck(mouse_exited);
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
    click_pos = getMousePos();
}

void TestApplication::onProcessLeftRelease() {
    process_left_release = true;
    release_pos = getMousePos();
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
