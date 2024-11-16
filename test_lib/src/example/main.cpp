#include "common/logger.h"
#include "example/test_module_1.h"
#include "example/test_module_2.h"
#include "example/test_module_3.h"

void run_tests() {
    test::TestModule root_module("Example tests", nullptr);
    root_module.print_summary_enabled = true;
    test::TestModule* module_1 = root_module.addModule<ExampleTestModule1>("ExampleModule1");
    test::TestModule* module_2 = root_module.addModule<ExampleTestModule2>("ExampleModule2", { module_1 });
    test::TestModule* module_3 = root_module.addModule<ExampleTestModule3>("ExampleModule3");
    for (test::TestModule* module : root_module.getChildModules()) {
        module->print_summary_enabled = true;
    }
    root_module.run();
}

int main() {
    run_tests();
    return 0;
}
