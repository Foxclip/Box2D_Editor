#pragma once

#include "test_lib/test.h"

class EventTests : public test::TestModule {
public:
	EventTests(const std::string& name, test::TestModule* manager, const std::vector<TestNode*>& required_nodes = { });

private:
	void basicTest(test::Test& test);
	void invokeTest(test::Test& test);
	void invokeArg1Test(test::Test& test);
	void invokeArg2Test(test::Test& test);
	void handlerTest(test::Test& test);
	void multiTest(test::Test& test);
	void chainTest(test::Test& test);
	void unsubscribeFuncTest(test::Test& test);
	void unsubscribeEventTest(test::Test& test);
	void copySimpleTest(test::Test& test);
	void copyMultiTest(test::Test& test);
	void copyChainTest(test::Test& test);
	void clearTest(test::Test& test);

};
