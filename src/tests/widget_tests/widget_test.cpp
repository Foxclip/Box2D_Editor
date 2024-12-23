#include "tests/widget_tests/widget_test.h"
#include "tests/widget_tests/widget_tests.h"

WidgetTest::WidgetTest(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : TestModule(name, parent, required_nodes) { }

sf::RenderWindow& WidgetTest::getWindow() {
	return dynamic_cast<WidgetTests*>(parent)->window;
}

fw::Font& WidgetTest::getFont() {
	return dynamic_cast<WidgetTests*>(parent)->textbox_font;
}
