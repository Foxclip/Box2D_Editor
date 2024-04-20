#include "tests/widget_tests.h"

WidgetTests::WidgetTests(test::TestManager& manager) : TestModule("Widgets", manager) {

}

void WidgetTests::createTestLists() {
	createApplicationList();
}

void WidgetTests::createApplicationList() {
	test::TestList* list = createTestList("Application");

    test::Test* basic_test = list->addTest(
        "basic",
        [&](test::Test& test) {
            fw::Application application;
        }
    );
}
