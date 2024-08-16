#include "tests/tests.h"
#include "common/logger.h"
#include "tests/example/test_module_1.h"
#include "tests/example/test_module_2.h"

namespace example_tests { // remove this

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

} // remove this
