#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_basic.h"
#include "widgets/widgets_common.h"

WidgetTestsBasic::WidgetTestsBasic(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : WidgetTest(name, parent, required_nodes) {
    test::Test* root_widget_test = addTest("root_widget", [&](test::Test& test) { rootWidgetTest(test); });
    test::Test* empty_widget_test = addTest("empty_widget", { root_widget_test }, [&](test::Test& test) { emptyWidgetTest(test); });
    test::Test* rectangle_widget_test = addTest("rectangle_widget", { root_widget_test }, [&](test::Test& test) { rectangleWidgetTest(test); });
    test::Test* polygon_widget_basic_test = addTest("polygon_widget_basic", { root_widget_test }, [&](test::Test& test) { polygonWidgetBasicTest(test); });
    test::Test* set_parent_test = addTest("set_parent", { root_widget_test }, [&](test::Test& test) { setParentTest(test); });
    test::Test* duplicate_rect_test = addTest("duplicate_rect", { rectangle_widget_test }, [&](test::Test& test) { duplicateRectangleTest(test); });
    test::Test* duplicate_polygon_test = addTest("duplicate_polygon", { polygon_widget_basic_test }, [&](test::Test& test) { duplicatePolygonTest(test); });
    test::Test* duplicate_children_test = addTest("duplicate_children", { set_parent_test }, [&](test::Test& test) { duplicateChildrenTest(test); });
    test::Test* duplicate_without_children_test = addTest("duplicate_without_children", { set_parent_test }, [&](test::Test& test) { duplicateWithoutChildrenTest(test); });
    test::Test* widget_mouse_events_1_test = addTest("mouse_events_1", { root_widget_test }, [&](test::Test& test) { widgetMouseEvents1(test); });
    test::Test* widget_mouse_events_2_test = addTest("mouse_events_2", { root_widget_test }, [&](test::Test& test) { widgetMouseEvents2(test); });
    test::Test* drag_gesture_event_test = addTest("drag_gesture_event", { root_widget_test }, [&](test::Test& test) { dragGestureEventTest(test); });
    test::Test* events_test = addTest("events", { root_widget_test }, [&](test::Test& test) { eventsTest(test); });
    test::Test* coordinates_test = addTest("coordinates", { set_parent_test }, [&](test::Test& test) { coordinatesTest(test); });
    test::Test* find_test = addTest("find", { set_parent_test }, [&](test::Test& test) { findTest(test); });
    test::Test* anchor_test = addTest("anchor", { set_parent_test }, [&](test::Test& test) { anchorTest(test); });
    test::Test* remove_1_test = addTest("remove_1", { set_parent_test }, [&](test::Test& test) { remove1Test(test); });
    test::Test* remove_2_test = addTest("remove_2", { set_parent_test }, [&](test::Test& test) { remove2Test(test); });
}

void WidgetTestsBasic::rootWidgetTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::EmptyWidget* root_widget = dynamic_cast<fw::EmptyWidget*>(application.getWidgets().getRootWidget());
    T_ASSERT(T_CHECK(root_widget));

    GenericWidgetTest gwt(application, test);
    gwt.widget = root_widget;
    gwt.total_widgets = 1;
    gwt.type = fw::Widget::WidgetType::Empty;
    gwt.name = "root";
    gwt.fullname = "root";
    gwt.is_visual_position_quantized = false;
    fw::WidgetVisibility visibility;
    visibility.addedToRoot = true;
    visibility.allParentsVisible = true;
    visibility.hasUnclippedRegion = true;
    visibility.nonZeroSize = true;
    visibility.onScreen = true;
    visibility.opaque = false;
    visibility.renderableSetting = false;
    visibility.visibleSetting = true;
    gwt.visibility = visibility;
    gwt.is_click_through = false;
    gwt.is_mouse_over = true;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
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
    T_WRAP_CONTAINER(WidgetTests::genericWidgetTest(gwt));

    T_COMPARE(root_widget->getFillColor(), sf::Color::Transparent, &WidgetTests::colorToStr);

    T_COMPARE(root_widget->getParentChain().size(), 0);
    T_COMPARE(root_widget->getChildren().size(), 0);
}

void WidgetTestsBasic::emptyWidgetTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::EmptyWidget* empty_widget = application.getWidgets().createEmptyWidget();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(empty_widget));
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(50.0f, 30.0f);
    empty_widget->setPosition(position);
    empty_widget->setSize(size);

    GenericWidgetTest gwt(application, test);
    gwt.widget = empty_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Empty;
    gwt.name = "empty";
    gwt.fullname = "root|empty";
    gwt.is_visual_position_quantized = false;
    fw::WidgetVisibility visibility;
    visibility.addedToRoot = true;
    visibility.allParentsVisible = true;
    visibility.hasUnclippedRegion = true;
    visibility.nonZeroSize = true;
    visibility.onScreen = true;
    visibility.opaque = false;
    visibility.renderableSetting = false;
    visibility.visibleSetting = true;
    gwt.visibility = visibility;
    gwt.is_click_through = true;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
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
    T_WRAP_CONTAINER(WidgetTests::genericWidgetTest(gwt));

    T_COMPARE(empty_widget->getFillColor(), sf::Color::Transparent, &WidgetTests::colorToStr);

    T_COMPARE(empty_widget->getChildren().size(), 0);
}

