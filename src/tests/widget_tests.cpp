#include "tests/widget_tests.h"
#include <utils.h>

#define CLICK_MOUSE(pos) \
    application.mouseMove(pos); \
    application.mouseLeftPress(); \
    application.advance(); \
    application.mouseLeftRelease(); \
    application.advance();

#define PRESS_KEY(key) \
    application.keyPress(key); \
    application.advance();

#define RELEASE_KEY(key) \
    application.keyRelease(key); \
    application.advance();

#define TAP_KEY(key) \
    PRESS_KEY(key) \
    RELEASE_KEY(key)

#define ENTER_TEXT(key, code) \
    application.keyPress(key); \
    application.textEntered(code); \
    application.advance(); \
    application.keyRelease(key); \
    application.advance();

#define SELECT_ALL() \
    application.keyPress(sf::Keyboard::LControl); \
    application.keyPress(sf::Keyboard::A); \
    application.advance(); \
    application.keyRelease(sf::Keyboard::A); \
    application.keyRelease(sf::Keyboard::LControl); \
    application.advance();

#define COPY() \
    PRESS_KEY(sf::Keyboard::LControl); \
    ENTER_TEXT(sf::Keyboard::C, 'c'); \
    RELEASE_KEY(sf::Keyboard::LControl);

#define PASTE() \
    PRESS_KEY(sf::Keyboard::LControl); \
    ENTER_TEXT(sf::Keyboard::V, 'v'); \
    RELEASE_KEY(sf::Keyboard::LControl);

#define CUT() \
    PRESS_KEY(sf::Keyboard::LControl); \
    ENTER_TEXT(sf::Keyboard::X, 'x'); \
    RELEASE_KEY(sf::Keyboard::LControl);

#define _CHECK_SELECTION(active, text, cursor_pos, left, right) \
    T_CHECK(textbox_widget->isSelectionActive() == active); \
    T_COMPARE(textbox_widget->getSelectedText(), text); \
    T_COMPARE(textbox_widget->getCursorPos(), cursor_pos); \
    T_COMPARE(textbox_widget->getSelectionLeft(), left); \
    T_COMPARE(textbox_widget->getSelectionRight(), right); \

