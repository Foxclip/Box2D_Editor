#include "tests/widget_tests.h"
#include <utils.h>

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
    test::Test* init_test = list->addTest(
        "init",
        [&](test::Test& test) {
            TestApplication application;
            application.init("Test window", 800, 600, 0);
            tCompare(application.getWindowSize(), sf::Vector2u(800, 600), &WidgetTests::sfVec2uToStr);
        }
    );
}

std::string WidgetTests::sfVec2uToStr(const sf::Vector2u& vec) {
    return "(" + utils::vec_to_str(vec) + ")";
}

void TestApplication::onInit() {
    initialized = true;
}
