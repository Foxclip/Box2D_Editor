#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsPostActions : public WidgetTest {
public:
	WidgetTestsPostActions(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void moveTest(test::Test& test);
	void deleteTest(test::Test& test);
	void setParentTest(test::Test& test);

};
