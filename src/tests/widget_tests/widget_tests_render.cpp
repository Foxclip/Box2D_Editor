#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_render.h"
#include "widgets/widgets_common.h"

WidgetTestsRender::WidgetTestsRender(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : WidgetTest(name, parent, required_nodes) {
    test::Test* empty_test = addTest("empty", [&](test::Test& test) { emptyTest(test); });
    test::Test* rectangle_test = addTest("rectangle", { empty_test }, [&](test::Test& test) { rectangleTest(test); });
    test::Test* visibility_test = addTest("visibility", { rectangle_test }, [&](test::Test& test) { visibilityTest(test); });
}

void WidgetTestsRender::emptyTest(test::Test& test) {
    sf::Vector2u size(3, 3);
    sf::RenderWindow& window = getWindow();
    fw::Application application(window);
    application.init(test.name, size.x, size.y, 0, false);
    application.start(true);
    application.mouseMove(size.x / 2, size.y / 2);
    application.advance();

    checkPixels(test, application, size, [&](test::Test&, const sf::Image& image, unsigned int x, unsigned int y) {
        T_ASSERT(T_COMPARE(image.getPixel(x, y), sf::Color::Black, &WidgetTests::colorToStr));
    });

    sf::Color bg_color = sf::Color::Red;
    application.setBackgroundColor(bg_color);
    application.advance();

    checkPixels(test, application, size, [&](test::Test&, const sf::Image& image, unsigned int x, unsigned int y) {
        T_ASSERT(T_COMPARE(image.getPixel(x, y), bg_color, &WidgetTests::colorToStr));
    });
}

void WidgetTestsRender::rectangleTest(test::Test& test) {
    sf::Vector2u size(4, 4);
    sf::RenderWindow& window = getWindow();
    fw::Application application(window);
    application.init(test.name, size.x, size.y, 0, false);
    application.start(true);
    application.mouseMove(size.x / 2, size.y / 2);
    application.advance();

    sf::Color rect_color = sf::Color::Red;
    sf::Color bg_color = sf::Color::Black;
    fw::WidgetList& widgets = application.getWidgets();
    fw::RectangleWidget* rectangle_widget = widgets.createRectangleWidget(2.0f, 2.0f);
    rectangle_widget->setPosition(sf::Vector2f(1.0f, 1.0f));
    rectangle_widget->setFillColor(rect_color);
    application.advance();

    checkPixels(test, application, size, [&](test::Test&, const sf::Image& image, unsigned int x, unsigned int y) {
        if (x == 0 || x == image.getSize().x - 1 || y == 0 || y == image.getSize().y - 1) {
            T_ASSERT(T_COMPARE(image.getPixel(x, y), bg_color, &WidgetTests::colorToStr));
        } else {
            T_ASSERT(T_COMPARE(image.getPixel(x, y), rect_color, &WidgetTests::colorToStr));
        }
    });
}

void WidgetTestsRender::visibilityTest(test::Test& test) {
    sf::Vector2u size(4, 4);
    sf::RenderWindow& window = getWindow();
    fw::Application application(window);
    application.init(test.name, size.x, size.y, 0, false);
    application.start(true);
    application.mouseMove(size.x / 2, size.y / 2);
    application.advance();

    sf::Color rect_color = sf::Color::Red;
    sf::Color bg_color = sf::Color::Black;
    fw::WidgetList& widgets = application.getWidgets();
    fw::RectangleWidget* rectangle_widget = widgets.createRectangleWidget(2.0f, 2.0f);
    rectangle_widget->setPosition(sf::Vector2f(1.0f, 1.0f));
    rectangle_widget->setFillColor(rect_color);
    application.advance();

    auto check_rect_visible = [&]() {
		T_CONTAINER("check_rect_visible");
        checkPixels(test, application, size, [&](test::Test&, const sf::Image& image, unsigned int x, unsigned int y) {
            if (x == 0 || x == image.getSize().x - 1 || y == 0 || y == image.getSize().y - 1) {
                T_ASSERT(T_COMPARE(image.getPixel(x, y), bg_color, &WidgetTests::colorToStr));
            } else {
                T_ASSERT(T_COMPARE(image.getPixel(x, y), rect_color, &WidgetTests::colorToStr));
            }
        });
    };
    auto check_rect_invisible = [&]() {
        T_CONTAINER("check_rect_invisible");
        checkPixels(test, application, size, [&](test::Test&, const sf::Image& image, unsigned int x, unsigned int y) {
            T_ASSERT(T_COMPARE(image.getPixel(x, y), bg_color, &WidgetTests::colorToStr));
        });
    };

    check_rect_visible();

    rectangle_widget->setVisible(false);
    application.advance();
    check_rect_invisible();

    rectangle_widget->setVisible(true);
    rectangle_widget->setRenderable(false);
    application.advance();
    check_rect_invisible();

    rectangle_widget->setVisible(true);
    rectangle_widget->setRenderable(true);
    application.advance();
    check_rect_visible();
}

void WidgetTestsRender::beforeRunModule() {
    debug_mouse_saved = fw::WidgetList::debug_mouse;
    fw::WidgetList::debug_mouse = false; // mouse cursor will be on rendered textures otherwise
}

void WidgetTestsRender::afterRunModule() {
	fw::WidgetList::debug_mouse = debug_mouse_saved;
}

void WidgetTestsRender::checkPixels(
    test::Test& test,
    fw::Application& application,
    const sf::Vector2u& size,
    const std::function<void(test::Test&, const sf::Image&, unsigned int x, unsigned int y)> func
) const {
    const sf::Image& image = application.getRenderedImage();
    T_ASSERT(T_VEC2_COMPARE(image.getSize(), size));
    for (unsigned int y = 0; y < image.getSize().y; y++) {
        for (unsigned int x = 0; x < image.getSize().x; x++) {
            T_CONTAINER("Pixel (" + std::to_string(x) + ", " + std::to_string(y) + ")");
            func(test, image, x, y);
        }
    }
}