#define CHECK_SELECTION(active, text, cursor_pos, left, right) \
    T_WRAP_CONTAINER(_CHECK_SELECTION(active, text, cursor_pos, left, right))

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
            application.init("Test window", 800, 600, 0, false);
            T_ASSERT(T_CHECK(application.initialized));
            T_COMPARE(application.getWindowSize(), sf::Vector2u(800, 600), &WidgetTests::sfVec2uToStr);
        }
    );
    test::Test* start_test = list->addTest(
        "start",
        {
            init_test
        },
        [&](test::Test& test) {
            TestApplication application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            T_ASSERT(T_CHECK(application.started));
        }
    );
    test::Test* advance_test = list->addTest(
        "advance",
        {
            start_test
        },
        [&](test::Test& test) {
            TestApplication application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.advance();
            T_CHECK(application.initialized);
            T_CHECK(application.started);
            T_CHECK(application.frame_begin);
            T_CHECK(application.frame_end);
            T_CHECK(application.process_widgets);
            T_CHECK(!application.process_window_event);
            T_CHECK(!application.process_keyboard_event);
            T_CHECK(!application.before_process_mouse_event);
            T_CHECK(!application.process_left_click);
            T_CHECK(!application.process_left_release);
            T_CHECK(!application.process_mouse_scroll);
            T_CHECK(application.process_keyboard);
            T_CHECK(application.process_mouse);
            T_CHECK(application.after_process_input);
            T_CHECK(application.process_world);
            T_CHECK(application.rendered);
        }
    );
    test::Test* close_test = list->addTest(
        "close",
        {
            start_test
        },
        [&](test::Test& test) {
            TestApplication application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.advance();
            application.close();
            T_CHECK(application.closed);
        }
    );
    test::Test* mouse_events_test = list->addTest(
        "mouse_events",
        {
            advance_test
        },
        [&](test::Test& test) {
            TestApplication application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            {
                sf::Vector2i pos(100, 100);
                application.mouseMove(pos);
                application.mouseLeftPress();
                application.advance();
                T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
                T_COMPARE(application.click_pos, pos, &WidgetTests::sfVec2iToStr);
            }
            {
                sf::Vector2i pos(150, 150);
                application.mouseMove(pos);
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
                T_COMPARE(application.release_pos, pos, &WidgetTests::sfVec2iToStr);
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
            application.init("Test window", 800, 600, 0, false);
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
    );
}

void WidgetTests::createWidgetsList() {
    test::TestList* list = createTestList("Widgets");
    list->OnBeforeRunAllTests = [&]() {
        textbox_font.loadFromFile("fonts/verdana.ttf");
    };

    test::Test* root_widget_test = list->addTest(
        "root_widget",
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::RectangleWidget* root_widget = dynamic_cast<fw::RectangleWidget*>(application.getWidgets().getRootWidget());
            T_ASSERT(T_CHECK(root_widget, "Root widget is not a RectangleWidget"));

            GenericWidgetTest gwt(application, test);
            gwt.widget = root_widget;
            gwt.total_widgets = 1;
            gwt.name = "root";
            gwt.fullname = "root";
            gwt.is_visual_position_quantized = false;
            gwt.is_visible = true;
            gwt.opaque = false;
            gwt.is_click_through = true;
            gwt.is_mouse_over = true;
            gwt.is_focusable = false;
            gwt.is_focused = false;
            gwt.clip_children = true;
            gwt.force_custom_cursor = false;
            gwt.parent = nullptr;
            gwt.local_bounds = sf::FloatRect(sf::Vector2f(), fw::to2f(application.getWindowSize()));
            gwt.global_bounds = gwt.local_bounds;
            gwt.parent_local_bounds = gwt.local_bounds;
            gwt.visual_local_bounds = gwt.local_bounds;
            gwt.visual_global_bounds = gwt.local_bounds;
            gwt.visual_parent_local_bounds = gwt.local_bounds;
            T_WRAP_CONTAINER(genericWidgetTest(gwt));

            T_COMPARE(root_widget->getFillColor(), sf::Color::Transparent, &WidgetTests::colorToStr);

            T_COMPARE(root_widget->getParentChain().size(), 0);
            T_COMPARE(root_widget->getChildren().size(), 0);
        }
    );
    test::Test* rectangle_widget_test = list->addTest(
        "rectangle_widget",
        {
            root_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::RectangleWidget* rectangle_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            fw::Widget* root_widget = application.getWidgets().getRootWidget();
            T_ASSERT(T_CHECK(rectangle_widget));
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f size(100.0f, 100.0f);
            rectangle_widget->setPosition(position);
            rectangle_widget->setSize(size);

            GenericWidgetTest gwt(application, test);
            gwt.widget = rectangle_widget;
            gwt.total_widgets = 2;
            gwt.name = "rectangle";
            gwt.fullname = "root|rectangle";
            gwt.is_visual_position_quantized = false;
            gwt.is_visible = true;
            gwt.opaque = true;
            gwt.is_click_through = true;
            gwt.is_mouse_over = false;
            gwt.is_focusable = false;
            gwt.is_focused = false;
            gwt.clip_children = false;
            gwt.force_custom_cursor = false;
            gwt.parent = root_widget;
            gwt.local_bounds = sf::FloatRect(sf::Vector2f(), size);
            gwt.global_bounds = sf::FloatRect(position, size);
            gwt.parent_local_bounds = gwt.global_bounds;
            gwt.visual_local_bounds = gwt.local_bounds;
            gwt.visual_global_bounds = gwt.global_bounds;
            gwt.visual_parent_local_bounds = gwt.global_bounds;
            T_WRAP_CONTAINER(genericWidgetTest(gwt));

            T_CHECK(rectangle_widget->getParent() == root_widget);
            CompVector<fw::Widget*> parent_chain = rectangle_widget->getParentChain();
            T_ASSERT(T_COMPARE(parent_chain.size(), 1));
            T_CHECK(parent_chain[0] == root_widget);
            T_COMPARE(rectangle_widget->getChildren().size(), 0);
            const CompVector<fw::Widget*>& root_children = root_widget->getChildren();
            T_ASSERT(T_COMPARE(root_children.size(), 1));
            T_CHECK(root_children[0] == rectangle_widget);
            T_CHECK(root_widget->getChild(0) == rectangle_widget);

            T_COMPARE(rectangle_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
        }
    );
    test::Test* set_parent_test = list->addTest(
        "set_parent",
        {
            root_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
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
            T_CHECK(child_widget->getParent() == parent_widget);
            CompVector<fw::Widget*> parent_chain = child_widget->getParentChain();
            T_ASSERT(T_COMPARE(parent_chain.size(), 2));
            T_CHECK(parent_chain[0] == parent_widget);
            T_CHECK(parent_chain[1] == root_widget);
            T_CHECK(parent_widget->getParent() == root_widget);
            T_ASSERT(T_COMPARE(child_widget->getChildren().size(), 0));
            const CompVector<fw::Widget*>& parent_children = parent_widget->getChildren();
            T_ASSERT(T_COMPARE(parent_children.size(), 1));
            T_CHECK(parent_children[0] == child_widget);
            T_CHECK(parent_widget->getChild(0) == child_widget);
            T_ASSERT(T_COMPARE(parent_widget->getChildren().size(), 1));
            T_CHECK(parent_widget->getChild(0) == child_widget);
            CompVector<fw::Widget*> root_children = root_widget->getAllChildren();
            T_ASSERT(T_COMPARE(root_children.size(), 2));
            T_CHECK(root_children[0] == parent_widget);
            T_CHECK(root_children[1] == child_widget);
            sf::Vector2f parent_local_pos_after = parent_widget->getPosition();
            sf::Vector2f parent_global_pos_after = parent_widget->getGlobalPosition();
            sf::Vector2f child_local_pos_after = child_widget->getPosition();
            sf::Vector2f child_global_pos_after = child_widget->getGlobalPosition();
            T_VEC2_APPROX_COMPARE(parent_local_pos_after, parent_local_pos_before);
            T_VEC2_APPROX_COMPARE(parent_global_pos_after, parent_global_pos_before);
            T_VEC2_APPROX_COMPARE(child_local_pos_after, child_local_pos_before - parent_local_pos_before);
            T_VEC2_APPROX_COMPARE(child_global_pos_after, child_global_pos_before);
        }
    );
    test::Test* widget_mouse_events_1_test = list->addTest(
        "widget_mouse_events_1",
        {
            root_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
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
            T_CHECK(!rectangle_widget->isMouseOver());
            T_CHECK(!mouse_entered);
            T_CHECK(!mouse_pressed);
            T_CHECK(!mouse_released);
            T_CHECK(!mouse_exited);
            application.mouseMove(mouse_pos_1);
            application.advance();
            T_CHECK(rectangle_widget->isMouseOver());
            T_CHECK(mouse_entered);
            T_CHECK(!mouse_pressed);
            T_CHECK(!mouse_released);
            T_CHECK(!mouse_exited);
            application.mouseLeftPress();
            application.advance();
            T_CHECK(mouse_entered);
            T_CHECK(mouse_pressed);
            T_CHECK(!mouse_released);
            T_CHECK(!mouse_exited);
            application.mouseLeftRelease();
            application.advance();
            T_CHECK(mouse_entered);
            T_CHECK(mouse_pressed);
            T_CHECK(!mouse_released); // clickThrough is on, so release is not processed
            T_CHECK(!mouse_exited);
            application.mouseMove(mouse_pos_2);
            application.advance();
            T_CHECK(!rectangle_widget->isMouseOver());
            T_CHECK(mouse_entered);
            T_CHECK(mouse_pressed);
            T_CHECK(!mouse_released);
            T_CHECK(mouse_exited);
        }
    );
    test::Test* widget_mouse_events_2_test = list->addTest(
        "widget_mouse_events_2",
        {
            root_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::RectangleWidget* rectangle_widget_1 = application.getWidgets().createWidget<fw::RectangleWidget>();
            fw::RectangleWidget* rectangle_widget_2 = application.getWidgets().createWidget<fw::RectangleWidget>();
            rectangle_widget_1->setClickThrough(false);
            bool mouse_entered_1 = false;
            bool mouse_pressed_1 = false;
            bool mouse_released_1 = false;
            bool mouse_exited_1 = false;
            bool mouse_entered_2 = false;
            bool mouse_pressed_2 = false;
            bool mouse_released_2 = false;
            bool mouse_exited_2 = false;
            rectangle_widget_1->OnMouseEnter = [&](const sf::Vector2f& pos) {
                mouse_entered_1 = true;
            };
            rectangle_widget_1->OnClick = [&](const sf::Vector2f& pos) {
                mouse_pressed_1 = true;
            };
            rectangle_widget_1->OnRelease = [&](const sf::Vector2f& pos) {
                mouse_released_1 = true;
            };
            rectangle_widget_1->OnMouseExit = [&](const sf::Vector2f& pos) {
                mouse_exited_1 = true;
            };
            rectangle_widget_2->OnMouseEnter = [&](const sf::Vector2f& pos) {
                mouse_entered_2 = true;
            };
            rectangle_widget_2->OnClick = [&](const sf::Vector2f& pos) {
                mouse_pressed_2 = true;
            };
            rectangle_widget_2->OnRelease = [&](const sf::Vector2f& pos) {
                mouse_released_2 = true;
            };
            rectangle_widget_2->OnMouseExit = [&](const sf::Vector2f& pos) {
                mouse_exited_2 = true;
            };
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f size(100.0f, 100.0f);
            sf::Vector2i mouse_pos_1(150, 150);
            sf::Vector2i mouse_pos_2(300, 300);
            rectangle_widget_1->setPosition(position);
            rectangle_widget_1->setSize(size);
            rectangle_widget_2->setPosition(position);
            rectangle_widget_2->setSize(size);
            T_CHECK(!rectangle_widget_1->isMouseOver());
            T_CHECK(!mouse_entered_1);
            T_CHECK(!mouse_pressed_1);
            T_CHECK(!mouse_released_1);
            T_CHECK(!mouse_exited_1);
            T_CHECK(!rectangle_widget_2->isMouseOver());
            T_CHECK(!mouse_entered_2);
            T_CHECK(!mouse_pressed_2);
            T_CHECK(!mouse_released_2);
            T_CHECK(!mouse_exited_2);
            application.mouseMove(mouse_pos_1);
            application.advance();
            T_CHECK(rectangle_widget_1->isMouseOver());
            T_CHECK(mouse_entered_1);
            T_CHECK(!mouse_pressed_1);
            T_CHECK(!mouse_released_1);
            T_CHECK(!mouse_exited_1);
            T_CHECK(rectangle_widget_2->isMouseOver());
            T_CHECK(mouse_entered_2);
            T_CHECK(!mouse_pressed_2);
            T_CHECK(!mouse_released_2);
            T_CHECK(!mouse_exited_2);
            application.mouseLeftPress();
            application.advance();
            T_CHECK(mouse_entered_1);
            T_CHECK(mouse_pressed_1);
            T_CHECK(!mouse_released_1);
            T_CHECK(!mouse_exited_1);
            T_CHECK(mouse_entered_2);
            T_CHECK(mouse_pressed_2);
            T_CHECK(!mouse_released_2);
            T_CHECK(!mouse_exited_2);
            application.mouseLeftRelease();
            application.advance();
            T_CHECK(mouse_entered_1);
            T_CHECK(mouse_pressed_1);
            T_CHECK(!mouse_released_1); // no focused widget, so release is not processed
            T_CHECK(!mouse_exited_1);
            T_CHECK(mouse_entered_2);
            T_CHECK(mouse_pressed_2);
            T_CHECK(!mouse_released_2);
            T_CHECK(!mouse_exited_2);
            application.mouseMove(mouse_pos_2);
            application.advance();
            T_CHECK(!rectangle_widget_1->isMouseOver());
            T_CHECK(mouse_entered_1);
            T_CHECK(mouse_pressed_1);
            T_CHECK(!mouse_released_1);
            T_CHECK(mouse_exited_1);
            T_CHECK(!rectangle_widget_2->isMouseOver());
            T_CHECK(mouse_entered_2);
            T_CHECK(mouse_pressed_2);
            T_CHECK(!mouse_released_2);
            T_CHECK(mouse_exited_2);
        }
    );
    test::Test* events_test = list->addTest(
        "events",
        {
            root_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            bool updated = false;
            bool before_render = false;
            unsigned int window_width = 0;
            unsigned int window_height = 0;
            fw::RectangleWidget* widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            widget->setSize(100.0f, 100.0f);
            widget->OnUpdate = [&]() {
                updated = true;
            };
            widget->OnBeforeRender = [&]() {
                before_render = true;
            };
            widget->OnWindowResized = [&](unsigned int width, unsigned int height) {
                window_width = width;
                window_height = height;
            };
            application.advance();
            T_CHECK(updated);
            T_CHECK(before_render);
            T_COMPARE(window_width, 0);
            T_COMPARE(window_height, 0);
            application.setWindowSize(640, 480);
            application.advance();
            T_COMPARE(window_width, 640);
            T_COMPARE(window_height, 480);
        }
    );
    test::Test* coordinates_test = list->addTest(
        "coordinates",
        {
            set_parent_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::RectangleWidget* parent_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            fw::RectangleWidget* child_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            sf::Vector2f parent_pos(100.0f, 100.0f);
            sf::Vector2f child_local_pos(30.0f, 30.0f);
            child_widget->setParent(parent_widget);
            parent_widget->setPosition(parent_pos);
            child_widget->setPosition(child_local_pos);
            T_VEC2_APPROX_COMPARE(child_widget->toGlobal(sf::Vector2f(10.0f, 5.0f)), sf::Vector2f(140.0f, 135.0f));
            T_VEC2_APPROX_COMPARE(child_widget->toLocal(sf::Vector2f(140.0f, 135.0f)), sf::Vector2f(10.0f, 5.0f));
        }
    );
    test::Test* find_test = list->addTest(
        "find",
        {
            set_parent_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::RectangleWidget* parent_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            fw::RectangleWidget* child_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            parent_widget->setName("parent");
            child_widget->setName("child");
            child_widget->setParent(parent_widget);
            T_CHECK(application.getWidgets().find("root") == application.getWidgets().getRootWidget());
            T_CHECK(application.getWidgets().find("parent") == parent_widget);
            T_CHECK(application.getWidgets().find("child") == child_widget);
        }
    );
    test::Test* anchor_test = list->addTest(
        "anchor",
        {
            set_parent_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::RectangleWidget* parent_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            fw::RectangleWidget* child_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            child_widget->setParent(parent_widget);
            sf::Vector2f parent_size(100.0f, 100.0f);
            sf::Vector2f child_size(30.0f, 30.0f);
            sf::Vector2f anchor_offset(5.0f, 7.0f);
            parent_widget->setSize(parent_size);
            child_widget->setSize(child_size);
            child_widget->setParentAnchor(fw::Widget::Anchor::TOP_LEFT);
            child_widget->setAnchorOffset(anchor_offset);
            T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f() + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::TOP_CENTER);
            T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x / 2.0f, 0.0f) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::TOP_RIGHT);
            T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x, 0.0f) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::CENTER_LEFT);
            T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(0.0f, parent_size.y / 2.0f) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::CENTER);
            T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x / 2.0f, parent_size.y / 2.0f) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::CENTER_RIGHT);
            T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x, parent_size.y / 2.0f) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::BOTTOM_LEFT);
            T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(0.0f, parent_size.y) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::BOTTOM_CENTER);
            T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x / 2.0f, parent_size.y) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::BOTTOM_RIGHT);
            T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x, parent_size.y) + anchor_offset);
        }
    );
    test::Test* text_widget_test = list->addTest(
        "text_widget",
        {
            root_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::TextWidget* text_widget = application.getWidgets().createWidget<fw::TextWidget>();
            fw::Widget* root_widget = application.getWidgets().getRootWidget();
            T_ASSERT(T_CHECK(text_widget));
            text_widget->setCharacterSize(20);
            text_widget->setFont(textbox_font);
            text_widget->setString("Text");
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f size(41.0f, 20.0f);
            sf::Vector2f local_bounds_offset(0.0f, 6.0f);
            text_widget->setPosition(position);

            GenericWidgetTest gwt(application, test);
            gwt.widget = text_widget;
            gwt.total_widgets = 2;
            gwt.name = "text";
            gwt.fullname = "root|text";
            gwt.is_visual_position_quantized = true;
            gwt.is_visible = true;
            gwt.opaque = true;
            gwt.is_click_through = true;
            gwt.is_mouse_over = false;
            gwt.is_focusable = false;
            gwt.is_focused = false;
            gwt.clip_children = false;
            gwt.force_custom_cursor = false;
            gwt.parent = root_widget;
            gwt.local_bounds = sf::FloatRect(sf::Vector2f(), size);
            gwt.global_bounds = sf::FloatRect(position, size);
            gwt.parent_local_bounds = gwt.global_bounds;
            gwt.visual_local_bounds = sf::FloatRect(local_bounds_offset, size - local_bounds_offset);
            gwt.visual_global_bounds = sf::FloatRect(position + local_bounds_offset, size - local_bounds_offset);
            gwt.visual_parent_local_bounds = gwt.visual_global_bounds;
            T_WRAP_CONTAINER(genericWidgetTest(gwt));

            T_COMPARE(text_widget->getChildren().size(), 0);

            T_COMPARE(text_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
        }
    );
    test::Test* checkbox_widget_basic_test = list->addTest(
        "checkbox_widget_basic",
        {
            rectangle_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::CheckboxWidget* checkbox_widget = application.getWidgets().createWidget<fw::CheckboxWidget>();
            T_ASSERT(T_CHECK(checkbox_widget));
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f size(20.0f, 20.0f);
            checkbox_widget->setPosition(position);
            checkbox_widget->setSize(size);
            T_COMPARE(application.getWidgets().getSize(), 3);
            T_COMPARE(checkbox_widget->getName(), "checkbox");
            T_COMPARE(checkbox_widget->getFullName(), "root|checkbox");
            T_CHECK(!checkbox_widget->isVisualPositionQuantized());
            T_CHECK(checkbox_widget->isVisible());
            fw::WidgetVisibility wv = checkbox_widget->checkVisibility();
            T_CHECK(wv.addedToRoot);
            T_CHECK(wv.allParentsVisible);
            T_CHECK(wv.hasUnclippedRegion);
            T_CHECK(wv.nonZeroSize);
            T_CHECK(wv.onScreen);
            T_CHECK(wv.opaque);
            T_CHECK(wv.visibleSetting);
            T_CHECK(!checkbox_widget->isClickThrough());
            T_CHECK(!checkbox_widget->isMouseOver());
            T_CHECK(checkbox_widget->isFocusable());
            T_CHECK(!checkbox_widget->isFocused());
            T_CHECK(!checkbox_widget->getClipChildren());
            T_CHECK(!checkbox_widget->getForceCustomCursor());
            if (T_COMPARE(checkbox_widget->getChildren().size(), 1)) {
                fw::RectangleWidget* check_widget = dynamic_cast<fw::RectangleWidget*>(checkbox_widget->getChild(0));
                T_CHECK(check_widget, "Check widget is not a RectangleWidget");
                T_COMPARE(check_widget->getParentAnchor(), fw::Widget::Anchor::CENTER, &WidgetTests::anchorToStr);
            }
            sf::FloatRect local_bounds = sf::FloatRect(sf::Vector2f(), size);
            sf::FloatRect parent_local_bounds = sf::FloatRect(position, size);
            auto rect_to_str = &WidgetTests::floatRectToStr;
            T_COMPARE(checkbox_widget->getLocalBounds(), local_bounds, rect_to_str);
            T_COMPARE(checkbox_widget->getParentLocalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(checkbox_widget->getGlobalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(checkbox_widget->getVisualLocalBounds(), local_bounds, rect_to_str);
            T_COMPARE(checkbox_widget->getVisualParentLocalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(checkbox_widget->getVisualGlobalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(checkbox_widget->getUnclippedRegion(), parent_local_bounds, rect_to_str);
            T_COMPARE(checkbox_widget->getQuantizedUnclippedRegion(), parent_local_bounds, rect_to_str);
            T_COMPARE(checkbox_widget->getWidth(), parent_local_bounds.width);
            T_COMPARE(checkbox_widget->getHeight(), parent_local_bounds.height);
            auto vec2f_to_str = &WidgetTests::sfVec2fToStr;
            T_COMPARE(checkbox_widget->getSize(), size, vec2f_to_str);
            T_COMPARE(checkbox_widget->getFillColor(), sf::Color(50, 50, 50), &WidgetTests::colorToStr);
        }
    );
    test::Test* checkbox_widget_toggle_test = list->addTest(
        "checkbox_widget_toggle",
        {
            checkbox_widget_basic_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            fw::CheckboxWidget* checkbox_widget = application.getWidgets().createWidget<fw::CheckboxWidget>();
            T_CHECK(!checkbox_widget->getValue());
            application.mouseMove(fw::to2i(checkbox_widget->getGlobalCenter()));
            application.advance();
            application.mouseLeftPress();
            application.advance();
            T_CHECK(checkbox_widget->getValue());
            application.mouseLeftRelease();
            application.advance();
            T_CHECK(checkbox_widget->getValue());
        }
    );
    test::Test* container_widget_basic_test = list->addTest(
        "container_widget_basic",
        {
            rectangle_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::ContainerWidget* container_widget = application.getWidgets().createWidget<fw::ContainerWidget>();
            T_ASSERT(T_CHECK(container_widget));
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f size(100.0f, 100.0f);
            container_widget->setPosition(position);
            container_widget->setSize(size);
            T_COMPARE(application.getWidgets().getSize(), 2);
            T_COMPARE(container_widget->getName(), "container");
            T_COMPARE(container_widget->getFullName(), "root|container");
            T_CHECK(!container_widget->isVisualPositionQuantized());
            T_CHECK(container_widget->isVisible());
            fw::WidgetVisibility wv = container_widget->checkVisibility();
            T_CHECK(wv.addedToRoot);
            T_CHECK(wv.allParentsVisible);
            T_CHECK(wv.hasUnclippedRegion);
            T_CHECK(wv.nonZeroSize);
            T_CHECK(wv.onScreen);
            T_CHECK(wv.opaque);
            T_CHECK(wv.visibleSetting);
            T_CHECK(container_widget->isClickThrough());
            T_CHECK(!container_widget->isMouseOver());
            T_CHECK(!container_widget->isFocusable());
            T_CHECK(!container_widget->isFocused());
            T_CHECK(!container_widget->getClipChildren());
            T_CHECK(!container_widget->getForceCustomCursor());
            sf::FloatRect local_bounds = sf::FloatRect(sf::Vector2f(), size);
            sf::FloatRect parent_local_bounds = sf::FloatRect(position, size);
            auto rect_to_str = &WidgetTests::floatRectToStr;
            T_COMPARE(container_widget->getLocalBounds(), local_bounds, rect_to_str);
            T_COMPARE(container_widget->getParentLocalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(container_widget->getGlobalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(container_widget->getVisualLocalBounds(), local_bounds, rect_to_str);
            T_COMPARE(container_widget->getVisualParentLocalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(container_widget->getVisualGlobalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(container_widget->getUnclippedRegion(), parent_local_bounds, rect_to_str);
            T_COMPARE(container_widget->getQuantizedUnclippedRegion(), parent_local_bounds, rect_to_str);
            T_COMPARE(container_widget->getWidth(), parent_local_bounds.width);
            T_COMPARE(container_widget->getHeight(), parent_local_bounds.height);
            auto vec2f_to_str = &WidgetTests::sfVec2fToStr;
            T_COMPARE(container_widget->getSize(), size, vec2f_to_str);
            T_COMPARE(container_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
        }
    );
    test::Test* container_widget_children_test = list->addTest(
        "container_widget_children",
        {
            container_widget_basic_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            fw::ContainerWidget* container_widget = application.getWidgets().createWidget<fw::ContainerWidget>();
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f container_size(100.0f, 100.0f);
            float horizontal_padding = 5.0f;
            float vertical_padding = 10.0f;
            container_widget->setPosition(position);
            container_widget->setSize(container_size);
            container_widget->setHorizontalPadding(horizontal_padding);
            container_widget->setVerticalPadding(vertical_padding);
            T_VEC2_APPROX_COMPARE(container_widget->getSize(), container_size);
            application.advance();
            T_VEC2_APPROX_COMPARE(container_widget->getSize(), sf::Vector2f(horizontal_padding, vertical_padding));
            fw::RectangleWidget* child_1_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            fw::RectangleWidget* child_2_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            sf::Vector2f child_1_size(35.0f, 28.0f);
            sf::Vector2f child_2_size(46.0f, 54.0f);
            child_1_widget->setSize(child_1_size);
            child_2_widget->setSize(child_2_size);
            child_1_widget->setParent(container_widget);
            child_2_widget->setParent(container_widget);
            application.advance();
            sf::Vector2f new_container_size = sf::Vector2f(
                horizontal_padding + child_1_widget->getWidth() + horizontal_padding + child_2_widget->getWidth() + horizontal_padding,
                vertical_padding + std::max(child_1_widget->getHeight(), child_2_widget->getHeight()) + vertical_padding
            );
            T_VEC2_APPROX_COMPARE(container_widget->getSize(), new_container_size);
            T_VEC2_APPROX_COMPARE(child_1_widget->getPosition(), sf::Vector2f(horizontal_padding, vertical_padding));
            T_VEC2_APPROX_COMPARE(child_2_widget->getPosition(), sf::Vector2f(horizontal_padding + child_1_widget->getWidth() + horizontal_padding, vertical_padding));
        }
    );
    test::Test* textbox_widget_basic_test = list->addTest(
        "textbox_widget_basic",
        {
            rectangle_widget_test,
            text_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0, false);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::TextBoxWidget* textbox_widget = application.getWidgets().createWidget<fw::TextBoxWidget>();
            T_ASSERT(T_CHECK(textbox_widget));
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f size(40.0f, 20.0f);
            textbox_widget->setPosition(position);
            textbox_widget->setSize(size);
            T_COMPARE(application.getWidgets().getSize(), 5);
            T_COMPARE(textbox_widget->getName(), "textbox");
            T_COMPARE(textbox_widget->getFullName(), "root|textbox");
            T_CHECK(!textbox_widget->isVisualPositionQuantized());
            T_CHECK(textbox_widget->isVisible());
            fw::WidgetVisibility wv = textbox_widget->checkVisibility();
            T_CHECK(wv.addedToRoot);
            T_CHECK(wv.allParentsVisible);
            T_CHECK(wv.hasUnclippedRegion);
            T_CHECK(wv.nonZeroSize);
            T_CHECK(wv.onScreen);
            T_CHECK(wv.opaque);
            T_CHECK(wv.visibleSetting);
            T_CHECK(!textbox_widget->isClickThrough());
            T_CHECK(!textbox_widget->isMouseOver());
            T_CHECK(textbox_widget->isFocusable());
            T_CHECK(!textbox_widget->isFocused());
            T_CHECK(textbox_widget->getClipChildren());
            T_CHECK(textbox_widget->getForceCustomCursor());
            sf::FloatRect local_bounds = sf::FloatRect(sf::Vector2f(), size);
            sf::FloatRect parent_local_bounds = sf::FloatRect(position, size);
            auto rect_to_str = &WidgetTests::floatRectToStr;
            T_COMPARE(textbox_widget->getLocalBounds(), local_bounds, rect_to_str);
            T_COMPARE(textbox_widget->getParentLocalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(textbox_widget->getGlobalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(textbox_widget->getVisualLocalBounds(), local_bounds, rect_to_str);
            T_COMPARE(textbox_widget->getVisualParentLocalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(textbox_widget->getVisualGlobalBounds(), parent_local_bounds, rect_to_str);
            T_COMPARE(textbox_widget->getUnclippedRegion(), parent_local_bounds, rect_to_str);
            T_COMPARE(textbox_widget->getQuantizedUnclippedRegion(), parent_local_bounds, rect_to_str);
            T_COMPARE(textbox_widget->getWidth(), parent_local_bounds.width);
            T_COMPARE(textbox_widget->getHeight(), parent_local_bounds.height);
            auto vec2f_to_str = &WidgetTests::sfVec2fToStr;
            T_COMPARE(textbox_widget->getSize(), size, vec2f_to_str);
            T_COMPARE(textbox_widget->getFillColor(), sf::Color(50, 50, 50), &WidgetTests::colorToStr);
        }
    );
    test::Test* textbox_widget_input_test = list->addTest(
        "textbox_widget_input",
        {
            textbox_widget_basic_test
        },
        [&](test::Test& test) {
            fw::Application application;
            fw::TextBoxWidget* textbox_widget = initTextBox(application, 80.0f, 20.0f);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));

            T_CHECK(textbox_widget->isFocused());
            T_CHECK(application.getWidgets().getFocusedWidget() == textbox_widget);
            T_CHECK(textbox_widget->isEditMode());
            T_COMPARE(textbox_widget->getValue(), "Text");
            T_CHECK(textbox_widget->isValidValue());
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            ENTER_TEXT(sf::Keyboard::BackSpace, '\b');
            T_COMPARE(textbox_widget->getValue(), "");
            CHECK_SELECTION(false, "", 0, -1, -1);
            ENTER_TEXT(sf::Keyboard::A, 'a');
            T_COMPARE(textbox_widget->getValue(), "a");
            CHECK_SELECTION(false, "", 1, -1, -1);
            ENTER_TEXT(sf::Keyboard::B, 'b');
            T_COMPARE(textbox_widget->getValue(), "ab");
            CHECK_SELECTION(false, "", 2, -1, -1);
            ENTER_TEXT(sf::Keyboard::C, 'c');
            T_COMPARE(textbox_widget->getValue(), "abc");
            CHECK_SELECTION(false, "", 3, -1, -1);
            ENTER_TEXT(sf::Keyboard::Enter, '\n');
            T_CHECK(!textbox_widget->isEditMode());
            T_COMPARE(textbox_widget->getValue(), "abc");
            T_CHECK(textbox_widget->isFocused());
            T_CHECK(application.getWidgets().getFocusedWidget() == textbox_widget);
        }
    );
    test::Test* textbox_widget_events_test = list->addTest(
        "textbox_widget_events",
        {
            textbox_widget_input_test
        },
        [&](test::Test& test) {
            fw::Application application;
            fw::TextBoxWidget* textbox_widget = initTextBox(application, 80.0f, 20.0f);
            bool edit_mode = false;
            sf::String value;
            bool confirmed = false;
            bool cancelled = false;
            textbox_widget->OnEditModeToggle = [&](bool value) {
                edit_mode = value;
            };
            textbox_widget->OnValueChanged = [&](const sf::String& new_value) {
                value = new_value;
            };
            textbox_widget->OnConfirm = [&](const sf::String& value) {
                confirmed = true;
            };
            textbox_widget->OnCancel = [&]() {
                cancelled = true;
            };
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));
            T_CHECK(edit_mode);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalTopRight() + sf::Vector2f(10.0f, 0.0f)));
            T_CHECK(!edit_mode);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));
            T_CHECK(edit_mode);
            ENTER_TEXT(sf::Keyboard::A, 'a');
            T_COMPARE(value, "a");
            ENTER_TEXT(sf::Keyboard::A, 'b');
            T_COMPARE(value, "ab");
            ENTER_TEXT(sf::Keyboard::A, 'c');
            T_COMPARE(value, "abc");
            ENTER_TEXT(sf::Keyboard::Enter, '\n');
            T_CHECK(!edit_mode);
            T_CHECK(confirmed);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));
            T_CHECK(edit_mode);
            TAP_KEY(sf::Keyboard::Escape);
            T_CHECK(!edit_mode);
            T_CHECK(cancelled);
        }
    );
    test::Test* textbox_widget_cursor_test = list->addTest(
        "textbox_widget_cursor",
        {
            textbox_widget_basic_test
        },
        [&](test::Test& test) {
            fw::Application application;
            fw::TextBoxWidget* textbox_widget = initTextBox(application, 80.0f, 20.0f);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));

            ENTER_TEXT(sf::Keyboard::BackSpace, '\b');
            TAP_KEY(sf::Keyboard::Left);
            T_COMPARE(textbox_widget->getValue(), "");
            CHECK_SELECTION(false, "", 0, -1, -1);
            TAP_KEY(sf::Keyboard::Right);
            T_COMPARE(textbox_widget->getValue(), "");
            CHECK_SELECTION(false, "", 0, -1, -1);
            ENTER_TEXT(sf::Keyboard::A, 'a');
            ENTER_TEXT(sf::Keyboard::B, 'b');
            ENTER_TEXT(sf::Keyboard::C, 'c');
            ENTER_TEXT(sf::Keyboard::D, 'd');
            T_ASSERT(T_COMPARE(textbox_widget->getValue(), "abcd"));
            CHECK_SELECTION(false, "", 4, -1, -1);
            auto move_cursor = [&](sf::Keyboard::Key key, size_t pos) {
                application.keyPress(key);
                application.advance();
                T_COMPARE(textbox_widget->getValue(), "abcd");
                CHECK_SELECTION(false, "", pos, -1, -1);
            };
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 4));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 4));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 3));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 2));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 1));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 0));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 0));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 0));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 1));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 2));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 3));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 4));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 4));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 4));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Home, 0));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Home, 0));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::End, 4));
            T_WRAP_CONTAINER(move_cursor(sf::Keyboard::End, 4));
        }
    );
    test::Test* textbox_widget_scroll_test = list->addTest(
        "textbox_widget_scroll",
        {
            textbox_widget_cursor_test
        },
        [&](test::Test& test) {
            fw::Application application;
            fw::TextBoxWidget* textbox_widget = initTextBox(application, 20.0f, 20.0f);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));

            const fw::TextWidget* text_widget = textbox_widget->getTextWidget();
            float zero_pos = textbox_widget->TEXT_VIEW_ZERO_POS.x;
            float right_margin = textbox_widget->getWidth() - textbox_widget->CURSOR_MOVE_MARGIN;
            auto calc_text_pos = [&](size_t cursor_pos_index) {
                float cursor_pos = zero_pos + text_widget->getLocalCharPos(cursor_pos_index).x;
                float past_right_margin = std::max(0.0f, cursor_pos - right_margin);
                float text_pos = zero_pos - past_right_margin;
                return text_pos;
            };
            T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(4));
            TAP_KEY(sf::Keyboard::Home);
            T_APPROX_COMPARE(text_widget->getPosition().x, zero_pos);

            T_ASSERT_NO_ERRORS();
            TAP_KEY(sf::Keyboard::Right);
            T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(1));
            TAP_KEY(sf::Keyboard::Right);
            T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(2));
            TAP_KEY(sf::Keyboard::Right);
            T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(3));
            TAP_KEY(sf::Keyboard::Right);
            T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(4));
            TAP_KEY(sf::Keyboard::Right);
            T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(4));

            T_ASSERT_NO_ERRORS();
            float begin_pos = text_widget->getPosition().x;
            TAP_KEY(sf::Keyboard::Left);
            T_COMPARE(text_widget->getPosition().x, begin_pos);
            TAP_KEY(sf::Keyboard::Left);
            T_COMPARE(textbox_widget->getLocalCharPos(2).x, zero_pos);
            TAP_KEY(sf::Keyboard::Left);
            T_COMPARE(textbox_widget->getLocalCharPos(1).x, zero_pos);
            TAP_KEY(sf::Keyboard::Left);
            T_COMPARE(textbox_widget->getLocalCharPos(0).x, zero_pos);
            TAP_KEY(sf::Keyboard::Left);
            T_COMPARE(textbox_widget->getLocalCharPos(0).x, zero_pos);
        }
    );
    test::Test* textbox_widget_selection_test = list->addTest(
        "textbox_widget_selection",
        {
            textbox_widget_cursor_test
        },
        [&](test::Test& test) {
            fw::Application application;
            fw::TextBoxWidget* textbox_widget = initTextBox(application, 80.0f, 20.0f);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));

            CHECK_SELECTION(true, "Text", 4, 0, 4);
            TAP_KEY(sf::Keyboard::Left);
            CHECK_SELECTION(false, "", 0, -1, -1);

            T_ASSERT_NO_ERRORS();
            SELECT_ALL();
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            TAP_KEY(sf::Keyboard::Right);
            CHECK_SELECTION(false, "", 4, -1, -1);
            SELECT_ALL();
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            TAP_KEY(sf::Keyboard::Home);
            CHECK_SELECTION(false, "", 0, -1, -1);
            SELECT_ALL();
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            TAP_KEY(sf::Keyboard::End);
            CHECK_SELECTION(false, "", 4, -1, -1);

            T_ASSERT_NO_ERRORS();
            TAP_KEY(sf::Keyboard::Home);
            PRESS_KEY(sf::Keyboard::LShift);
            TAP_KEY(sf::Keyboard::Right);
            CHECK_SELECTION(true, "T", 1, 0, 1);
            TAP_KEY(sf::Keyboard::Right);
            CHECK_SELECTION(true, "Te", 2, 0, 2);
            TAP_KEY(sf::Keyboard::Right);
            CHECK_SELECTION(true, "Tex", 3, 0, 3);
            TAP_KEY(sf::Keyboard::Right);
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            TAP_KEY(sf::Keyboard::Right);
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            RELEASE_KEY(sf::Keyboard::LShift);
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            TAP_KEY(sf::Keyboard::Left);
            CHECK_SELECTION(false, "", 0, -1, -1);

            T_ASSERT_NO_ERRORS();
            TAP_KEY(sf::Keyboard::Right);
            TAP_KEY(sf::Keyboard::Right);
            TAP_KEY(sf::Keyboard::Right);
            PRESS_KEY(sf::Keyboard::LShift);
            TAP_KEY(sf::Keyboard::Left);
            CHECK_SELECTION(true, "x", 2, 2, 3);
            TAP_KEY(sf::Keyboard::Left);
            CHECK_SELECTION(true, "ex", 1, 1, 3);
            TAP_KEY(sf::Keyboard::Left);
            CHECK_SELECTION(true, "Tex", 0, 0, 3);
            RELEASE_KEY(sf::Keyboard::LShift);
            TAP_KEY(sf::Keyboard::Right);
            CHECK_SELECTION(false, "", 3, -1, -1);

            T_ASSERT_NO_ERRORS();
            TAP_KEY(sf::Keyboard::Home);
            TAP_KEY(sf::Keyboard::Right);
            PRESS_KEY(sf::Keyboard::LShift);
            TAP_KEY(sf::Keyboard::Right);
            TAP_KEY(sf::Keyboard::Right);
            RELEASE_KEY(sf::Keyboard::LShift);
            CHECK_SELECTION(true, "ex", 3, 1, 3);
            SELECT_ALL();
            CHECK_SELECTION(true, "Text", 4, 0, 4);
        }
    );
    test::Test* textbox_widget_mouse_click_test = list->addTest(
        "textbox_widget_mouse_click",
        {
            textbox_widget_cursor_test
        },
        [&](test::Test& test) {
            fw::Application application;
            fw::TextBoxWidget* textbox_widget = initTextBox(application, 80.0f, 20.0f);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));

            auto click_at_char = [&](size_t index) {
                T_ASSERT_NO_ERRORS();
                SELECT_ALL();
                CHECK_SELECTION(true, "Text", 4, 0, 4);
                CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCharPos(index)));
                T_CHECK(textbox_widget->isFocused());
                CHECK_SELECTION(false, "", index, -1, -1);
            };
            T_WRAP_CONTAINER(click_at_char(0));
            T_WRAP_CONTAINER(click_at_char(1));
            T_WRAP_CONTAINER(click_at_char(2));
            T_WRAP_CONTAINER(click_at_char(3));
            T_WRAP_CONTAINER(click_at_char(4));

            T_ASSERT_NO_ERRORS();
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalTopRight() + sf::Vector2f(10.0f, 0.0f)));
            CHECK_SELECTION(false, "", 0, -1, -1);
            T_CHECK(!textbox_widget->isFocused());
        }
    );
    test::Test* textbox_widget_mouse_drag_test = list->addTest(
        "textbox_widget_mouse_drag",
        {
            textbox_widget_mouse_click_test
        },
        [&](test::Test& test) {
            fw::Application application;
            fw::TextBoxWidget* textbox_widget = initTextBox(application, 80.0f, 20.0f);

            auto get_char_pos = [&](size_t index) {
                return fw::to2i(textbox_widget->getGlobalCharPos(index));
            };
            auto move_to_char = [&](size_t index) {
                application.mouseMove(get_char_pos(index));
                application.advance();
            };
            auto drag_from_char = [&](size_t index) {
                application.mouseMove(get_char_pos(index));
                application.mouseLeftPress();
                application.advance();
            };
            auto mouse_release = [&]() {
                application.mouseLeftRelease();
                application.advance();
            };
            T_ASSERT(T_CHECK(!textbox_widget->isEditMode()));
            drag_from_char(1);
            T_ASSERT(T_CHECK(textbox_widget->isEditMode()));
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            move_to_char(2);
            CHECK_SELECTION(true, "e", 2, 1, 2);
            move_to_char(3);
            CHECK_SELECTION(true, "ex", 3, 1, 3);
            mouse_release();
            CHECK_SELECTION(true, "ex", 3, 1, 3);

            T_ASSERT_NO_ERRORS();
            drag_from_char(0);
            CHECK_SELECTION(false, "", 0, -1, -1);
            move_to_char(4);
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            mouse_release();
            CHECK_SELECTION(true, "Text", 4, 0, 4);

            T_ASSERT_NO_ERRORS();
            drag_from_char(4);
            CHECK_SELECTION(false, "", 4, -1, -1);
            move_to_char(0);
            CHECK_SELECTION(true, "Text", 0, 0, 4);
            mouse_release();
            CHECK_SELECTION(true, "Text", 0, 0, 4);
        }
    );
    test::Test* textbox_widget_copypaste_test = list->addTest(
        "textbox_widget_copypaste",
        {
            textbox_widget_selection_test
        },
        [&](test::Test& test) {
            fw::Application application;
            fw::TextBoxWidget* textbox_widget = initTextBox(application, 80.0f, 20.0f);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));

            COPY();
            T_COMPARE(textbox_widget->getValue(), "Text");
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            T_COMPARE(textbox_widget->getValue(), "");
            PASTE();
            T_COMPARE(textbox_widget->getValue(), "Text");

            T_ASSERT_NO_ERRORS();
            SELECT_ALL();
            CUT();
            T_COMPARE(textbox_widget->getValue(), "");
            PASTE();
            T_COMPARE(textbox_widget->getValue(), "Text");

            T_ASSERT_NO_ERRORS();
            TAP_KEY(sf::Keyboard::Home);
            TAP_KEY(sf::Keyboard::Right);
            PRESS_KEY(sf::Keyboard::LShift);
            TAP_KEY(sf::Keyboard::Right);
            TAP_KEY(sf::Keyboard::Right);
            RELEASE_KEY(sf::Keyboard::LShift);
            COPY();
            T_COMPARE(textbox_widget->getValue(), "Text");
            TAP_KEY(sf::Keyboard::Home);
            PASTE();
            T_COMPARE(textbox_widget->getValue(), "exText");

            T_ASSERT_NO_ERRORS();
            PRESS_KEY(sf::Keyboard::LShift);
            TAP_KEY(sf::Keyboard::Right);
            TAP_KEY(sf::Keyboard::Right);
            RELEASE_KEY(sf::Keyboard::LShift);
            CUT();
            T_COMPARE(textbox_widget->getValue(), "exxt");
            TAP_KEY(sf::Keyboard::Left);
            PRESS_KEY(sf::Keyboard::LShift);
            TAP_KEY(sf::Keyboard::Right);
            TAP_KEY(sf::Keyboard::Right);
            RELEASE_KEY(sf::Keyboard::LShift);
            PASTE();
            T_COMPARE(textbox_widget->getValue(), "eTet");

            T_ASSERT_NO_ERRORS();
            COPY();
            PASTE();
            T_COMPARE(textbox_widget->getValue(), "eTeTet");
            CUT();
            PASTE();
            T_COMPARE(textbox_widget->getValue(), "eTeTeTet");

            T_ASSERT_NO_ERRORS();
            SELECT_ALL();
            CUT();
            COPY();
            PASTE();
            T_COMPARE(textbox_widget->getValue(), "eTeTeTet");
            CUT();
            PASTE();
            T_COMPARE(textbox_widget->getValue(), "eTeTeTeteTeTeTet");
        }
    );
    test::Test* textbox_widget_history_test = list->addTest(
        "textbox_widget_history",
        {
            textbox_widget_copypaste_test
        },
        [&](test::Test& test) {
            fw::Application application;
            fw::TextBoxWidget* textbox_widget = initTextBox(application, 80.0f, 20.0f);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));

            auto undo = [&]() {
                PRESS_KEY(sf::Keyboard::LControl);
                ENTER_TEXT(sf::Keyboard::Z, 'z');
                RELEASE_KEY(sf::Keyboard::LControl);
            };
            auto redo = [&]() {
                PRESS_KEY(sf::Keyboard::LControl);
                PRESS_KEY(sf::Keyboard::LShift);
                ENTER_TEXT(sf::Keyboard::Z, 'z');
                RELEASE_KEY(sf::Keyboard::LShift);
                RELEASE_KEY(sf::Keyboard::LControl);
            };

            T_COMPARE(textbox_widget->getValue(), "Text");
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            T_COMPARE(textbox_widget->getValue(), "");
            undo();
            T_COMPARE(textbox_widget->getValue(), "Text");
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            redo();
            T_COMPARE(textbox_widget->getValue(), "");

            T_ASSERT_NO_ERRORS();
            ENTER_TEXT(sf::Keyboard::A, 'a');
            ENTER_TEXT(sf::Keyboard::B, 'b');
            ENTER_TEXT(sf::Keyboard::C, 'c');
            T_COMPARE(textbox_widget->getValue(), "abc");
            undo();
            T_COMPARE(textbox_widget->getValue(), "");
            undo();
            T_COMPARE(textbox_widget->getValue(), "Text");
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            undo();
            T_COMPARE(textbox_widget->getValue(), "Text");
            CHECK_SELECTION(true, "Text", 4, 0, 4);
            redo();
            T_COMPARE(textbox_widget->getValue(), "");
            redo();
            T_COMPARE(textbox_widget->getValue(), "abc");
            CHECK_SELECTION(false, "", 3, -1, -1);

            T_ASSERT_NO_ERRORS();
            undo();
            T_COMPARE(textbox_widget->getValue(), "");
            undo();
            T_COMPARE(textbox_widget->getValue(), "Text");
            TAP_KEY(sf::Keyboard::Left);
            TAP_KEY(sf::Keyboard::Right);
            TAP_KEY(sf::Keyboard::Right);
            TAP_KEY(sf::Keyboard::Right);
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            T_COMPARE(textbox_widget->getValue(), "Tt");
            undo();
            T_COMPARE(textbox_widget->getValue(), "Text");
            undo();
            T_COMPARE(textbox_widget->getValue(), "Text");
            CHECK_SELECTION(false, "", 3, -1, -1);

            T_ASSERT_NO_ERRORS();
            PRESS_KEY(sf::Keyboard::LShift);
            TAP_KEY(sf::Keyboard::Left);
            TAP_KEY(sf::Keyboard::Left);
            RELEASE_KEY(sf::Keyboard::LShift);
            CUT();
            T_COMPARE(textbox_widget->getValue(), "Tt");
            undo();
            T_COMPARE(textbox_widget->getValue(), "Text");
            undo();
            T_COMPARE(textbox_widget->getValue(), "Text");
            CHECK_SELECTION(true, "ex", 1, 1, 3);

            T_ASSERT_NO_ERRORS();
            T_COMPARE(textbox_widget->getValue(), "Text");
            TAP_KEY(sf::Keyboard::Right);
            PASTE();
            T_COMPARE(textbox_widget->getValue(), "Texext");
            PASTE();
            T_COMPARE(textbox_widget->getValue(), "Texexext");
            undo();
            T_COMPARE(textbox_widget->getValue(), "Texext");
            undo();
            T_COMPARE(textbox_widget->getValue(), "Text");
            undo();
            T_COMPARE(textbox_widget->getValue(), "Text");
            CHECK_SELECTION(false, "", 3, -1, -1);
        }
    );
    test::Test* textbox_widget_integer_test = list->addTest(
        "textbox_widget_integer",
        {
            textbox_widget_copypaste_test
        },
        [&](test::Test& test) {
            fw::Application application;
            fw::TextBoxWidget* textbox_widget = initTextBox(application, 80.0f, 20.0f);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));

            T_CHECK(textbox_widget->isValidValue());
            textbox_widget->setType(fw::TextBoxWidget::TextBoxType::INTEGER);
            T_CHECK(!textbox_widget->isValidValue());
            SELECT_ALL();
            CUT();
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Num1, '1');
            ENTER_TEXT(sf::Keyboard::Num2, '2');
            ENTER_TEXT(sf::Keyboard::Num3, '3');
            ENTER_TEXT(sf::Keyboard::Num4, '4');
            T_COMPARE(textbox_widget->getValue(), "1234");
            T_CHECK(textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::E, 'e');
            T_COMPARE(textbox_widget->getValue(), "1234");
            T_CHECK(textbox_widget->isValidValue());
            TAP_KEY(sf::Keyboard::Home);
            PRESS_KEY(sf::Keyboard::LShift);
            ENTER_TEXT(sf::Keyboard::Equal, '+');
            RELEASE_KEY(sf::Keyboard::LShift);
            T_COMPARE(textbox_widget->getValue(), "+1234");
            T_CHECK(textbox_widget->isValidValue());
            PRESS_KEY(sf::Keyboard::LShift);
            ENTER_TEXT(sf::Keyboard::Equal, '+');
            RELEASE_KEY(sf::Keyboard::LShift);
            T_COMPARE(textbox_widget->getValue(), "++1234");
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            TAP_KEY(sf::Keyboard::End);
            ENTER_TEXT(sf::Keyboard::Period, '.');
            T_COMPARE(textbox_widget->getValue(), "+1234");
            T_CHECK(textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Comma, ',');
            T_COMPARE(textbox_widget->getValue(), "+1234");
            T_CHECK(textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Dash, '-');
            T_COMPARE(textbox_widget->getValue(), "+1234-");
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            ENTER_TEXT(sf::Keyboard::Num5, '5');
            ENTER_TEXT(sf::Keyboard::Num6, '6');
            ENTER_TEXT(sf::Keyboard::Num7, '7');
            ENTER_TEXT(sf::Keyboard::Num8, '8');
            ENTER_TEXT(sf::Keyboard::Num9, '9');
            T_COMPARE(textbox_widget->getValue(), "+123456789");
            T_CHECK(textbox_widget->isValidValue());
            TAP_KEY(sf::Keyboard::Home);
            ENTER_TEXT(sf::Keyboard::Dash, '-');
            T_COMPARE(textbox_widget->getValue(), "-+123456789");
            T_CHECK(!textbox_widget->isValidValue());
            TAP_KEY(sf::Keyboard::Right);
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            T_COMPARE(textbox_widget->getValue(), "-123456789");
            T_CHECK(textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Num0, '0');
            T_COMPARE(textbox_widget->getValue(), "-0123456789");
            T_CHECK(textbox_widget->isValidValue());
        }
    );
    test::Test* textbox_widget_float_test = list->addTest(
        "textbox_widget_float",
        {
            textbox_widget_copypaste_test
        },
        [&](test::Test& test) {
            fw::Application application;
            fw::TextBoxWidget* textbox_widget = initTextBox(application, 80.0f, 20.0f);
            CLICK_MOUSE(fw::to2i(textbox_widget->getGlobalCenter()));

            T_CHECK(textbox_widget->isValidValue());
            textbox_widget->setType(fw::TextBoxWidget::TextBoxType::FLOAT);
            T_CHECK(!textbox_widget->isValidValue());
            SELECT_ALL();
            CUT();
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Num1, '1');
            ENTER_TEXT(sf::Keyboard::Num2, '2');
            ENTER_TEXT(sf::Keyboard::Num3, '3');
            ENTER_TEXT(sf::Keyboard::Num4, '4');
            T_COMPARE(textbox_widget->getValue(), "1234");
            T_CHECK(textbox_widget->isValidValue());
            TAP_KEY(sf::Keyboard::Home);
            PRESS_KEY(sf::Keyboard::LShift);
            ENTER_TEXT(sf::Keyboard::Equal, '+');
            RELEASE_KEY(sf::Keyboard::LShift);
            T_COMPARE(textbox_widget->getValue(), "+1234");
            T_CHECK(textbox_widget->isValidValue());
            PRESS_KEY(sf::Keyboard::LShift);
            ENTER_TEXT(sf::Keyboard::Equal, '+');
            RELEASE_KEY(sf::Keyboard::LShift);
            T_COMPARE(textbox_widget->getValue(), "++1234");
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            TAP_KEY(sf::Keyboard::End);
            ENTER_TEXT(sf::Keyboard::Dash, '-');
            T_COMPARE(textbox_widget->getValue(), "+1234-");
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            ENTER_TEXT(sf::Keyboard::A, 'a');
            T_COMPARE(textbox_widget->getValue(), "+1234");
            T_CHECK(textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::E, 'e');
            T_COMPARE(textbox_widget->getValue(), "+1234e");
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Num5, '9');
            T_COMPARE(textbox_widget->getValue(), "+1234e9");
            T_CHECK(textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Comma, ',');
            T_COMPARE(textbox_widget->getValue(), "+1234e9");
            T_CHECK(textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::E, 'e');
            T_COMPARE(textbox_widget->getValue(), "+1234e9e");
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            ENTER_TEXT(sf::Keyboard::Period, '.');
            T_COMPARE(textbox_widget->getValue(), "+1234e9.");
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Num6, '6');
            T_COMPARE(textbox_widget->getValue(), "+1234e9.6");
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            T_COMPARE(textbox_widget->getValue(), "+1234e9");
            TAP_KEY(sf::Keyboard::Left);
            TAP_KEY(sf::Keyboard::Left);
            TAP_KEY(sf::Keyboard::Left);
            ENTER_TEXT(sf::Keyboard::Period, '.');
            T_COMPARE(textbox_widget->getValue(), "+123.4e9");
            T_CHECK(textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Period, '.');
            T_COMPARE(textbox_widget->getValue(), "+123..4e9");
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            T_COMPARE(textbox_widget->getValue(), "+123.4e9");
            TAP_KEY(sf::Keyboard::Right);
            ENTER_TEXT(sf::Keyboard::Num5, '5');
            ENTER_TEXT(sf::Keyboard::Num6, '6');
            ENTER_TEXT(sf::Keyboard::Num7, '7');
            ENTER_TEXT(sf::Keyboard::Num8, '8');
            T_COMPARE(textbox_widget->getValue(), "+123.45678e9");
            T_CHECK(textbox_widget->isValidValue());
            TAP_KEY(sf::Keyboard::Home);
            ENTER_TEXT(sf::Keyboard::Dash, '-');
            T_COMPARE(textbox_widget->getValue(), "-+123.45678e9");
            T_CHECK(!textbox_widget->isValidValue());
            TAP_KEY(sf::Keyboard::Right);
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
            ENTER_TEXT(sf::Keyboard::Num0, '0');
            T_COMPARE(textbox_widget->getValue(), "-0123.45678e9");
            T_CHECK(textbox_widget->isValidValue());
            TAP_KEY(sf::Keyboard::Home);
            ENTER_TEXT(sf::Keyboard::Dash, '-');
            T_COMPARE(textbox_widget->getValue(), "--0123.45678e9");
            T_CHECK(!textbox_widget->isValidValue());
            ENTER_TEXT(sf::Keyboard::Backspace, '\b');
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
        default:                                 mAssert(false, "Unknown cursor type"); return "Unknown";
    }
}

