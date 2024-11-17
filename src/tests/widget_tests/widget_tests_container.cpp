#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_container.h"

WidgetTestsContainer::WidgetTestsContainer(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : TestModule(name, parent, required_nodes) {
    test::Test* container_widget_basic_test = addTest("basic", [&](test::Test& test) { containerWidgetBasicTest(test); });
    test::Test* container_widget_children_test = addTest("children", { container_widget_basic_test }, [&](test::Test& test) { containerWidgetChildrenTest(test); });
    test::Test* container_widget_padding_test = addTest("padding", { container_widget_children_test }, [&](test::Test& test) { containerWidgetPaddingTest(test); });
}

void WidgetTestsContainer::containerWidgetBasicTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(size);
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(container_widget));
    sf::Vector2f position(100.0f, 100.0f);
    container_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = container_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Container;
    gwt.name = "container";
    gwt.fullname = "root|container";
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

    T_COMPARE(container_widget->getChildren().size(), 0);

    T_COMPARE(container_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
}

void WidgetTestsContainer::containerWidgetChildrenTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    sf::Vector2f container_size(100.0f, 100.0f);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    sf::Vector2f position(100.0f, 100.0f);
    float padding_x = 5.0f;
    float padding_y = 10.0f;
    container_widget->setPosition(position);
    container_widget->setPaddingX(padding_x);
    container_widget->setPaddingY(padding_y);
    T_VEC2_APPROX_COMPARE(container_widget->getSize(), container_size);
    application.advance();
    T_VEC2_APPROX_COMPARE(container_widget->getSize(), sf::Vector2f(padding_x, padding_y));
    sf::Vector2f child_1_size(35.0f, 28.0f);
    sf::Vector2f child_2_size(46.0f, 54.0f);
    fw::RectangleWidget* child_1_widget = application.getWidgets().createRectangleWidget(child_1_size);
    fw::RectangleWidget* child_2_widget = application.getWidgets().createRectangleWidget(child_2_size);
    child_1_widget->setParent(container_widget);
    child_2_widget->setParent(container_widget);
    application.advance();
    sf::Vector2f new_container_size = sf::Vector2f(
        padding_x + child_1_widget->getWidth() + padding_x + child_2_widget->getWidth() + padding_x,
        padding_y + std::max(child_1_widget->getHeight(), child_2_widget->getHeight()) + padding_y
    );
    T_VEC2_APPROX_COMPARE(container_widget->getSize(), new_container_size);
    T_VEC2_APPROX_COMPARE(child_1_widget->getPosition(), sf::Vector2f(padding_x, padding_y));
    T_VEC2_APPROX_COMPARE(child_2_widget->getPosition(), sf::Vector2f(padding_x + child_1_widget->getWidth() + padding_x, padding_y));
}

void WidgetTestsContainer::containerWidgetPaddingTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    sf::Vector2f container_size(100.0f, 100.0f);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    sf::Vector2f position(100.0f, 100.0f);
    float inner_padding_x = 5.0f;
    float inner_padding_y = 11.0f;
    float top_padding = 1.0f;
    float bottom_padding = 7.0f;
    float left_padding = 2.0f;
    float right_padding = 3.0f;
    container_widget->setPosition(position);
    container_widget->setInnerPaddingX(inner_padding_x);
    container_widget->setInnerPaddingY(inner_padding_y);
    container_widget->setTopPadding(top_padding);
    container_widget->setBottomPadding(bottom_padding);
    container_widget->setLeftPadding(left_padding);
    container_widget->setRightPadding(right_padding);
    T_VEC2_APPROX_COMPARE(container_widget->getSize(), container_size);
    application.advance();
    T_VEC2_APPROX_COMPARE(container_widget->getSize(), sf::Vector2f(right_padding, bottom_padding));
    sf::Vector2f child_1_size(35.0f, 28.0f);
    sf::Vector2f child_2_size(46.0f, 54.0f);
    sf::Vector2f child_3_size(27.0f, 14.0f);
    fw::RectangleWidget* child_1_widget = application.getWidgets().createRectangleWidget(child_1_size);
    fw::RectangleWidget* child_2_widget = application.getWidgets().createRectangleWidget(child_2_size);
    fw::RectangleWidget* child_3_widget = application.getWidgets().createRectangleWidget(child_3_size);
    child_1_widget->setParent(container_widget);
    child_2_widget->setParent(container_widget);
    child_3_widget->setParent(container_widget);
    {
        // horizontal
        application.advance();
        sf::Vector2f new_container_size = sf::Vector2f(
            left_padding + child_1_widget->getWidth() + inner_padding_x + child_2_widget->getWidth() + inner_padding_x + child_3_widget->getWidth() + right_padding,
            top_padding + std::max(child_1_widget->getHeight(), child_2_widget->getHeight()) + bottom_padding
        );
        float child_1_x = left_padding;
        float child_2_x = child_1_x + child_1_widget->getWidth() + inner_padding_x;
        float child_3_x = child_2_x + child_2_widget->getWidth() + inner_padding_x;
        T_VEC2_APPROX_COMPARE(container_widget->getSize(), new_container_size);
        T_VEC2_APPROX_COMPARE(child_1_widget->getPosition(), sf::Vector2f(child_1_x, top_padding));
        T_VEC2_APPROX_COMPARE(child_2_widget->getPosition(), sf::Vector2f(child_2_x, top_padding));
        T_VEC2_APPROX_COMPARE(child_3_widget->getPosition(), sf::Vector2f(child_3_x, top_padding));
    }
    {
        // vertical
        container_widget->setHorizontal(false);
        application.advance();
        sf::Vector2f new_container_size = sf::Vector2f(
            left_padding + std::max(child_1_widget->getWidth(), child_2_widget->getWidth()) + right_padding,
            top_padding + child_1_widget->getHeight() + inner_padding_y + child_2_widget->getHeight() + inner_padding_y + child_3_widget->getHeight() + bottom_padding
        );
        float child_1_y = top_padding;
        float child_2_y = child_1_y + child_1_widget->getHeight() + inner_padding_y;
        float child_3_y = child_2_y + child_2_widget->getHeight() + inner_padding_y;
        T_VEC2_APPROX_COMPARE(container_widget->getSize(), new_container_size);
        T_VEC2_APPROX_COMPARE(child_1_widget->getPosition(), sf::Vector2f(left_padding, child_1_y));
        T_VEC2_APPROX_COMPARE(child_2_widget->getPosition(), sf::Vector2f(left_padding, child_2_y));
        T_VEC2_APPROX_COMPARE(child_3_widget->getPosition(), sf::Vector2f(left_padding, child_3_y));
    }
}

sf::RenderWindow& WidgetTestsContainer::getWindow() {
    return dynamic_cast<WidgetTests*>(parent)->window;
}

fw::Font& WidgetTestsContainer::getFont() {
    return dynamic_cast<WidgetTests*>(parent)->textbox_font;
}
