#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_render.h"
#include "widgets/widgets_common.h"

WidgetTestsRender::WidgetTestsRender(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : WidgetTest(name, parent, required_nodes) {
    test::Test* empty_test = addTest("empty", [&](test::Test& test) { emptyTest(test); });
}

void WidgetTestsRender::emptyTest(test::Test& test) {
    sf::Vector2u size(10, 10);

    sf::RenderWindow& window = getWindow();
    fw::Application application(window);
    application.init(test.name, size.x, size.y, 0, false);
    application.start(true);
    application.mouseMove(5, 5);
    application.advance();

    sf::RenderTexture& render_texture = application.getWidgets().getRootWidget()->getRenderTexture();
    const sf::Texture& texture = render_texture.getTexture();
    sf::Image image = texture.copyToImage();
    T_ASSERT(T_VEC2_COMPARE(image.getSize(), size));
    for (unsigned int y = 0; y < image.getSize().y; y++) {
		for (unsigned int x = 0; x < image.getSize().x; x++) {
			T_COMPARE(image.getPixel(x, y), sf::Color::Red, &WidgetTests::colorToStr);
		}
    }
}
