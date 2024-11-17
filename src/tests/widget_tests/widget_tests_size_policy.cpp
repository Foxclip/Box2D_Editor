#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_size_policy.h"

WidgetTestsSizePolicy::WidgetTestsSizePolicy(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : TestModule(name, parent, required_nodes) {
    test::Test* size_policy_basic_test = addTest("basic", [&](test::Test& test) { sizePolicyTest(test); });
    test::Test* size_policy_position_test = addTest("position", [&](test::Test& test) { sizePolicyPositionTest(test); });
    test::Test* size_policy_expand_test = addTest("expand", [&](test::Test& test) { sizePolicyExpandTest(test); });
    test::Test* size_policy_limits_test = addTest("limits", { size_policy_expand_test }, [&](test::Test& test) { sizePolicyLimitsTest(test); });
    std::vector<test::TestNode*> size_policy_tests = { size_policy_basic_test, size_policy_position_test, size_policy_expand_test, size_policy_limits_test };
    test::Test* size_policy_combined_test = addTest("combined", size_policy_tests, [&](test::Test& test) { sizePolicyCombinedTest(test); });
}

void WidgetTestsSizePolicy::sizePolicyTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f parent_size(150.0f, 150.0f);
    sf::Vector2f container_size(100.0f, 100.0f);
    sf::Vector2f child_size(30.0f, 30.0f);
    float container_padding = 10.0f;
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(parent_size);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::RectangleWidget* red_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* green_widget = application.getWidgets().createRectangleWidget(child_size);
    parent_widget->setFillColor(sf::Color(128, 128, 128));
    container_widget->setFillColor(sf::Color(100, 100, 100));
    red_widget->setFillColor(sf::Color::Red);
    green_widget->setFillColor(sf::Color::Green);
    container_widget->setPadding(container_padding);
    container_widget->setHorizontal(false);
    container_widget->setParent(parent_widget);
    red_widget->setParent(container_widget);
    green_widget->setParent(container_widget);
    container_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    red_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    application.advance();
    auto rect_to_str = &WidgetTests::floatRectToStr;
    float container_height = child_size.y * 2 + container_padding * 3;
    T_COMPARE(parent_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(container_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, container_height), rect_to_str);
    T_COMPARE(red_widget->getParentLocalBounds(), sf::FloatRect(container_padding, container_padding, parent_size.x - container_padding * 2, child_size.y), rect_to_str);
    T_COMPARE(green_widget->getParentLocalBounds(), sf::FloatRect(container_padding, container_padding * 2 + child_size.y, child_size.x, child_size.y), rect_to_str);
}