void WidgetTestsBasic::rectangleWidgetTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::RectangleWidget* rectangle_widget = application.getWidgets().createRectangleWidget(size);
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(rectangle_widget));
    sf::Vector2f position(100.0f, 100.0f);
    rectangle_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = rectangle_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Rectangle;
    gwt.name = "rectangle";
    gwt.fullname = "root|rectangle";
    gwt.is_visual_position_quantized = false;
    fw::WidgetVisibility visibility;
    visibility.addedToRoot = true;
    visibility.allParentsVisible = true;
    visibility.hasUnclippedRegion = true;
    visibility.nonZeroSize = true;
    visibility.onScreen = true;
    visibility.opaque = true;
    visibility.renderableSetting = true;
    visibility.visibleSetting = true;
    gwt.visibility = visibility;
    gwt.is_click_through = true;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
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
    T_WRAP_CONTAINER(WidgetTests::genericWidgetTest(gwt));

    T_CHECK(rectangle_widget->getParent() == root_widget);
    const CompVector<fw::Widget*>& parent_chain = rectangle_widget->getParentChain();
    T_ASSERT(T_COMPARE(parent_chain.size(), 1));
    T_CHECK(parent_chain[0] == root_widget);
    T_COMPARE(rectangle_widget->getChildren().size(), 0);
    const CompVector<fw::Widget*>& root_children = root_widget->getChildren();
    T_ASSERT(T_COMPARE(root_children.size(), 1));
    T_CHECK(root_children[0] == rectangle_widget);
    T_CHECK(root_widget->getChild(0) == rectangle_widget);

    T_COMPARE(rectangle_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
}

void WidgetTestsBasic::polygonWidgetBasicTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    std::vector<sf::Vector2f> vertices = fw::get_regular_polygon<sf::Vector2f>(6, 1.0f);
    fw::PolygonWidget* polygon_widget = application.getWidgets().createPolygonWidget(vertices);
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(polygon_widget));
    sf::Vector2f position(100.0f, 100.0f);
    polygon_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = polygon_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Polygon;
    gwt.name = "polygon";
    gwt.fullname = "root|polygon";
    gwt.is_visual_position_quantized = false;
    fw::WidgetVisibility visibility;
    visibility.addedToRoot = true;
    visibility.allParentsVisible = true;
    visibility.hasUnclippedRegion = true;
    visibility.nonZeroSize = true;
    visibility.onScreen = true;
    visibility.opaque = true;
    visibility.renderableSetting = true;
    visibility.visibleSetting = true;
    gwt.visibility = visibility;
    gwt.is_click_through = true;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
    gwt.is_focused = false;
    gwt.clip_children = false;
    gwt.force_custom_cursor = false;
    gwt.parent = root_widget;
    sf::Vector2f local_pos(-1.0f, -vertices[2].y);
    sf::Vector2f size(2.0f, vertices[2].y * 2);
    gwt.local_bounds = sf::FloatRect(local_pos, size);
    gwt.global_bounds = sf::FloatRect(position + local_pos, size);
    gwt.parent_local_bounds = gwt.global_bounds;
    gwt.visual_local_bounds = gwt.local_bounds;
    gwt.visual_global_bounds = gwt.global_bounds;
    gwt.visual_parent_local_bounds = gwt.global_bounds;
    T_WRAP_CONTAINER(WidgetTests::genericWidgetTest(gwt));

    T_CHECK(polygon_widget->getParent() == root_widget);
    T_COMPARE(polygon_widget->getChildren().size(), 0);

    T_COMPARE(polygon_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
}

void WidgetTestsBasic::setParentTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    sf::Vector2f parent_local_pos_before = parent_widget->getPosition();
    sf::Vector2f parent_global_pos_before = parent_widget->getGlobalPosition();
    sf::Vector2f child_local_pos_before = child_widget->getPosition();
    sf::Vector2f child_global_pos_before = child_widget->getGlobalPosition();
    child_widget->setParent(parent_widget);
    T_CHECK(child_widget->getParent() == parent_widget);
    const CompVector<fw::Widget*>& parent_chain = child_widget->getParentChain();
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

