#include "tests/logger_tests.h"
#include "tests/compvector_tests.h"
#include "tests/searchindex_tests.h"
#include "tests/simulation_tests.h"
#include "tests/widget_tests.h"
#include "test.h"
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