void WidgetTestsSizePolicy::sizePolicyPositionTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f parent_size(150.0f, 150.0f);
    sf::Vector2f child0_size(100.0f, 100.0f);
    sf::Vector2f child1_size(30.0f, 30.0f);
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(parent_size);
    fw::RectangleWidget* child0_widget = application.getWidgets().createRectangleWidget(child0_size);
    fw::RectangleWidget* child1_widget = application.getWidgets().createRectangleWidget(child1_size);
    parent_widget->setName("parent");
    child0_widget->setName("child0");
    child1_widget->setName("child1");
    parent_widget->setFillColor(sf::Color(128, 128, 128));
    child0_widget->setFillColor(sf::Color::Red);
    child1_widget->setFillColor(sf::Color::Green);
    child0_widget->setParent(parent_widget);
    child1_widget->setParent(child0_widget);
    auto rect_to_str = &WidgetTests::floatRectToStr;

    child0_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    child1_widget->setParentAnchor(fw::Widget::Anchor::TOP_RIGHT);
    application.advance();
    T_COMPARE(parent_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(child0_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, child0_size.y), rect_to_str);
    T_COMPARE(child1_widget->getParentLocalBounds(), sf::FloatRect(parent_size.x, 0.0f, child1_size.x, child1_size.y), rect_to_str);

    child1_widget->setParentAnchor(fw::Widget::Anchor::CENTER);
    application.advance();
    T_COMPARE(parent_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(child0_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, child0_size.y), rect_to_str);
    T_COMPARE(child1_widget->getParentLocalBounds(), sf::FloatRect(parent_size.x / 2.0f, child0_size.y / 2.0f, child1_size.x, child1_size.y), rect_to_str);

    child0_widget->setSizeYPolicy(fw::Widget::SizePolicy::PARENT);
    application.advance();
    T_COMPARE(parent_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(child0_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(child1_widget->getParentLocalBounds(), sf::FloatRect(parent_size.x / 2.0f, parent_size.y / 2.0f, child1_size.x, child1_size.y), rect_to_str);
}

void WidgetTestsSizePolicy::sizePolicyExpandTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f container_size(200.0f, 150.0f);
    sf::Vector2f child_size(30.0f, 30.0f);
    float container_padding = 10.0f;
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::RectangleWidget* red_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* green_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* blue_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* yellow_widget = application.getWidgets().createRectangleWidget(child_size);
    container_widget->setName("container");
    container_widget->setPadding(container_padding);
    red_widget->setName("red");
    green_widget->setName("green");
    blue_widget->setName("blue");
    yellow_widget->setName("yellow");
    red_widget->setParent(container_widget);
    green_widget->setParent(container_widget);
    blue_widget->setParent(container_widget);
    yellow_widget->setParent(container_widget);
    container_widget->setFillColor(sf::Color(100, 100, 100));
    red_widget->setFillColor(sf::Color::Red);
    green_widget->setFillColor(sf::Color::Green);
    blue_widget->setFillColor(sf::Color::Blue);
    yellow_widget->setFillColor(sf::Color::Yellow);
    container_widget->setSizeXPolicy(fw::Widget::SizePolicy::NONE);
    green_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    yellow_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    auto rect_to_str = &WidgetTests::floatRectToStr;

    float red_x = container_padding;
    float green_x = container_padding * 2 + child_size.x;
    float fixed_widgets_size = child_size.x * 2;
    float padding_size = container_padding * (container_widget->getChildren().size() + 1);
    float free_space = container_size.x - fixed_widgets_size - padding_size;
    float green_width = free_space / 2;
    float blue_x = green_x + green_width + container_padding;
    float yellow_x = blue_x + child_size.x + container_padding;
    float yellow_width = green_width;
    application.advance();
    T_COMPARE(container_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, container_size.x, child_size.y + container_padding * 2), rect_to_str);
    T_COMPARE(red_widget->getParentLocalBounds(), sf::FloatRect(red_x, container_padding, child_size.x, child_size.y), rect_to_str);
    T_COMPARE(green_widget->getParentLocalBounds(), sf::FloatRect(green_x, container_padding, green_width, child_size.y), rect_to_str);
    T_COMPARE(blue_widget->getParentLocalBounds(), sf::FloatRect(blue_x, container_padding, child_size.x, child_size.y), rect_to_str);
    T_COMPARE(yellow_widget->getParentLocalBounds(), sf::FloatRect(yellow_x, container_padding, yellow_width, child_size.y), rect_to_str);
}

void WidgetTestsSizePolicy::sizePolicyLimitsTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f container_size(300.0f, 150.0f);
    sf::Vector2f child_size(30.0f, 30.0f);
    float container_padding = 10.0f;
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::RectangleWidget* red_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* green_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* blue_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* yellow_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* magenta_widget = application.getWidgets().createRectangleWidget(child_size);
    container_widget->setName("container");
    container_widget->setPadding(container_padding);
    container_widget->setFillColor(sf::Color(100, 100, 100));
    container_widget->setSizeXPolicy(fw::Widget::SizePolicy::NONE);
    red_widget->setName("red");
    red_widget->setParent(container_widget);
    red_widget->setFillColor(sf::Color::Red);
    green_widget->setName("green");
    green_widget->setParent(container_widget);
    green_widget->setFillColor(sf::Color::Green);
    green_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    green_widget->setMinSize(75.0f, 0.0f);
    blue_widget->setName("blue");
    blue_widget->setParent(container_widget);
    blue_widget->setFillColor(sf::Color::Blue);
    blue_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    blue_widget->setMaxSize(15.0f, -1.0f);
    yellow_widget->setName("yellow");
    yellow_widget->setParent(container_widget);
    yellow_widget->setFillColor(sf::Color::Yellow);
    yellow_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    yellow_widget->setMaxSize(500.0f, -1.0f);
    magenta_widget->setName("purple");
    magenta_widget->setParent(container_widget);
    magenta_widget->setFillColor(sf::Color::Magenta);
    magenta_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    auto rect_to_str = &WidgetTests::floatRectToStr;

    float red_x = container_padding;
    float green_x = container_padding * 2 + child_size.x;
    float padding_size = container_padding * (container_widget->getChildren().size() + 1);
    float free_space = container_size.x - child_size.x - green_widget->getMinSize().x - blue_widget->getMaxSize().x - padding_size;
    float exp_delta = free_space / 3;
    float green_width = green_widget->getMinSize().x + exp_delta;
    float blue_x = green_x + green_width + container_padding;
    float blue_width = blue_widget->getMaxSize().x;
    float yellow_x = blue_x + blue_width + container_padding;
    float yellow_width = exp_delta;
    float magenta_x = yellow_x + yellow_width + container_padding;
    float magenta_width = exp_delta;
    application.advance();
    T_COMPARE(container_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, container_size.x, child_size.y + container_padding * 2), rect_to_str);
    T_COMPARE(red_widget->getParentLocalBounds(), sf::FloatRect(red_x, container_padding, child_size.x, child_size.y), rect_to_str);
    T_COMPARE(green_widget->getParentLocalBounds(), sf::FloatRect(green_x, container_padding, green_width, child_size.y), rect_to_str);
    T_COMPARE(blue_widget->getParentLocalBounds(), sf::FloatRect(blue_x, container_padding, blue_width, child_size.y), rect_to_str);
    T_COMPARE(yellow_widget->getParentLocalBounds(), sf::FloatRect(yellow_x, container_padding, yellow_width, child_size.y), rect_to_str);
    T_COMPARE(magenta_widget->getParentLocalBounds(), sf::FloatRect(magenta_x, container_padding, magenta_width, child_size.y), rect_to_str);
}

