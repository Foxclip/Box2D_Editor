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
            auto vec2f_to_str = &WidgetTests::sfVec2fToStr;
            tCompare(rectangle_widget->getSize(), size, vec2f_to_str);
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
    test::Test* widget_mouse_events_2_test = list->addTest(
        "widget_mouse_events_2",
        {
            root_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
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
            tCheck(!rectangle_widget_1->isMouseOver());
            tCheck(!mouse_entered_1);
            tCheck(!mouse_pressed_1);
            tCheck(!mouse_released_1);
            tCheck(!mouse_exited_1);
            tCheck(!rectangle_widget_2->isMouseOver());
            tCheck(!mouse_entered_2);
            tCheck(!mouse_pressed_2);
            tCheck(!mouse_released_2);
            tCheck(!mouse_exited_2);
            application.mouseMove(mouse_pos_1);
            application.advance();
            tCheck(rectangle_widget_1->isMouseOver());
            tCheck(mouse_entered_1);
            tCheck(!mouse_pressed_1);
            tCheck(!mouse_released_1);
            tCheck(!mouse_exited_1);
            tCheck(rectangle_widget_2->isMouseOver());
            tCheck(mouse_entered_2);
            tCheck(!mouse_pressed_2);
            tCheck(!mouse_released_2);
            tCheck(!mouse_exited_2);
            application.mouseLeftPress();
            application.advance();
            tCheck(mouse_entered_1);
            tCheck(mouse_pressed_1);
            tCheck(!mouse_released_1);
            tCheck(!mouse_exited_1);
            tCheck(mouse_entered_2);
            tCheck(mouse_pressed_2);
            tCheck(!mouse_released_2);
            tCheck(!mouse_exited_2);
            application.mouseLeftRelease();
            application.advance();
            tCheck(mouse_entered_1);
            tCheck(mouse_pressed_1);
            tCheck(!mouse_released_1); // no focused widget, so release is not processed
            tCheck(!mouse_exited_1);
            tCheck(mouse_entered_2);
            tCheck(mouse_pressed_2);
            tCheck(!mouse_released_2);
            tCheck(!mouse_exited_2);
            application.mouseMove(mouse_pos_2);
            application.advance();
            tCheck(!rectangle_widget_1->isMouseOver());
            tCheck(mouse_entered_1);
            tCheck(mouse_pressed_1);
            tCheck(!mouse_released_1);
            tCheck(mouse_exited_1);
            tCheck(!rectangle_widget_2->isMouseOver());
            tCheck(mouse_entered_2);
            tCheck(mouse_pressed_2);
            tCheck(!mouse_released_2);
            tCheck(mouse_exited_2);
        }
    );
    test::Test* coordinates_test = list->addTest(
        "coordinates",
        {
            set_parent_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
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
            tVec2ApproxCompare(child_widget->toGlobal(sf::Vector2f(10.0f, 5.0f)), sf::Vector2f(140.0f, 135.0f));
            tVec2ApproxCompare(child_widget->toLocal(sf::Vector2f(140.0f, 135.0f)), sf::Vector2f(10.0f, 5.0f));
        }
    );
    test::Test* find_test = list->addTest(
        "find",
        {
            set_parent_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::RectangleWidget* parent_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            fw::RectangleWidget* child_widget = application.getWidgets().createWidget<fw::RectangleWidget>();
            parent_widget->setName("parent");
            child_widget->setName("child");
            child_widget->setParent(parent_widget);
            tCheck(application.getWidgets().find("root") == application.getWidgets().getRootWidget());
            tCheck(application.getWidgets().find("parent") == parent_widget);
            tCheck(application.getWidgets().find("child") == child_widget);
        }
    );
    test::Test* anchor_test = list->addTest(
        "anchor",
        {
            set_parent_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
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
            tVec2ApproxCompare(child_widget->getPosition(), sf::Vector2f() + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::TOP_CENTER);
            tVec2ApproxCompare(child_widget->getPosition(), sf::Vector2f(parent_size.x / 2.0f, 0.0f) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::TOP_RIGHT);
            tVec2ApproxCompare(child_widget->getPosition(), sf::Vector2f(parent_size.x, 0.0f) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::CENTER_LEFT);
            tVec2ApproxCompare(child_widget->getPosition(), sf::Vector2f(0.0f, parent_size.y / 2.0f) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::CENTER);
            tVec2ApproxCompare(child_widget->getPosition(), sf::Vector2f(parent_size.x / 2.0f, parent_size.y / 2.0f) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::CENTER_RIGHT);
            tVec2ApproxCompare(child_widget->getPosition(), sf::Vector2f(parent_size.x, parent_size.y / 2.0f) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::BOTTOM_LEFT);
            tVec2ApproxCompare(child_widget->getPosition(), sf::Vector2f(0.0f, parent_size.y) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::BOTTOM_CENTER);
            tVec2ApproxCompare(child_widget->getPosition(), sf::Vector2f(parent_size.x / 2.0f, parent_size.y) + anchor_offset);
            child_widget->setParentAnchor(fw::Widget::Anchor::BOTTOM_RIGHT);
            tVec2ApproxCompare(child_widget->getPosition(), sf::Vector2f(parent_size.x, parent_size.y) + anchor_offset);
        }
    );
    test::Test* text_widget_test = list->addTest(
        "text_widget",
        {
            root_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::TextWidget* text_widget = application.getWidgets().createWidget<fw::TextWidget>();
            text_widget->setCharacterSize(20);
            sf::Font font;
            font.loadFromFile("fonts/verdana.ttf");
            text_widget->setFont(font);
            text_widget->setString("Text");
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f size(41.0f, 20.0f);
            sf::Vector2f local_bounds_offset(0.0f, 6.0f);
            text_widget->setPosition(position);
            tAssert(tCheck(text_widget));
            tCompare(application.getWidgets().getSize(), 2);
            tCompare(text_widget->getName(), "text");
            tCompare(text_widget->getFullName(), "root|text");
            tCheck(text_widget->isVisualPositionQuantized());
            tCheck(text_widget->isVisible());
            fw::WidgetVisibility wv = text_widget->checkVisibility();
            tCheck(wv.addedToRoot);
            tCheck(wv.allParentsVisible);
            tCheck(wv.hasUnclippedRegion);
            tCheck(wv.nonZeroSize);
            tCheck(wv.onScreen);
            tCheck(wv.opaque);
            tCheck(wv.visibleSetting);
            tCheck(text_widget->isClickThrough());
            tCheck(!text_widget->isMouseOver());
            tCheck(!text_widget->isFocusable());
            tCheck(!text_widget->isFocused());
            tCheck(!text_widget->getClipChildren());
            tCheck(!text_widget->getForceCustomCursor());
            tCompare(text_widget->getChildren().size(), 0);
            sf::FloatRect local_bounds = sf::FloatRect(sf::Vector2f(), size);
            sf::FloatRect parent_local_bounds = sf::FloatRect(position, size);
            sf::FloatRect visual_local_bounds = sf::FloatRect(local_bounds_offset, size - local_bounds_offset);
            sf::FloatRect visual_parent_local_bounds = sf::FloatRect(position + local_bounds_offset, size - local_bounds_offset);
            auto rect_to_str = &WidgetTests::floatRectToStr;
            tCompare(text_widget->getLocalBounds(), local_bounds, rect_to_str);
            tCompare(text_widget->getParentLocalBounds(), parent_local_bounds, rect_to_str);
            tCompare(text_widget->getGlobalBounds(), parent_local_bounds, rect_to_str);
            tCompare(text_widget->getVisualLocalBounds(), visual_local_bounds, rect_to_str);
            tCompare(text_widget->getVisualParentLocalBounds(), visual_parent_local_bounds, rect_to_str);
            tCompare(text_widget->getVisualGlobalBounds(), visual_parent_local_bounds, rect_to_str);
            tCompare(text_widget->getUnclippedRegion(), visual_parent_local_bounds, rect_to_str);
            tCompare(text_widget->getQuantizedUnclippedRegion(), visual_parent_local_bounds, rect_to_str);
            tCompare(text_widget->getWidth(), parent_local_bounds.width);
            tCompare(text_widget->getHeight(), parent_local_bounds.height);
            auto vec2f_to_str = &WidgetTests::sfVec2fToStr;
            tCompare(text_widget->getSize(), size, vec2f_to_str);
            tCompare(text_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
        }
    );
    test::Test* checkbox_widget_basic_test = list->addTest(
        "checkbox_widget_basic",
        {
            rectangle_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::CheckboxWidget* checkbox_widget = application.getWidgets().createWidget<fw::CheckboxWidget>();
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f size(20.0f, 20.0f);
            checkbox_widget->setPosition(position);
            checkbox_widget->setSize(size);
            tAssert(tCheck(checkbox_widget));
            tCompare(application.getWidgets().getSize(), 3);
            tCompare(checkbox_widget->getName(), "checkbox");
            tCompare(checkbox_widget->getFullName(), "root|checkbox");
            tCheck(!checkbox_widget->isVisualPositionQuantized());
            tCheck(checkbox_widget->isVisible());
            fw::WidgetVisibility wv = checkbox_widget->checkVisibility();
            tCheck(wv.addedToRoot);
            tCheck(wv.allParentsVisible);
            tCheck(wv.hasUnclippedRegion);
            tCheck(wv.nonZeroSize);
            tCheck(wv.onScreen);
            tCheck(wv.opaque);
            tCheck(wv.visibleSetting);
            tCheck(!checkbox_widget->isClickThrough());
            tCheck(!checkbox_widget->isMouseOver());
            tCheck(checkbox_widget->isFocusable());
            tCheck(!checkbox_widget->isFocused());
            tCheck(!checkbox_widget->getClipChildren());
            tCheck(!checkbox_widget->getForceCustomCursor());
            if (tCompare(checkbox_widget->getChildren().size(), 1)) {
                fw::RectangleWidget* check_widget = dynamic_cast<fw::RectangleWidget*>(checkbox_widget->getChild(0));
                tCheck(check_widget, "Check widget is not a RectangleWidget");
                tCompare(check_widget->getParentAnchor(), fw::Widget::Anchor::CENTER, &WidgetTests::anchorToStr);
            }
            sf::FloatRect local_bounds = sf::FloatRect(sf::Vector2f(), size);
            sf::FloatRect parent_local_bounds = sf::FloatRect(position, size);
            auto rect_to_str = &WidgetTests::floatRectToStr;
            tCompare(checkbox_widget->getLocalBounds(), local_bounds, rect_to_str);
            tCompare(checkbox_widget->getParentLocalBounds(), parent_local_bounds, rect_to_str);
            tCompare(checkbox_widget->getGlobalBounds(), parent_local_bounds, rect_to_str);
            tCompare(checkbox_widget->getVisualLocalBounds(), local_bounds, rect_to_str);
            tCompare(checkbox_widget->getVisualParentLocalBounds(), parent_local_bounds, rect_to_str);
            tCompare(checkbox_widget->getVisualGlobalBounds(), parent_local_bounds, rect_to_str);
            tCompare(checkbox_widget->getUnclippedRegion(), parent_local_bounds, rect_to_str);
            tCompare(checkbox_widget->getQuantizedUnclippedRegion(), parent_local_bounds, rect_to_str);
            tCompare(checkbox_widget->getWidth(), parent_local_bounds.width);
            tCompare(checkbox_widget->getHeight(), parent_local_bounds.height);
            auto vec2f_to_str = &WidgetTests::sfVec2fToStr;
            tCompare(checkbox_widget->getSize(), size, vec2f_to_str);
            tCompare(checkbox_widget->getFillColor(), sf::Color(50, 50, 50), &WidgetTests::colorToStr);
        }
    );
    test::Test* checkbox_widget_toggle_test = list->addTest(
        "checkbox_widget_toggle",
        {
            checkbox_widget_basic_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            fw::CheckboxWidget* checkbox_widget = application.getWidgets().createWidget<fw::CheckboxWidget>();
            tCheck(!checkbox_widget->getValue());
            application.mouseMove(fw::to2i(checkbox_widget->getGlobalCenter()));
            application.advance();
            application.mouseLeftPress();
            application.advance();
            tCheck(checkbox_widget->getValue());
            application.mouseLeftRelease();
            application.advance();
            tCheck(checkbox_widget->getValue());
        }
    );
    test::Test* container_widget_basic_test = list->addTest(
        "container_widget_basic",
        {
            rectangle_widget_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
            application.start(true);
            application.mouseMove(400, 300);
            application.advance();
            fw::ContainerWidget* container_widget = application.getWidgets().createWidget<fw::ContainerWidget>();
            sf::Vector2f position(100.0f, 100.0f);
            sf::Vector2f size(100.0f, 100.0f);
            container_widget->setPosition(position);
            container_widget->setSize(size);
            tAssert(tCheck(container_widget));
            tCompare(application.getWidgets().getSize(), 2);
            tCompare(container_widget->getName(), "container");
            tCompare(container_widget->getFullName(), "root|container");
            tCheck(!container_widget->isVisualPositionQuantized());
            tCheck(container_widget->isVisible());
            fw::WidgetVisibility wv = container_widget->checkVisibility();
            tCheck(wv.addedToRoot);
            tCheck(wv.allParentsVisible);
            tCheck(wv.hasUnclippedRegion);
            tCheck(wv.nonZeroSize);
            tCheck(wv.onScreen);
            tCheck(wv.opaque);
            tCheck(wv.visibleSetting);
            tCheck(container_widget->isClickThrough());
            tCheck(!container_widget->isMouseOver());
            tCheck(!container_widget->isFocusable());
            tCheck(!container_widget->isFocused());
            tCheck(!container_widget->getClipChildren());
            tCheck(!container_widget->getForceCustomCursor());
            sf::FloatRect local_bounds = sf::FloatRect(sf::Vector2f(), size);
            sf::FloatRect parent_local_bounds = sf::FloatRect(position, size);
            auto rect_to_str = &WidgetTests::floatRectToStr;
            tCompare(container_widget->getLocalBounds(), local_bounds, rect_to_str);
            tCompare(container_widget->getParentLocalBounds(), parent_local_bounds, rect_to_str);
            tCompare(container_widget->getGlobalBounds(), parent_local_bounds, rect_to_str);
            tCompare(container_widget->getVisualLocalBounds(), local_bounds, rect_to_str);
            tCompare(container_widget->getVisualParentLocalBounds(), parent_local_bounds, rect_to_str);
            tCompare(container_widget->getVisualGlobalBounds(), parent_local_bounds, rect_to_str);
            tCompare(container_widget->getUnclippedRegion(), parent_local_bounds, rect_to_str);
            tCompare(container_widget->getQuantizedUnclippedRegion(), parent_local_bounds, rect_to_str);
            tCompare(container_widget->getWidth(), parent_local_bounds.width);
            tCompare(container_widget->getHeight(), parent_local_bounds.height);
            auto vec2f_to_str = &WidgetTests::sfVec2fToStr;
            tCompare(container_widget->getSize(), size, vec2f_to_str);
            tCompare(container_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
        }
    );
    test::Test* container_widget_children_test = list->addTest(
        "container_widget_children",
        {
            container_widget_basic_test
        },
        [&](test::Test& test) {
            fw::Application application;
            application.init("Test window", 800, 600, 0);
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
            tVec2ApproxCompare(container_widget->getSize(), container_size);
            application.advance();
            tVec2ApproxCompare(container_widget->getSize(), sf::Vector2f(horizontal_padding, vertical_padding));
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
            tVec2ApproxCompare(container_widget->getSize(), new_container_size);
            tVec2ApproxCompare(child_1_widget->getPosition(), sf::Vector2f(horizontal_padding, vertical_padding));
            tVec2ApproxCompare(child_2_widget->getPosition(), sf::Vector2f(horizontal_padding + child_1_widget->getWidth() + horizontal_padding, vertical_padding));
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