std::string WidgetTests::floatRectToStr(const sf::FloatRect& rect) {
    return "pos: " + sfVec2fToStr(rect.getPosition()) + " size: " + sfVec2fToStr(rect.getSize());
}

std::string WidgetTests::colorToStr(const sf::Color& color) {
    return "(" + utils::color_to_str(color) + ")";
}

std::string WidgetTests::anchorToStr(fw::Widget::Anchor anchor) {
    switch (anchor) {
        case fw::Widget::Anchor::CUSTOM:        return "CUSTOM";
        case fw::Widget::Anchor::TOP_LEFT:      return "TOP_LEFT";
        case fw::Widget::Anchor::TOP_CENTER:    return "TOP_CENTER";
        case fw::Widget::Anchor::TOP_RIGHT:     return "TOP_RIGHT";
        case fw::Widget::Anchor::CENTER_LEFT:   return "CENTER_LEFT";
        case fw::Widget::Anchor::CENTER:        return "CENTER";
        case fw::Widget::Anchor::CENTER_RIGHT:  return "CENTER_RIGHT";
        case fw::Widget::Anchor::BOTTOM_LEFT:   return "BOTTOM_LEFT";
        case fw::Widget::Anchor::BOTTOM_CENTER: return "BOTTOM_CENTER";
        case fw::Widget::Anchor::BOTTOM_RIGHT:  return "BOTTOM_RIGHT";
        default:                                mAssert("Unknown anchor type"); return "Unknown";
    }
}

