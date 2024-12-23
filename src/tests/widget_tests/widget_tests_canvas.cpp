#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_canvas.h"

WidgetTestsCanvas::WidgetTestsCanvas(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : WidgetTest(name, parent, required_nodes) {
    test::Test* canvas_widget_basic_test = addTest("basic", [&](test::Test& test) { canvasWidgetBasicTest(test); });
    test::Test* canvas_widget_draw_test = addTest("draw", { canvas_widget_basic_test }, [&](test::Test& test) { canvasWidgetDrawTest(test); });
    test::Test* canvas_widget_alpha_test = addTest("alpha", { canvas_widget_basic_test }, [&](test::Test& test) { canvasWidgetAlphaTest(test); });
}

void WidgetTestsCanvas::canvasWidgetBasicTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::CanvasWidget* canvas_widget =
        application.getWidgets().createCanvasWidget(size, sf::Vector2u(100, 100));
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(canvas_widget));
    sf::Vector2f position(100.0f, 100.0f);
    canvas_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = canvas_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Canvas;
    gwt.name = "canvas";
    gwt.fullname = "root|canvas";
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

    T_COMPARE(canvas_widget->getChildren().size(), 0);
}

void WidgetTestsCanvas::canvasWidgetDrawTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::CanvasWidget* canvas_widget = application.getWidgets().createCanvasWidget(100.0f, 100.0f, 100, 100);
    auto color_to_str = &WidgetTests::colorToStr;
    {
        canvas_widget->clear();
        sf::Image image = canvas_widget->getRenderTexture().getTexture().copyToImage();
        T_ASSERT(T_COMPARE(image.getPixel(0, 0), sf::Color::Black, color_to_str));
    }
    {
        canvas_widget->clear(sf::Color(128, 128, 128));
        sf::Image image = canvas_widget->getRenderTexture().getTexture().copyToImage();
        T_ASSERT(T_COMPARE(image.getPixel(0, 0), sf::Color(128, 128, 128), color_to_str));
    }
    {
        canvas_widget->clear(sf::Color::Red);
        sf::RectangleShape rect(sf::Vector2f(30.0f, 30.0f));
        rect.setPosition(10.0f, 10.0f);
        rect.setFillColor(sf::Color::Green);
        canvas_widget->draw(rect);
        canvas_widget->display();
        sf::Image image = canvas_widget->getRenderTexture().getTexture().copyToImage();
        T_COMPARE(image.getPixel(0, 0), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(5, 5), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(10, 10), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(15, 15), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(20, 20), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(25, 25), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(30, 30), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(35, 35), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(40, 40), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(45, 45), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(50, 50), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(55, 55), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(60, 60), sf::Color::Red, color_to_str);
    }
}

void WidgetTestsCanvas::canvasWidgetAlphaTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    fw::CanvasWidget* canvas_widget = application.getWidgets().createCanvasWidget(100.0f, 100.0f, 100, 100);
    auto color_to_str = &WidgetTests::colorToStr;
    {
        sf::RectangleShape rect(sf::Vector2f(100.0f, 100.0f));
        canvas_widget->clear();
        rect.setFillColor(sf::Color(255, 0, 0, 128));
        canvas_widget->draw(rect);
        rect.setFillColor(sf::Color(0, 255, 0, 128));
        canvas_widget->draw(rect);
        rect.setFillColor(sf::Color(0, 0, 255, 128));
        canvas_widget->draw(rect);
        sf::Image image = canvas_widget->getRenderTexture().getTexture().copyToImage();
        T_ASSERT(T_COMPARE(image.getPixel(0, 0), sf::Color(32, 64, 128), color_to_str));
    }
}
