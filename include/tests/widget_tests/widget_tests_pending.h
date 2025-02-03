#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsPending : public WidgetTest {
public:
	WidgetTestsPending(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void pendingMoveTest(test::Test& test);
	void pendingDeleteTest(test::Test& test);
	void pendingSetParentTest(test::Test& test);

};
