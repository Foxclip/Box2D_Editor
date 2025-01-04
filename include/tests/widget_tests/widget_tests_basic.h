#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"

class WidgetTestsBasic : public WidgetTest {
public:
	WidgetTestsBasic(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void rootWidgetTest(test::Test& test);
	void emptyWidgetTest(test::Test& test);
	void rectangleWidgetTest(test::Test& test);
	void polygonWidgetBasicTest(test::Test& test);
	void setParentTest(test::Test& test);
	void duplicateRectangleTest(test::Test& test);
	void duplicatePolygonTest(test::Test& test);
	void duplicateChildrenTest(test::Test& test);
	void duplicateWithoutChildrenTest(test::Test& test);
	void widgetMouseEvents1(test::Test& test);
	void widgetMouseEvents2(test::Test& test);
	void dragGestureEventTest(test::Test& test);
	void eventsTest(test::Test& test);
	void coordinatesTest(test::Test& test);
	void findTest(test::Test& test);
	void anchorTest(test::Test& test);
	void remove1Test(test::Test& test);
	void remove2Test(test::Test& test);

};
