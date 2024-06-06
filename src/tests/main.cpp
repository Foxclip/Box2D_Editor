#include "tests/tests.h"
#include "logger.h"

void run_tests() {
    logger << "Running tests\n";
    LoggerIndent test_modules_indent;
    test::TestManager test_manager;
    test_manager.addModule<LoggerTests>();
    test_manager.addModule<CompVectorTests>();
    test_manager.addModule<SearchIndexTests>();
    test_manager.addModule<SimulationTests>();
    test_manager.addModule<WidgetTests>();
    test_manager.runAllModules();
}

int main() {
    run_tests();
    return 0;
}