void WidgetTestsBasic::duplicateRectangleTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();

    fw::WidgetList& widgets = application.getWidgets();
    fw::RectangleWidget* widget = widgets.createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* copy = widgets.duplicateWidget(widget);
    application.advance();
    if (T_COMPARE(widgets.getAllWidgets().size(), 3)) {
        T_CHECK(widgets.getAllWidgets()[0] == widgets.getRootWidget());
        T_CHECK(widgets.getAllWidgets()[1] == widget);
		T_CHECK(widgets.getAllWidgets()[2] == copy);
    }
    T_CHECK(copy->getGlobalTransform() == widget->getGlobalTransform());
	T_VEC2_COMPARE(copy->getGlobalPosition(), widget->getGlobalPosition());
    T_VEC2_COMPARE(copy->getSize(), widget->getSize());
	T_CHECK(copy->getParent() == widget->getParent());
}

void WidgetTestsBasic::duplicatePolygonTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();

    sf::Vector2f pos = sf::Vector2f(100.0f, 100.0f);
    sf::Vector2f offset = sf::Vector2f(100.0f, 0.0f);
    fw::WidgetList& widgets = application.getWidgets();
    fw::PolygonWidget* widget = widgets.createPolygonWidget(6);
    widget->setFillColor(sf::Color::Red);
    widget->setPosition(pos);
    fw::PolygonWidget* copy = widgets.duplicateWidget(widget);
    copy->setFillColor(sf::Color::Green);
    copy->setPosition(copy->getPosition() + offset);
    copy->setRotation(fw::to_radians(180.0f));
    application.advance();
    if (T_COMPARE(widgets.getAllWidgets().size(), 3)) {
        T_CHECK(widgets.getAllWidgets()[0] == widgets.getRootWidget());
        T_CHECK(widgets.getAllWidgets()[1] == widget);
        T_CHECK(widgets.getAllWidgets()[2] == copy);
    }
    T_VEC2_COMPARE(copy->getGlobalPosition(), widget->getGlobalPosition() + offset);
    T_COMPARE(copy->getRotation(), fw::to_radians(180.0f));
    T_CHECK(copy->getParent() == widget->getParent());
    T_CHECK(copy->getVertices() == widget->getVertices());
}

void WidgetTestsBasic::duplicateChildrenTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();

    sf::Vector2f pos = sf::Vector2f(100.0f, 100.0f);
    sf::Vector2f child_1_offset = sf::Vector2f(-20.0f, 20.0f);
	sf::Vector2f child_2_offset = sf::Vector2f(0.0f, 20.0f);
    sf::Vector2f child_3_offset = sf::Vector2f(20.0f, 20.0f);
    fw::WidgetList& widgets = application.getWidgets();
    fw::RectangleWidget* widget = widgets.createRectangleWidget(10.0f, 10.0f);
    widget->setPosition(pos);
    fw::RectangleWidget* child_1 = widgets.createRectangleWidget(10.0f, 10.0f);
    child_1->setName("red rectangle");
    child_1->setFillColor(sf::Color::Red);
    child_1->setParent(widget);
    child_1->setPosition(child_1_offset);
    fw::RectangleWidget* child_2 = widgets.createRectangleWidget(10.0f, 10.0f);
	child_2->setName("green rectangle");
	child_2->setFillColor(sf::Color::Green);
    child_2->setParent(widget);
    child_2->setPosition(child_2_offset);
    fw::RectangleWidget* child_3 = widgets.createRectangleWidget(10.0f, 10.0f);
	child_3->setName("blue rectangle");
	child_3->setFillColor(sf::Color::Blue);
	child_3->setParent(widget);
    child_3->setPosition(child_3_offset);
    application.advance();

    sf::Vector2f offset = sf::Vector2f(100.0f, 0.0f);
    sf::Vector2f pos2 = pos + offset;
    fw::RectangleWidget* copy = widgets.duplicateWidget(widget);
    copy->setPosition(pos2);
    application.advance();

    T_COMPARE(widgets.getAllWidgets().size(), 9);
    fw::Widget* root_widget = widgets.getRootWidget();
    const CompVector<fw::Widget*>& root_children = root_widget->getChildren();
    if (T_COMPARE(root_children.size(), 2)) {
        T_CHECK(root_children[0] == widget);
        T_CHECK(root_children[1] == copy);
    }
    const CompVector<fw::Widget*>& widget_children = widget->getChildren();
    if (T_COMPARE(widget_children.size(), 3)) {
        T_CHECK(widget_children[0] == child_1);
		T_CHECK(widget_children[1] == child_2);
		T_CHECK(widget_children[2] == child_3);
    }
    const CompVector<fw::Widget*>& copy_children = copy->getChildren();
    if (T_COMPARE(copy_children.size(), 3)) {
        T_COMPARE(copy_children[0]->getName(), child_1->getName());
		T_COMPARE(copy_children[1]->getName(), child_2->getName());
		T_COMPARE(copy_children[2]->getName(), child_3->getName());
    }

    T_ASSERT_NO_ERRORS();
    T_VEC2_COMPARE(widget->getGlobalPosition(), pos);
    T_VEC2_COMPARE(copy->getGlobalPosition(), pos2);
    sf::Vector2f child_1_copy_pos = child_1->getGlobalPosition() + offset;
    sf::Vector2f child_2_copy_pos = child_2->getGlobalPosition() + offset;
    sf::Vector2f child_3_copy_pos = child_3->getGlobalPosition() + offset;
    T_VEC2_COMPARE(copy_children[0]->getGlobalPosition(), child_1_copy_pos);
    T_VEC2_COMPARE(copy_children[1]->getGlobalPosition(), child_2_copy_pos);
    T_VEC2_COMPARE(copy_children[2]->getGlobalPosition(), child_3_copy_pos);
    T_COMPARE(widget_children[0]->getFillColor(), child_1->getFillColor(), &WidgetTests::colorToStr);
    T_COMPARE(widget_children[1]->getFillColor(), child_2->getFillColor(), &WidgetTests::colorToStr);
    T_COMPARE(widget_children[2]->getFillColor(), child_3->getFillColor(), &WidgetTests::colorToStr);
}

