#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsRender : public WidgetTest {
public:
	WidgetTestsRender(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void emptyTest(test::Test& test);
	void rectangleTest(test::Test& test);
	void visibilityTest(test::Test& test);
	void localLayersTest(test::Test& test);
	void globalLayersTest(test::Test& test);

protected:
	void beforeRunModule() override;
	void afterRunModule() override;

private:
	bool debug_mouse_saved = false;

	void checkPixels(
		test::Test& test,
		fw::Application& application,
		const sf::Vector2u& size,
		const std::function<void(test::Test&, const sf::Image&, unsigned int x, unsigned int y)> func
	) const;

};
