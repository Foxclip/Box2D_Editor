#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_render.h"
#include "widgets/widgets_common.h"

WidgetTestsRender::WidgetTestsRender(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : WidgetTest(name, parent, required_nodes) {
    test::Test* empty_test = addTest("empty", [&](test::Test& test) { emptyTest(test); });
}

void WidgetTestsRender::emptyTest(test::Test& test) {
    sf::Vector2u size(3, 3);

    sf::RenderWindow& window = getWindow();
    fw::Application application(window);
    application.init(test.name, size.x, size.y, 0, false);
    application.start(true);
    application.mouseMove(size.x / 2, size.y / 2);
    application.advance();

    const sf::Image& image = application.getRenderedImage();
    T_ASSERT(T_VEC2_COMPARE(image.getSize(), size));
    for (unsigned int y = 0; y < image.getSize().y; y++) {
		for (unsigned int x = 0; x < image.getSize().x; x++) {
            T_CONTAINER("Pixel (" + std::to_string(x) + ", " + std::to_string(y) + ")");
            T_ASSERT(T_COMPARE(image.getPixel(x, y), sf::Color::Black, &WidgetTests::colorToStr));
		}
    }
}

void WidgetTestsRender::beforeRunModule() {
    debug_mouse_saved = fw::WidgetList::debug_mouse;
    fw::WidgetList::debug_mouse = false; // mouse cursor will be on rendered textures otherwise
}

void WidgetTestsRender::afterRunModule() {
	fw::WidgetList::debug_mouse = debug_mouse_saved;
}