fw::TextBoxWidget* WidgetTests::initTextBox(fw::Application& application, float width, float height) const {
    application.init("Test window", 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::TextBoxWidget* textbox_widget = application.getWidgets().createWidget<fw::TextBoxWidget>();
    textbox_widget->setCharacterSize(20);
    textbox_widget->setFont(textbox_font);
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(width, height);
    std::string value = "Text";
    textbox_widget->setPosition(position);
    textbox_widget->setSize(size);
    textbox_widget->setValue(value);
    application.advance();
    return textbox_widget;
}

void WidgetTests::genericWidgetTest(const GenericWidgetTest& gwt) {
    fw::Application& application = gwt.application;
    test::Test& test = gwt.test;
    fw::Widget* widget = gwt.widget;
    T_COMPARE(application.getWidgets().getSize(), gwt.total_widgets);
    T_COMPARE(widget->getName(), gwt.name);
    T_COMPARE(widget->getFullName(), gwt.fullname);
    T_COMPARE(widget->isVisualPositionQuantized(), gwt.is_visual_position_quantized);
    T_COMPARE(widget->isVisible(), gwt.is_visible);
    fw::WidgetVisibility wv = widget->checkVisibility();
    T_CHECK(wv.addedToRoot);
    T_CHECK(wv.allParentsVisible);
    T_CHECK(wv.hasUnclippedRegion);
    T_CHECK(wv.nonZeroSize);
    T_CHECK(wv.onScreen);
    T_COMPARE(wv.opaque, gwt.opaque);
    T_CHECK(wv.visibleSetting);
    T_COMPARE(widget->isClickThrough(), gwt.is_click_through);
    T_COMPARE(widget->isMouseOver(), gwt.is_mouse_over);
    T_COMPARE(widget->isFocusable(), gwt.is_focusable);
    T_COMPARE(widget->isFocused(), gwt.is_focused);
    T_COMPARE(widget->getClipChildren(), gwt.clip_children);
    T_COMPARE(widget->getForceCustomCursor(), gwt.force_custom_cursor);
    T_CHECK(widget->getParent() == gwt.parent);
    auto rect_to_str = &WidgetTests::floatRectToStr;
    T_COMPARE(widget->getLocalBounds(), gwt.local_bounds, rect_to_str);
    T_COMPARE(widget->getParentLocalBounds(), gwt.parent_local_bounds, rect_to_str);
    T_COMPARE(widget->getGlobalBounds(), gwt.parent_local_bounds, rect_to_str);
    T_COMPARE(widget->getVisualLocalBounds(), gwt.visual_local_bounds, rect_to_str);
    T_COMPARE(widget->getVisualParentLocalBounds(), gwt.visual_parent_local_bounds, rect_to_str);
    T_COMPARE(widget->getVisualGlobalBounds(), gwt.visual_parent_local_bounds, rect_to_str);
    T_COMPARE(widget->getUnclippedRegion(), gwt.visual_parent_local_bounds, rect_to_str);
    T_COMPARE(widget->getQuantizedUnclippedRegion(), gwt.visual_parent_local_bounds, rect_to_str);
    T_COMPARE(widget->getWidth(), gwt.parent_local_bounds.width);
    T_COMPARE(widget->getHeight(), gwt.parent_local_bounds.height);
    T_COMPARE(widget->getGlobalWidth(), gwt.parent_local_bounds.width);
    T_COMPARE(widget->getGlobalHeight(), gwt.parent_local_bounds.height);
    auto vec2f_to_str = &WidgetTests::sfVec2fToStr;
    T_COMPARE(widget->getSize(), gwt.local_bounds.getSize(), vec2f_to_str);
    auto get_corners = [&](const sf::FloatRect& bounds) {
        std::vector<sf::Vector2f> corners(4);
        corners[0] = bounds.getPosition();
        corners[1] = bounds.getPosition() + sf::Vector2f(gwt.local_bounds.width, 0.0f);
        corners[2] = bounds.getPosition() + sf::Vector2f(0.0f, bounds.height);
        corners[3] = bounds.getPosition() + bounds.getSize();
        return corners;
    };
    std::vector<sf::Vector2f> parent_local_corners = get_corners(gwt.parent_local_bounds);
    std::vector<sf::Vector2f> global_corners = get_corners(gwt.global_bounds);
    std::vector<sf::Vector2f> visual_global_corners = get_corners(gwt.visual_global_bounds);
    T_COMPARE(widget->getTopLeft(), parent_local_corners[0], vec2f_to_str);
    T_COMPARE(widget->getTopRight(), parent_local_corners[1], vec2f_to_str);
    T_COMPARE(widget->getBottomLeft(), parent_local_corners[2], vec2f_to_str);
    T_COMPARE(widget->getBottomRight(), parent_local_corners[3], vec2f_to_str);
    T_COMPARE(widget->getGlobalTopLeft(), global_corners[0], vec2f_to_str);
    T_COMPARE(widget->getGlobalTopRight(), global_corners[1], vec2f_to_str);
    T_COMPARE(widget->getGlobalBottomLeft(), global_corners[2], vec2f_to_str);
    T_COMPARE(widget->getGlobalBottomRight(), global_corners[3], vec2f_to_str);
    T_COMPARE(widget->getVisualGlobalTopLeft(), visual_global_corners[0], vec2f_to_str);
    T_COMPARE(widget->getVisualGlobalTopRight(), visual_global_corners[1], vec2f_to_str);
    T_COMPARE(widget->getVisualGlobalBottomLeft(), visual_global_corners[2], vec2f_to_str);
    T_COMPARE(widget->getVisualGlobalBottomRight(), visual_global_corners[3], vec2f_to_str);
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

GenericWidgetTest::GenericWidgetTest(
    fw::Application& application,
    test::Test& test
) : application(application), test(test) { }
