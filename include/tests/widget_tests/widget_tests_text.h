#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsText : public WidgetTest {
public:
	WidgetTestsText(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void textWidgetTest(test::Test& test);
	void textWidgetDefaultFontTest(test::Test& test);

};
