#pragma once

#include "test.h"

class EventTests : public test::TestModule {
public:
	EventTests(test::TestManager& manager, const std::vector<TestModule*>& required_modules = { });
	void createTestLists() override;

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
	void clearTest(test::Test& test);

};
