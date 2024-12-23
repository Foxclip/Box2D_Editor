#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsWindow: public WidgetTest {
public:
	WidgetTestsWindow(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void windowWidgetBasicTest(test::Test& test);
	void windowWidgetDragTest(test::Test& test);
	void windowWidgetChildrenTest(test::Test& test);
	void windowWidgetResizeTest(test::Test& test);
	void windowWidgetChainTest(test::Test& test);
	void windowWidgetDragLimitsTest(test::Test& test);
	void windowWidgetResizeLimitsTest(test::Test& test);
	void windowWidgetMoveToTopDragTest(test::Test& test);
	void windowWidgetMoveToTopResizeTest(test::Test& test);

private:
	void resizeWindowTest(fw::Application& application, test::Test& test, fw::WindowWidget* widget);

};
