#include "common/logger.h"
#include "example/test_module_1.h"
#include "example/test_module_2.h"

void run_tests() {
    test::TestModule root_module("Example tests", nullptr);
    root_module.print_summary_enabled = true;
    test::TestModule* first_module = root_module.addModule<ExampleTestModule1>("ExampleModule1");
    test::TestModule* second_module = root_module.addModule<ExampleTestModule2>("ExampleModule2", { first_module });
    root_module.run();
}

int main() {
    run_tests();
    return 0;
}
