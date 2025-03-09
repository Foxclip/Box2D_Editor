#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsButton : public WidgetTest {
public:
	WidgetTestsButton(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void basicTest(test::Test& test);

};