void WidgetTestsSizePolicy::sizePolicyCombinedTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f parent_size(200.0f, 150.0f);
    sf::Vector2f container_size(100.0f, 50.0f);
    sf::Vector2f child_size(30.0f, 30.0f);
    float container_padding = 10.0f;
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(parent_size);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::ContainerWidget* inner_container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::RectangleWidget* red_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* green_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* blue_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* yellow_widget = application.getWidgets().createRectangleWidget(child_size);
    parent_widget->setName("parent");
    parent_widget->setFillColor(sf::Color(128, 128, 128));
    container_widget->setName("container");
    container_widget->setHorizontal(false);
    container_widget->setPadding(container_padding);
    container_widget->setFillColor(sf::Color(100, 100, 100));
    container_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    container_widget->setSizeYPolicy(fw::Widget::SizePolicy::PARENT);
    container_widget->setParent(parent_widget);
    inner_container_widget->setName("inner container");
    inner_container_widget->setPadding(container_padding);
    inner_container_widget->setFillColor(sf::Color(75, 75, 75));
    inner_container_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    inner_container_widget->setParent(container_widget);
    red_widget->setName("red");
    red_widget->setParent(inner_container_widget);
    red_widget->setFillColor(sf::Color::Red);
    green_widget->setName("green");
    green_widget->setParent(inner_container_widget);
    green_widget->setFillColor(sf::Color::Green);
    green_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    blue_widget->setName("blue");
    blue_widget->setParent(inner_container_widget);
    blue_widget->setFillColor(sf::Color::Blue);
    yellow_widget->setName("yellow");
    yellow_widget->setParent(container_widget);
    yellow_widget->setFillColor(sf::Color::Yellow);
    yellow_widget->setSizeYPolicy(fw::Widget::SizePolicy::EXPAND);
    auto rect_to_str = &WidgetTests::floatRectToStr;

    float red_x = container_padding;
    float green_x = container_padding * 2 + child_size.x;
    float fixed_widgets_size = child_size.x * 2;
    float inner_padding_size = container_padding * (inner_container_widget->getChildren().size() + 1);
    float inner_container_width = parent_size.x - container_padding * 2;
    float green_width = inner_container_width - fixed_widgets_size - inner_padding_size;
    float blue_x = green_x + green_width + container_padding;
    float yellow_y = child_size.y + container_padding * 4;
    float yellow_height = parent_size.y - child_size.y - container_padding * 5;
    application.advance();
    T_COMPARE(parent_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(container_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(inner_container_widget->getParentLocalBounds(), sf::FloatRect(
        container_padding, container_padding, parent_size.x - container_padding * 2, child_size.y + container_padding * 2), rect_to_str
    );
    T_COMPARE(red_widget->getParentLocalBounds(), sf::FloatRect(red_x, container_padding, child_size.x, child_size.y), rect_to_str);
    T_COMPARE(green_widget->getParentLocalBounds(), sf::FloatRect(green_x, container_padding, green_width, child_size.y), rect_to_str);
    T_COMPARE(blue_widget->getParentLocalBounds(), sf::FloatRect(blue_x, container_padding, child_size.x, child_size.y), rect_to_str);
    T_COMPARE(yellow_widget->getParentLocalBounds(), sf::FloatRect(container_padding, yellow_y, child_size.x, yellow_height), rect_to_str);
}

sf::RenderWindow& WidgetTestsSizePolicy::getWindow() {
    return dynamic_cast<WidgetTests*>(parent)->window;
}

fw::Font& WidgetTestsSizePolicy::getFont() {
    return dynamic_cast<WidgetTests*>(parent)->textbox_font;
}
