#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_post_actions.h"
#include "widgets/widgets_common.h"

WidgetTestsPostActions::WidgetTestsPostActions(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : WidgetTest(name, parent, required_nodes) {
    test::Test* move_test = addTest("move", [&](test::Test& test) { moveTest(test); });
    test::Test* delete_test = addTest("delete", [&](test::Test& test) { deleteTest(test); });
    test::Test* set_parent_test = addTest("set_parent", [&](test::Test& test) { setParentTest(test); });
}

void WidgetTestsPostActions::moveTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();

    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(100.0f, 100.0f);
    fw::WidgetList& widgets = application.getWidgets();
    fw::Widget* root_widget = widgets.getRootWidget();
    fw::RectangleWidget* rectangle_1_widget = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_2_widget = application.getWidgets().createRectangleWidget(size);
    rectangle_1_widget->setPosition(position);
    rectangle_2_widget->setPosition(position);
    rectangle_2_widget->OnLeftClick += [&](const sf::Vector2f& pos) {
        widgets.addPostAction([=](fw::WidgetList& widget_list) {
            rectangle_2_widget->moveToIndex(0);
        }, fw::PostActionStage::MOVE);
    };
    application.advance();
    if (T_COMPARE(root_widget->getChildrenCount(), 2)) {
        T_CHECK(root_widget->getChild(0) == rectangle_1_widget);
        T_CHECK(root_widget->getChild(1) == rectangle_2_widget);
    }

    T_ASSERT_NO_ERRORS();
    sf::Vector2f rect_2_center = rectangle_2_widget->getGlobalCenter();
    application.mouseMove(rect_2_center);
	application.advance();
	application.mouseLeftClick(rect_2_center);
	application.advance();
    if (T_COMPARE(root_widget->getChildrenCount(), 2)) {
        T_CHECK(root_widget->getChild(0) == rectangle_2_widget);
        T_CHECK(root_widget->getChild(1) == rectangle_1_widget);
    }
}

void WidgetTestsPostActions::deleteTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();

    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(100.0f, 100.0f);
    fw::WidgetList& widgets = application.getWidgets();
    fw::Widget* root_widget = widgets.getRootWidget();
    fw::RectangleWidget* rectangle_widget = application.getWidgets().createRectangleWidget(size);
    rectangle_widget->setPosition(position);
    rectangle_widget->OnLeftClick += [&](const sf::Vector2f& pos) {
        widgets.addPostAction([=](fw::WidgetList& widget_list) {
            rectangle_widget->remove();
        }, fw::PostActionStage::REMOVE);
    };
    application.advance();
    if (T_COMPARE(root_widget->getChildrenCount(), 1)) {
        T_CHECK(root_widget->getChild(0) == rectangle_widget);
    }

    T_ASSERT_NO_ERRORS();
    sf::Vector2f rec_center = rectangle_widget->getGlobalCenter();
    application.mouseMove(rec_center);
    application.advance();
    application.mouseLeftClick(rec_center);
    application.advance();
    T_COMPARE(root_widget->getChildrenCount(), 0);
}

void WidgetTestsPostActions::setParentTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();

    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(100.0f, 100.0f);
    fw::WidgetList& widgets = application.getWidgets();
    fw::Widget* root_widget = widgets.getRootWidget();
    fw::RectangleWidget* rectangle_1_widget = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_2_widget = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_3_widget = application.getWidgets().createRectangleWidget(size);
    rectangle_3_widget->setParent(rectangle_1_widget);
    rectangle_1_widget->setPosition(position);
    rectangle_2_widget->setPosition(position);
    rectangle_3_widget->OnLeftClick += [&](const sf::Vector2f& pos) {
        widgets.addPostAction([=](fw::WidgetList& widget_list) {
            rectangle_3_widget->setParent(rectangle_2_widget);
        }, fw::PostActionStage::SET_PARENT);
    };
    application.advance();
    if (T_COMPARE(root_widget->getChildrenCount(), 2)) {
        T_CHECK(root_widget->getChild(0) == rectangle_1_widget);
        T_CHECK(root_widget->getChild(1) == rectangle_2_widget);
        if (T_COMPARE(rectangle_1_widget->getChildrenCount(), 1)) {
			T_CHECK(rectangle_1_widget->getChild(0) == rectangle_3_widget);
		}
        T_COMPARE(rectangle_2_widget->getChildrenCount(), 0);
    }

    T_ASSERT_NO_ERRORS();
    sf::Vector2f rect_3_center = rectangle_3_widget->getGlobalCenter();
    application.mouseMove(rect_3_center);
    application.advance();
    application.mouseLeftClick(rect_3_center);
    application.advance();
    if (T_COMPARE(root_widget->getChildrenCount(), 2)) {
        T_CHECK(root_widget->getChild(0) == rectangle_1_widget);
        T_CHECK(root_widget->getChild(1) == rectangle_2_widget);
        T_COMPARE(rectangle_1_widget->getChildrenCount(), 0);
        if (T_COMPARE(rectangle_2_widget->getChildrenCount(), 1)) {
            T_CHECK(rectangle_2_widget->getChild(0) == rectangle_3_widget);
        }
    }
}
