#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_dropdown.h"

WidgetTestsDropdown::WidgetTestsDropdown(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : TestModule(name, parent, required_nodes) {
    test::Test* dropdown_widget_basic_test = addTest("basic", [&](test::Test& test) { dropdownWidgetBasicTest(test); });
    test::Test* dropdown_widget_options_1_test = addTest("options_1", { dropdown_widget_basic_test }, [&](test::Test& test) { dropdownWidgetOptions1Test(test); });
    test::Test* dropdown_widget_options_2_test = addTest("options_2", { dropdown_widget_options_1_test }, [&](test::Test& test) { dropdownWidgetOptions2Test(test); });
}

void WidgetTestsDropdown::dropdownWidgetBasicTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    fw::DropdownWidget* dropdown_widget = application.getWidgets().createDropdownWidget();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(dropdown_widget));
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(40.0f, 20.0f);
    dropdown_widget->setPosition(position);
    dropdown_widget->setSize(size);

    GenericWidgetTest gwt(application, test);
    gwt.widget = dropdown_widget;
    gwt.total_widgets = 7;
    gwt.type = fw::Widget::WidgetType::Dropdown;
    gwt.name = "dropdown";
    gwt.fullname = "root|dropdown";
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
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::MODAL;
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
}

void WidgetTestsDropdown::dropdownWidgetOptions1Test(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    fw::DropdownWidget* dropdown_widget = application.getWidgets().createDropdownWidget();
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(40.0f, 20.0f);
    dropdown_widget->setPosition(position);
    dropdown_widget->setSize(size);
    dropdown_widget->setFont(getFont());
    dropdown_widget->setCharacterSize(15);
    sf::Vector2f dropdown_center = dropdown_widget->getGlobalCenter();
    fw::RectangleWidget* panel_widget = dropdown_widget->getPanelWidget();
    application.advance();

    dropdown_widget->addOption("option1");
    dropdown_widget->addOption("option2");
    dropdown_widget->addOption("option3");
    T_COMPARE(dropdown_widget->getValue(), -1);
    T_CHECK(!panel_widget->isVisible());

    auto click_option = [&](size_t index, ptrdiff_t prev) {
        T_CHECK(!dropdown_widget->isFocused());
        CLICK_MOUSE(dropdown_center);
        T_CHECK(dropdown_widget->isFocused());
        T_COMPARE(dropdown_widget->getValue(), prev);
        T_CHECK(panel_widget->isVisible());
        CLICK_MOUSE(dropdown_widget->getOptionWidget(index)->getGlobalCenter());
        T_CHECK(!dropdown_widget->isFocused());
        T_COMPARE(dropdown_widget->getValue(), index);
        T_CHECK(!panel_widget->isVisible());
    };
    T_WRAP_CONTAINER(click_option(0, -1));
    T_WRAP_CONTAINER(click_option(1, 0));
    T_WRAP_CONTAINER(click_option(2, 1));

    T_ASSERT_NO_ERRORS();
    CLICK_MOUSE(dropdown_center);
    ptrdiff_t value = dropdown_widget->getValue();
    CLICK_MOUSE(dropdown_center);
    T_CHECK(!dropdown_widget->isFocused());
    T_COMPARE(dropdown_widget->getValue(), value);
    T_CHECK(!panel_widget->isVisible());
}

void WidgetTestsDropdown::dropdownWidgetOptions2Test(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    fw::DropdownWidget* dropdown_widget = application.getWidgets().createDropdownWidget();
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(40.0f, 20.0f);
    dropdown_widget->setPosition(position);
    dropdown_widget->setSize(size);
    dropdown_widget->setFont(getFont());
    dropdown_widget->setCharacterSize(15);
    sf::Vector2f dropdown_center = dropdown_widget->getGlobalCenter();
    fw::RectangleWidget* panel_widget = dropdown_widget->getPanelWidget();
    application.advance();

    auto click_option = [&](size_t index) {
        CLICK_MOUSE(dropdown_center);
        CLICK_MOUSE(dropdown_widget->getOptionWidget(index)->getGlobalCenter());
    };
    dropdown_widget->addOption("option1");
    dropdown_widget->addOption("option2");
    dropdown_widget->addOption("option1.5", 1);
    T_COMPARE(dropdown_widget->getOptionText(0), "option1");
    T_COMPARE(dropdown_widget->getOptionText(1), "option1.5");
    T_COMPARE(dropdown_widget->getOptionText(2), "option2");
    dropdown_widget->removeOption("option1.5");
    T_COMPARE(dropdown_widget->getOptionText(0), "option1");
    T_COMPARE(dropdown_widget->getOptionText(1), "option2");
    dropdown_widget->removeOption("option1");
    T_COMPARE(dropdown_widget->getOptionText(0), "option2");
    dropdown_widget->addOption("option1", 0);
    T_COMPARE(dropdown_widget->getOptionText(0), "option1");
    T_COMPARE(dropdown_widget->getOptionText(1), "option2");
    dropdown_widget->addOption("option1.5", 1);
    T_COMPARE(dropdown_widget->getOptionText(0), "option1");
    T_COMPARE(dropdown_widget->getOptionText(1), "option1.5");
    T_COMPARE(dropdown_widget->getOptionText(2), "option2");
    dropdown_widget->setOptionText(1, "option3");
    T_COMPARE(dropdown_widget->getOptionText(0), "option1");
    T_COMPARE(dropdown_widget->getOptionText(1), "option3");
    T_COMPARE(dropdown_widget->getOptionText(2), "option2");
}

sf::RenderWindow& WidgetTestsDropdown::getWindow() {
    return dynamic_cast<WidgetTests*>(parent)->window;
}

fw::Font& WidgetTestsDropdown::getFont() {
    return dynamic_cast<WidgetTests*>(parent)->textbox_font;
}
