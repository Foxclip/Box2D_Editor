#include "common/logger.h"
#include "example/test_module_1.h"
#include "example/test_module_2.h"

void run_tests() {
    logger << "Running tests\n";
    LoggerIndent test_modules_indent;
    test::TestModule root_module("Example tests", nullptr);
    test::TestModule* first_module = root_module.addModule<ExampleTestModule1>("ExampleModule1");
    test::TestModule* second_module = root_module.addModule<ExampleTestModule2>("ExampleModule2", { first_module });
    root_module.run();
}

int main() {
    run_tests();
    return 0;
}
