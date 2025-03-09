#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_button.h"

WidgetTestsButton::WidgetTestsButton(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : WidgetTest(name, parent, required_nodes) {
	test::Test* basic_test = addTest("basic", [&](test::Test& test) { basicTest(test); });
    test::Test* press_test = addTest("press", { basic_test }, [&](test::Test& test) { pressTest(test); });
}

void WidgetTestsButton::basicTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::ButtonWidget* button_widget = application.getWidgets().createButtonWidget(40.0f, 20.0f);
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(button_widget));
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(40.0f, 20.0f);
    button_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = button_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Button;
    gwt.name = "button";
    gwt.fullname = "root|button";
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
    gwt.is_click_through = false;
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

    T_COMPARE(button_widget->getChildren().size(), 0);

    T_COMPARE(button_widget->getFillColor(), fw::BUTTON_DEFAULT_NORMAL_COLOR, &WidgetTests::colorToStr);
}

void WidgetTestsButton::pressTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();

    sf::Vector2f size(200.0f, 200.0f);
    sf::Vector2f position(100.0f, 100.0f);
    fw::ButtonWidget* button_widget = application.getWidgets().createButtonWidget(size);
    button_widget->setPosition(position);
    bool pressed = false;
    bool released = false;
    button_widget->OnPress += [&]() {
        pressed = true;
    };
    button_widget->OnRelease += [&]() {
		released = true;
	};
    T_CHECK(!button_widget->isPressed());
    T_CHECK(!pressed);
    T_CHECK(!released);
    T_COMPARE(button_widget->getFillColor(), fw::BUTTON_DEFAULT_NORMAL_COLOR, &WidgetTests::colorToStr);
    application.advance();

    sf::Vector2f button_center = button_widget->getGlobalCenter();
    application.mouseMove(button_center);
    application.mouseLeftPress();
    application.advance();
    T_CHECK(button_widget->isPressed());
    T_CHECK(pressed);
    T_CHECK(!released);
    T_COMPARE(button_widget->getFillColor(), fw::BUTTON_DEFAULT_PRESSED_COLOR, &WidgetTests::colorToStr);

    application.mouseLeftRelease();
    application.advance();
    T_CHECK(!button_widget->isPressed());
    T_CHECK(pressed);
    T_CHECK(released);
    T_COMPARE(button_widget->getFillColor(), fw::BUTTON_DEFAULT_NORMAL_COLOR, &WidgetTests::colorToStr);
}
