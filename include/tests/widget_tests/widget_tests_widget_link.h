#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsWidgetLink : public WidgetTest {
public:
	WidgetTestsWidgetLink(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void widgetLinkBasicTest(test::Test& test);
	void widgetLinkTargetsFuncTest(test::Test& test);
	void widgetLinkContainerTest(test::Test& test);
	void widgetLinkRemoveTest(test::Test& test);

};
