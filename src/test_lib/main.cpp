#include "common/logger.h"
#include "test_lib/example/test_module_1.h"
#include "test_lib/example/test_module_2.h"

void run_tests() {
    logger << "Running tests\n";
    LoggerIndent test_modules_indent;
    test::TestManager test_manager;
    test::TestModule* first_module = test_manager.addModule<ExampleTestModule1>();
    test::TestModule* second_module = test_manager.addModule<ExampleTestModule2>({ first_module });
    test_manager.runAllModules();
}

int main() {
    run_tests();
    return 0;
}
