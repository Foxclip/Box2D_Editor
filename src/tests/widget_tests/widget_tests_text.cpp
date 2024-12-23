#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_text.h"

WidgetTestsText::WidgetTestsText(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : WidgetTest(name, parent, required_nodes) {
    test::Test* text_widget_basic_test = addTest("basic", [&](test::Test& test) { textWidgetTest(test); });
    test::Test* text_widget_default_font_test = addTest("default_font", { text_widget_basic_test }, [&](test::Test& test) { textWidgetDefaultFontTest(test); });
}

void WidgetTestsText::textWidgetTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::TextWidget* text_widget = application.getWidgets().createTextWidget();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(text_widget));
    text_widget->setCharacterSize(20);
    text_widget->setFont(getFont());
    text_widget->setString("Text");
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(41.0f, 20.0f);
    sf::Vector2f local_bounds_offset(0.0f, 6.0f);
    text_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = text_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Text;
    gwt.name = "text";
    gwt.fullname = "root|text";
    gwt.is_visual_position_quantized = true;
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
    gwt.visual_local_bounds = sf::FloatRect(local_bounds_offset, size - local_bounds_offset);
    gwt.visual_global_bounds = sf::FloatRect(position + local_bounds_offset, size - local_bounds_offset);
    gwt.visual_parent_local_bounds = gwt.visual_global_bounds;
    T_WRAP_CONTAINER(WidgetTests::genericWidgetTest(gwt));

    T_COMPARE(text_widget->getChildren().size(), 0);

    T_COMPARE(text_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
}

void WidgetTestsText::textWidgetDefaultFontTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    fw::TextWidget* text_widget = application.getWidgets().createTextWidget();
    text_widget->setCharacterSize(20);
    text_widget->setString("Text");
    sf::Vector2f position(100.0f, 100.0f);
    text_widget->setPosition(position);
    application.advance();
}
