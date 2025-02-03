#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_pending.h"
#include "widgets/widgets_common.h"

WidgetTestsPending::WidgetTestsPending(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : WidgetTest(name, parent, required_nodes) {
    test::Test* pending_move_test = addTest("pending_move", [&](test::Test& test) { pendingMoveTest(test); });
    //test::Test* pending_move_delete = addTest("pending_move_delete", [&](test::Test& test) { pendingDeleteTest(test); });
    //test::Test* pending_move_set_parent = addTest("pending_move_set_parent", [&](test::Test& test) { pendingSetParentTest(test); });
}

void WidgetTestsPending::pendingMoveTest(test::Test& test) {
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
        widgets.addPendingMove(rectangle_2_widget, 0);
    };
    application.advance();
    if (T_COMPARE(root_widget->getChildrenCount(), 2)) {
        T_CHECK(root_widget->getChild(0) == rectangle_1_widget);
        T_CHECK(root_widget->getChild(1) == rectangle_2_widget);
    }

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

void WidgetTestsPending::pendingDeleteTest(test::Test& test) {
}

void WidgetTestsPending::pendingSetParentTest(test::Test& test) {
}