void WidgetTestsBasic::duplicateWithoutChildrenTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();

    sf::Vector2f pos = sf::Vector2f(100.0f, 100.0f);
    sf::Vector2f child_1_offset = sf::Vector2f(-20.0f, 20.0f);
    sf::Vector2f child_2_offset = sf::Vector2f(0.0f, 20.0f);
    sf::Vector2f child_3_offset = sf::Vector2f(20.0f, 20.0f);
    fw::WidgetList& widgets = application.getWidgets();
    fw::RectangleWidget* widget = widgets.createRectangleWidget(10.0f, 10.0f);
    widget->setPosition(pos);
    fw::RectangleWidget* child_1 = widgets.createRectangleWidget(10.0f, 10.0f);
    child_1->setName("red rectangle");
    child_1->setFillColor(sf::Color::Red);
    child_1->setParent(widget);
    child_1->setPosition(child_1_offset);
    fw::RectangleWidget* child_2 = widgets.createRectangleWidget(10.0f, 10.0f);
    child_2->setName("green rectangle");
    child_2->setFillColor(sf::Color::Green);
    child_2->setParent(widget);
    child_2->setPosition(child_2_offset);
    fw::RectangleWidget* child_3 = widgets.createRectangleWidget(10.0f, 10.0f);
    child_3->setName("blue rectangle");
    child_3->setFillColor(sf::Color::Blue);
    child_3->setParent(widget);
    child_3->setPosition(child_3_offset);
    application.advance();

    sf::Vector2f offset = sf::Vector2f(100.0f, 0.0f);
    sf::Vector2f pos2 = pos + offset;
    fw::RectangleWidget* copy = widgets.duplicateWidget(widget, false);
    copy->setPosition(pos2);
    application.advance();

    T_COMPARE(widgets.getAllWidgets().size(), 6);
    fw::Widget* root_widget = widgets.getRootWidget();
    const CompVector<fw::Widget*>& root_children = root_widget->getChildren();
    if (T_COMPARE(root_children.size(), 2)) {
        T_CHECK(root_children[0] == widget);
        T_CHECK(root_children[1] == copy);
    }
    const CompVector<fw::Widget*>& widget_children = widget->getChildren();
    if (T_COMPARE(widget_children.size(), 3)) {
        T_CHECK(widget_children[0] == child_1);
        T_CHECK(widget_children[1] == child_2);
        T_CHECK(widget_children[2] == child_3);
    }
    const CompVector<fw::Widget*>& copy_children = copy->getChildren();
    T_COMPARE(copy_children.size(), 0);

    T_VEC2_COMPARE(widget->getGlobalPosition(), pos);
    T_VEC2_COMPARE(copy->getGlobalPosition(), pos2);
}

