#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsRender : public WidgetTest {
public:
	WidgetTestsRender(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void emptyTest(test::Test& test);
	void rectangleTest(test::Test& test);

protected:
	void beforeRunModule() override;
	void afterRunModule() override;

private:
	bool debug_mouse_saved = false;

};
