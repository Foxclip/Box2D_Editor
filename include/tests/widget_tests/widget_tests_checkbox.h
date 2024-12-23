#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsCheckbox : public WidgetTest {
public:
	WidgetTestsCheckbox(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void checkboxWidgetBasicTest(test::Test& test);
	void checkboxWidgetToggleTest(test::Test& test);

};