void WidgetTestsBasic::widgetMouseEvents1(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::RectangleWidget* rectangle_widget = application.getWidgets().createRectangleWidget(size);
    bool mouse_entered = false;
    bool mouse_left_pressed = false;
    bool mouse_right_pressed = false;
    bool mouse_left_released_global = false;
    bool mouse_left_released_blockable = false;
    bool mouse_right_released_global = false;
    bool mouse_right_released_blockable = false;
    bool mouse_left_clicked = false;
    bool mouse_right_clicked = false;
    bool mouse_moved = false;
    bool mouse_scroll_x = false;
    bool mouse_scroll_y = false;
    bool mouse_exited = false;
    bool mouse_processed = false;
    rectangle_widget->OnMouseEnter += [&](const sf::Vector2f& pos) {
        mouse_entered = true;
    };
    rectangle_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
        mouse_left_pressed = true;
    };
    rectangle_widget->OnRightPress += [&](const sf::Vector2f& pos) {
        mouse_right_pressed = true;
    };
    rectangle_widget->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_left_released_global = true;
    };
    rectangle_widget->OnBlockableLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_left_released_blockable = true;
    };
    rectangle_widget->OnGlobalRightRelease += [&](const sf::Vector2f& pos) {
        mouse_right_released_global = true;
    };
    rectangle_widget->OnBlockableRightRelease += [&](const sf::Vector2f& pos) {
        mouse_right_released_blockable = true;
    };
    rectangle_widget->OnLeftClick += [&](const sf::Vector2f& pos) {
        mouse_left_clicked = true;
    };
    rectangle_widget->OnRightClick += [&](const sf::Vector2f& pos) {
        mouse_right_clicked = true;
    };
    rectangle_widget->OnMouseMoved += [&](const sf::Vector2f& pos) {
        mouse_moved = true;
    };
    rectangle_widget->OnScrollX += [&](const sf::Vector2f& pos, float delta) {
        mouse_scroll_x = true;
    };
    rectangle_widget->OnScrollY += [&](const sf::Vector2f& pos, float delta) {
        mouse_scroll_y = true;
    };
    rectangle_widget->OnMouseExit += [&](const sf::Vector2f& pos) {
        mouse_exited = true;
    };
    rectangle_widget->OnProcessMouse += [&](const sf::Vector2f& pos) {
        mouse_processed = true;
    };
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2i mouse_pos_1(150, 150);
    sf::Vector2i mouse_pos_2(300, 300);
    rectangle_widget->setPosition(position);
    T_CHECK(!rectangle_widget->isMouseOver());
    T_CHECK(!mouse_entered);
    T_CHECK(!mouse_left_pressed);
    T_CHECK(!mouse_left_released_global);
    T_CHECK(!mouse_left_released_blockable);
    T_CHECK(!mouse_right_pressed);
    T_CHECK(!mouse_right_released_global);
    T_CHECK(!mouse_right_released_blockable);
    T_CHECK(!mouse_left_clicked);
    T_CHECK(!mouse_right_clicked);
    T_CHECK(!mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(!mouse_processed);
    application.mouseMove(mouse_pos_1);
    application.advance();
    T_CHECK(rectangle_widget->isMouseOver());
    T_CHECK(mouse_entered);
    T_CHECK(!mouse_left_pressed);
    T_CHECK(!mouse_left_released_global);
    T_CHECK(!mouse_left_released_blockable);
    T_CHECK(!mouse_right_pressed);
    T_CHECK(!mouse_right_released_global);
    T_CHECK(!mouse_right_released_blockable);
    T_CHECK(!mouse_left_clicked);
    T_CHECK(!mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseLeftPress();
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(!mouse_left_released_global);
    T_CHECK(!mouse_left_released_blockable);
    T_CHECK(!mouse_right_pressed);
    T_CHECK(!mouse_right_released_global);
    T_CHECK(!mouse_right_released_blockable);
    T_CHECK(!mouse_left_clicked);
    T_CHECK(!mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(!mouse_right_pressed);
    T_CHECK(!mouse_right_released_global);
    T_CHECK(!mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(!mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseRightPress();
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(mouse_right_pressed);
    T_CHECK(!mouse_right_released_global);
    T_CHECK(!mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(!mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseRightRelease();
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(mouse_right_pressed);
    T_CHECK(mouse_right_released_global);
    T_CHECK(mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseScrollX(1.0f);
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(mouse_right_pressed);
    T_CHECK(mouse_right_released_global);
    T_CHECK(mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseScrollY(1.0f);
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(mouse_right_pressed);
    T_CHECK(mouse_right_released_global);
    T_CHECK(mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(mouse_scroll_x);
    T_CHECK(mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseMove(mouse_pos_2);
    application.advance();
    T_CHECK(!rectangle_widget->isMouseOver());
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(mouse_right_pressed);
    T_CHECK(mouse_right_released_global);
    T_CHECK(mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(mouse_scroll_x);
    T_CHECK(mouse_scroll_y);
    T_CHECK(mouse_exited);
    T_CHECK(mouse_processed);
}

void WidgetTestsBasic::widgetMouseEvents2(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::RectangleWidget* rectangle_widget_1 = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_widget_2 = application.getWidgets().createRectangleWidget(size);
    rectangle_widget_1->setClickThrough(false);
    bool mouse_entered_1 = false;
    bool mouse_pressed_1 = false;
    bool mouse_released_global_1 = false;
    bool mouse_released_blockable_1 = false;
    bool mouse_clicked_1 = false;
    bool mouse_exited_1 = false;
    bool mouse_processed_1 = false;
    bool mouse_entered_2 = false;
    bool mouse_pressed_2 = false;
    bool mouse_released_global_2 = false;
    bool mouse_released_blockable_2 = false;
    bool mouse_clicked_2 = false;
    bool mouse_exited_2 = false;
    bool mouse_processed_2 = false;
    rectangle_widget_1->OnMouseEnter += [&](const sf::Vector2f& pos) {
        mouse_entered_1 = true;
    };
    rectangle_widget_1->OnLeftPress += [&](const sf::Vector2f& pos) {
        mouse_pressed_1 = true;
    };
    rectangle_widget_1->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_released_global_1 = true;
    };
    rectangle_widget_1->OnBlockableLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_released_blockable_1 = true;
    };
    rectangle_widget_1->OnLeftClick += [&](const sf::Vector2f& pos) {
        mouse_clicked_1 = true;
    };
    rectangle_widget_1->OnMouseExit += [&](const sf::Vector2f& pos) {
        mouse_exited_1 = true;
    };
    rectangle_widget_1->OnProcessMouse += [&](const sf::Vector2f& pos) {
        mouse_processed_1 = true;
    };
    rectangle_widget_2->OnMouseEnter += [&](const sf::Vector2f& pos) {
        mouse_entered_2 = true;
    };
    rectangle_widget_2->OnLeftPress += [&](const sf::Vector2f& pos) {
        mouse_pressed_2 = true;
    };
    rectangle_widget_2->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_released_global_2 = true;
    };
    rectangle_widget_2->OnBlockableLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_released_blockable_2 = true;
    };
    rectangle_widget_2->OnLeftClick += [&](const sf::Vector2f& pos) {
        mouse_clicked_2 = true;
    };
    rectangle_widget_2->OnMouseExit += [&](const sf::Vector2f& pos) {
        mouse_exited_2 = true;
    };
    rectangle_widget_2->OnProcessMouse += [&](const sf::Vector2f& pos) {
        mouse_processed_2 = true;
    };
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2i mouse_pos_1(150, 150);
    sf::Vector2i mouse_pos_2(300, 300);
    rectangle_widget_1->setPosition(position);
    rectangle_widget_2->setPosition(position);
    T_CHECK(!rectangle_widget_1->isMouseOver());
    T_CHECK(!mouse_entered_1);
    T_CHECK(!mouse_pressed_1);
    T_CHECK(!mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(!mouse_processed_1);
    T_CHECK(!rectangle_widget_2->isMouseOver());
    T_CHECK(!mouse_entered_2);
    T_CHECK(!mouse_pressed_2);
    T_CHECK(!mouse_released_global_2);
    T_CHECK(!mouse_released_blockable_2);
    T_CHECK(!mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(!mouse_processed_2);
    application.mouseMove(mouse_pos_1);
    application.advance();
    T_CHECK(rectangle_widget_1->isMouseOver());
    T_CHECK(mouse_entered_1);
    T_CHECK(!mouse_pressed_1);
    T_CHECK(!mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(rectangle_widget_2->isMouseOver());
    T_CHECK(mouse_entered_2);
    T_CHECK(!mouse_pressed_2);
    T_CHECK(!mouse_released_global_2);
    T_CHECK(!mouse_released_blockable_2);
    T_CHECK(!mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(mouse_processed_2);
    rectangle_widget_2->setClickThrough(true);
    application.mouseLeftPress();
    application.advance();
    T_CHECK(mouse_entered_1);
    T_CHECK(mouse_pressed_1);
    T_CHECK(!mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(mouse_entered_2);
    T_CHECK(mouse_pressed_2);
    T_CHECK(!mouse_released_global_2);
    T_CHECK(!mouse_released_blockable_2);
    T_CHECK(!mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(mouse_processed_2);
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(mouse_entered_1);
    T_CHECK(mouse_pressed_1);
    T_CHECK(mouse_released_global_1);
    T_CHECK(mouse_released_blockable_1);
    T_CHECK(mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(mouse_entered_2);
    T_CHECK(mouse_pressed_2);
    T_CHECK(mouse_released_global_2);
    T_CHECK(mouse_released_blockable_2);
    T_CHECK(mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(mouse_processed_2);
    mouse_pressed_1 = false;
    mouse_pressed_2 = false;
    mouse_released_global_1 = false;
    mouse_released_blockable_1 = false;
    mouse_clicked_1 = false;
    mouse_released_global_2 = false;
    mouse_released_blockable_2 = false;
    mouse_clicked_2 = false;
    rectangle_widget_2->setClickThrough(false);
    application.mouseLeftPress();
    application.advance();
    T_CHECK(mouse_entered_1);
    T_CHECK(!mouse_pressed_1);
    T_CHECK(!mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(mouse_entered_2);
    T_CHECK(mouse_pressed_2);
    T_CHECK(!mouse_released_global_2);
    T_CHECK(!mouse_released_blockable_2);
    T_CHECK(!mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(mouse_processed_2);
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(mouse_entered_1);
    T_CHECK(!mouse_pressed_1);
    T_CHECK(mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(mouse_entered_2);
    T_CHECK(mouse_pressed_2);
    T_CHECK(mouse_released_global_2);
    T_CHECK(mouse_released_blockable_2);
    T_CHECK(mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(mouse_processed_2);
    application.mouseMove(mouse_pos_2);
    application.advance();
    T_CHECK(!rectangle_widget_1->isMouseOver());
    T_CHECK(mouse_entered_1);
    T_CHECK(!mouse_pressed_1);
    T_CHECK(mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(!rectangle_widget_2->isMouseOver());
    T_CHECK(mouse_entered_2);
    T_CHECK(mouse_pressed_2);
    T_CHECK(mouse_released_global_2);
    T_CHECK(mouse_released_blockable_2);
    T_CHECK(mouse_clicked_2);
    T_CHECK(mouse_exited_2);
    T_CHECK(mouse_processed_2);
}

void WidgetTestsBasic::dragGestureEventTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::RectangleWidget* rectangle_widget_1 = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_widget_2 = application.getWidgets().createRectangleWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2i mouse_pos_1(150, 150);
    sf::Vector2i mouse_pos_2(300, 300);
    rectangle_widget_1->setPosition(position);
    rectangle_widget_2->setPosition(position);
    rectangle_widget_1->setClickThrough(false);
    sf::Vector2f drag_pos;
    bool dragged_1 = false;
    bool dragged_2 = false;
    rectangle_widget_1->OnProcessDragGesture += [&](sf::Mouse::Button button, const sf::Vector2f& pos) {
        dragged_1 = true;
        drag_pos = pos;
    };
    rectangle_widget_2->OnProcessDragGesture += [&](sf::Mouse::Button button, const sf::Vector2f& pos) {
        dragged_2 = true;
    };
    application.advance();
    T_CHECK(!dragged_1);
    T_CHECK(!dragged_2);

    application.mouseMove(mouse_pos_1);
    application.advance();
    T_CHECK(!dragged_1);
    T_CHECK(!dragged_2);
    application.mouseLeftPress();
    application.advance();
    T_CHECK(dragged_1);
    T_CHECK(!dragged_2);
    T_VEC2_APPROX_COMPARE(drag_pos, fw::to2f(mouse_pos_1));
    application.mouseMove(mouse_pos_2);
    application.advance();
    T_CHECK(dragged_1);
    T_CHECK(!dragged_2);
    T_VEC2_APPROX_COMPARE(drag_pos, fw::to2f(mouse_pos_2));
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(dragged_1);
    T_CHECK(!dragged_2);
    T_VEC2_APPROX_COMPARE(drag_pos, fw::to2f( mouse_pos_2));
}

void WidgetTestsBasic::eventsTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    bool pre_update = false;
    bool post_update = false;
    bool before_global_render = false;
    bool before_render = false;
    bool after_render = false;
    bool after_global_render = false;
    unsigned int window_width = 0;
    unsigned int window_height = 0;
    fw::RectangleWidget* widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    widget->OnPreUpdate += [&]() {
        pre_update = true;
    };
    widget->OnPostUpdate += [&]() {
        post_update = true;
    };
    widget->OnBeforeGlobalRender += [&](sf::RenderTarget& target) {
        before_global_render = true;
    };
    widget->OnBeforeRender += [&](sf::RenderTarget& target) {
        before_render = true;
    };
    widget->OnAfterRender += [&](sf::RenderTarget& target) {
        after_render = true;
    };
    widget->OnAfterGlobalRender += [&](sf::RenderTarget& target) {
        after_global_render = true;
    };
    widget->OnWindowResized += [&](unsigned int width, unsigned int height) {
        window_width = width;
        window_height = height;
    };
    application.advance();
    T_CHECK(pre_update);
    T_CHECK(post_update);
    T_CHECK(before_global_render);
    T_CHECK(after_global_render);
    T_COMPARE(window_width, 0);
    T_COMPARE(window_height, 0);
    application.setWindowSize(640, 480);
    application.advance();
    T_COMPARE(window_width, 640);
    T_COMPARE(window_height, 480);
}

void WidgetTestsBasic::coordinatesTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    sf::Vector2f parent_pos(100.0f, 100.0f);
    sf::Vector2f child_local_pos(30.0f, 30.0f);
    child_widget->setParent(parent_widget);
    parent_widget->setPosition(parent_pos);
    child_widget->setPosition(child_local_pos);
    T_VEC2_APPROX_COMPARE(child_widget->toGlobal(sf::Vector2f(10.0f, 5.0f)), sf::Vector2f(140.0f, 135.0f));
    T_VEC2_APPROX_COMPARE(child_widget->toLocal(sf::Vector2f(140.0f, 135.0f)), sf::Vector2f(10.0f, 5.0f));

}

void WidgetTestsBasic::findTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    parent_widget->setName("parent");
    child_widget->setName("child");
    child_widget->setParent(parent_widget);
    T_CHECK(application.getWidgets().find("root") == application.getWidgets().getRootWidget());
    T_CHECK(application.getWidgets().find("parent") == parent_widget);
    T_CHECK(application.getWidgets().find("child") == child_widget);
}

void WidgetTestsBasic::anchorTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f parent_size(100.0f, 100.0f);
    sf::Vector2f child_size(30.0f, 30.0f);
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(parent_size);
    fw::RectangleWidget* child_widget = application.getWidgets().createRectangleWidget(child_size);
    child_widget->setParent(parent_widget);
    sf::Vector2f anchor_offset(5.0f, 7.0f);
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

void WidgetTestsBasic::remove1Test(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child0_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child1_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    parent_widget->setName("parent");
    child0_widget->setName("child0");
    child0_widget->setParent(parent_widget);
    child1_widget->setName("child1");
    child1_widget->setParent(parent_widget);
    if (T_COMPARE(application.getWidgets().getSize(), 4)) {
        if (T_COMPARE(root_widget->getChildrenCount(), 1)) {
            T_CHECK(root_widget->getChild(0) == parent_widget);
            if (T_COMPARE(parent_widget->getChildrenCount(), 2)) {
                T_CHECK(parent_widget->getChild(0) == child0_widget);
                T_CHECK(parent_widget->getChild(1) == child1_widget);
                T_COMPARE(child0_widget->getChildrenCount(), 0);
                T_COMPARE(child1_widget->getChildrenCount(), 0);
            }
        }
    }

    application.advance();

    T_ASSERT_NO_ERRORS();
    child0_widget->remove();
    if (T_COMPARE(application.getWidgets().getSize(), 3)) {
        if (T_COMPARE(root_widget->getChildrenCount(), 1)) {
            T_CHECK(root_widget->getChild(0) == parent_widget);
            if (T_COMPARE(parent_widget->getChildrenCount(), 1)) {
                T_CHECK(parent_widget->getChild(0) == child1_widget);
                T_COMPARE(child1_widget->getChildrenCount(), 0);
            }
        }
    }

    application.advance();

    T_ASSERT_NO_ERRORS();
    parent_widget->remove();
    if (T_COMPARE(application.getWidgets().getSize(), 1)) {
        T_COMPARE(root_widget->getChildrenCount(), 0);
    }

    application.advance();
}

void WidgetTestsBasic::remove2Test(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* subparent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child0_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child1_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    parent_widget->setName("parent");
    subparent_widget->setName("subparent");
    subparent_widget->setParent(parent_widget);
    child0_widget->setName("child0");
    child0_widget->setParent(subparent_widget);
    child1_widget->setName("child1");
    child1_widget->setParent(subparent_widget);
    if (T_COMPARE(application.getWidgets().getSize(), 5)) {
        if (T_COMPARE(root_widget->getChildrenCount(), 1)) {
            T_CHECK(root_widget->getChild(0) == parent_widget);
            if (T_COMPARE(parent_widget->getChildrenCount(), 1)) {
                T_CHECK(parent_widget->getChild(0) == subparent_widget);
                if (T_COMPARE(subparent_widget->getChildrenCount(), 2)) {
                    T_CHECK(subparent_widget->getChild(0) == child0_widget);
                    T_CHECK(subparent_widget->getChild(1) == child1_widget);
                    T_COMPARE(child0_widget->getChildrenCount(), 0);
                    T_COMPARE(child1_widget->getChildrenCount(), 0);
                }
            }
        }
    }

    application.advance();

    T_ASSERT_NO_ERRORS();
    subparent_widget->remove(false);
    if (T_COMPARE(application.getWidgets().getSize(), 4)) {
        if (T_COMPARE(root_widget->getChildrenCount(), 1)) {
            T_CHECK(root_widget->getChild(0) == parent_widget);
            if (T_COMPARE(parent_widget->getChildrenCount(), 2)) {
                T_CHECK(parent_widget->getChild(0) == child0_widget);
                T_CHECK(parent_widget->getChild(1) == child1_widget);
                T_COMPARE(child0_widget->getChildrenCount(), 0);
                T_COMPARE(child1_widget->getChildrenCount(), 0);
            }
        }
    }

    application.advance();
}
