#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsScrollArea: public WidgetTest {
public:
	WidgetTestsScrollArea(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void scrollAreaWidgetBasicTest(test::Test& test);
	void scrollAreaWidgetScrollTest(test::Test& test);
	void scrollAreaWidgetScrollbarVisibilityTest(test::Test& test);
	void scrollAreaWidgetScrollbarContainerTest(test::Test& test);

};
