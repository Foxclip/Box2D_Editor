#include "tests/event_tests.h"
#include "common/event.h"

EventTests::EventTests(
	const std::string& name, test::TestModule* manager, const std::vector<TestNode*>& required_nodes
) : TestModule(name, manager, required_nodes) {
	test::TestModule* event_list = addModule("Event");
	test::Test* basic_test = event_list->addTest("basic", [&](test::Test& test) { basicTest(test); });
	test::Test* invoke_test = event_list->addTest("invoke", { basic_test }, [&](test::Test& test) { invokeTest(test); });
	test::Test* invoke_arg1_test = event_list->addTest("invoke_arg1", { invoke_test }, [&](test::Test& test) { invokeArg1Test(test); });
	test::Test* invoke_arg2_test = event_list->addTest("invoke_arg2", { invoke_arg1_test }, [&](test::Test& test) { invokeArg2Test(test); });
	test::Test* handler_test = event_list->addTest("handler", { invoke_arg2_test }, [&](test::Test& test) { handlerTest(test); });
	test::Test* chain_test = event_list->addTest("chain", { invoke_arg2_test }, [&](test::Test& test) { chainTest(test); });
	test::Test* unsubscribe_func_test = event_list->addTest("unsubscribe_func", { invoke_test }, [&](test::Test& test) { unsubscribeFuncTest(test); });
	test::Test* unsubscribe_event_test = event_list->addTest("unsubscribe_event", { invoke_test }, [&](test::Test& test) { unsubscribeEventTest(test); });
	test::Test* clear_test = event_list->addTest("clear", { handler_test }, [&](test::Test& test) { clearTest(test); });
}

void EventTests::basicTest(test::Test& test) {
	Event<> event1;
	Event<int> event2;
	Event<int, float> event3;
}

void EventTests::invokeTest(test::Test& test) {
	bool invoked = false;
	Event<> event;
	event += [&]() {
		invoked = true;
	};
	event();
	T_CHECK(invoked);
}

void EventTests::invokeArg1Test(test::Test& test) {
	int arg = -1;
	Event<int> event;
	event += [&](int p_arg) {
		arg = p_arg;
	};
	event(5);
	T_COMPARE(arg, 5);
}

void EventTests::invokeArg2Test(test::Test& test) {
	int arg1 = -1;
	std::string arg2 = "";
	Event<int, const std::string&> event;
	event += [&](int p_arg1, const std::string& p_arg2) {
		arg1 = p_arg1;
		arg2 = p_arg2;
	};
	int val1 = 5;
	std::string val2 = "abc";
	event(val1, val2);
	T_COMPARE(arg1, 5);
	T_COMPARE(arg2, "abc");
}

void EventTests::handlerTest(test::Test& test) {
	int arg1 = -1;
	std::string arg2 = "";
	Event<int, const std::string&> event;
	EventHandlerFunc<int, const std::string&> func([&](int p_arg1, const std::string& p_arg2) {
		arg1 = p_arg1;
		arg2 = p_arg2;
	});
	event += func;
	int val1 = 5;
	std::string val2 = "abc";
	event(val1, val2);
	T_COMPARE(arg1, 5);
	T_COMPARE(arg2, "abc");
}

void EventTests::multiTest(test::Test& test) {
	bool invoke1 = false;
	bool invoke2 = false;
	Event<> event;
	event += [&]() {
		invoke1 = true;
	};
	event += [&]() {
		invoke2 = true;
	};
	event();
	T_CHECK(invoke1);
	T_CHECK(invoke2);
}

void EventTests::chainTest(test::Test& test) {
	int arg1 = -1;
	std::string arg2 = "";
	Event<int, const std::string&> event1;
	Event<int, const std::string&> event2;
	event1 += event2;
	event2 += [&](int p_arg1, const std::string& p_arg2) {
		arg1 = p_arg1;
		arg2 = p_arg2;
	};
	int val1 = 5;
	std::string val2 = "abc";
	event1(val1, val2);
	T_COMPARE(arg1, 5);
	T_COMPARE(arg2, "abc");
}

void EventTests::unsubscribeFuncTest(test::Test& test) {
	size_t invoke_count = 0;
	Event<> event;
	EventHandlerFunc<> func([&]() {
		invoke_count++;
	});
	event += func;
	event();
	T_COMPARE(invoke_count, 1);
	event -= func;
	event();
	T_COMPARE(invoke_count, 1);
}

void EventTests::unsubscribeEventTest(test::Test& test) {
	size_t invoke_count = 0;
	Event<> event1;
	Event<> event2;
	event1 += event2;
	event2 += [&]() {
		invoke_count++;
	};
	event1();
	T_COMPARE(invoke_count, 1);
	event1 -= event2;
	event1();
	T_COMPARE(invoke_count, 1);
}

void EventTests::clearTest(test::Test& test) {
	size_t invoke_count = 0;
	Event<> event;
	auto func = [&]() { invoke_count++; };
	EventHandlerFunc<> handler([&]() { invoke_count++; });
	Event<> event2;
	event2 += func;
	event += func;
	event += handler;
	event += event2;
	event();
	T_COMPARE(invoke_count, 3);
	event.clear();
	event();
	T_COMPARE(invoke_count, 3);
}
