#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsCanvas : public WidgetTest {
public:
	WidgetTestsCanvas(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void canvasWidgetBasicTest(test::Test& test);
	void canvasWidgetDrawTest(test::Test& test);
	void canvasWidgetAlphaTest(test::Test& test);

};
