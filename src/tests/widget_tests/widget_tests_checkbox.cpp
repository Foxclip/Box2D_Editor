#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_checkbox.h"

WidgetTestsCheckbox::WidgetTestsCheckbox(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : TestModule(name, parent, required_nodes) {
    test::Test* checkbox_widget_basic_test = addTest("basic", [&](test::Test& test) { checkboxWidgetBasicTest(test); });
    test::Test* checkbox_widget_toggle_test = addTest("toggle", { checkbox_widget_basic_test }, [&](test::Test& test) { checkboxWidgetToggleTest(test); });
}

void WidgetTestsCheckbox::checkboxWidgetBasicTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::CheckboxWidget* checkbox_widget = application.getWidgets().createCheckboxWidget();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(checkbox_widget));
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(20.0f, 20.0f);
    checkbox_widget->setPosition(position);
    checkbox_widget->setSize(size);

    GenericWidgetTest gwt(application, test);
    gwt.widget = checkbox_widget;
    gwt.total_widgets = 3;
    gwt.type = fw::Widget::WidgetType::Checkbox;
    gwt.name = "checkbox";
    gwt.fullname = "root|checkbox";
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

    if (T_COMPARE(checkbox_widget->getChildren().size(), 1)) {
        fw::RectangleWidget* check_widget = dynamic_cast<fw::RectangleWidget*>(checkbox_widget->getChild(0));
        T_CHECK(check_widget, "Check widget is not a RectangleWidget");
        T_COMPARE(check_widget->getParentAnchor(), fw::Widget::Anchor::CENTER, &WidgetTests::anchorToStr);
    }

    T_COMPARE(checkbox_widget->getFillColor(), sf::Color(50, 50, 50), &WidgetTests::colorToStr);
}

void WidgetTestsCheckbox::checkboxWidgetToggleTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    fw::CheckboxWidget* checkbox_widget = application.getWidgets().createCheckboxWidget();
    T_CHECK(!checkbox_widget->getValue());
    application.mouseMove(checkbox_widget->getGlobalCenter());
    application.advance();
    application.mouseLeftPress();
    application.advance();
    T_CHECK(checkbox_widget->getValue());
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(checkbox_widget->getValue());
}

sf::RenderWindow& WidgetTestsCheckbox::getWindow() {
    return dynamic_cast<WidgetTests*>(parent)->window;
}

fw::Font& WidgetTestsCheckbox::getFont() {
    return dynamic_cast<WidgetTests*>(parent)->textbox_font;
}
