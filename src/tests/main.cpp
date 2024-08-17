#include "tests/tests.h"
#include "common/logger.h"

void run_tests() {
    logger << "Running tests\n";
    LoggerIndent test_modules_indent;
    test::TestManager test_manager;
    test::TestModule* logger_module = test_manager.addModule<LoggerTests>();
    test::TestModule* compvector_module = test_manager.addModule<CompVectorTests>({ logger_module });
    test::TestModule* searchindex_module = test_manager.addModule<SearchIndexTests>({ logger_module });
    test::TestModule* event_module = test_manager.addModule<EventTests>({ logger_module });
    test::TestModule* simulation_module = test_manager.addModule<SimulationTests>({ logger_module, compvector_module });
    test::TestModule* widget_module = test_manager.addModule<WidgetTests>({ logger_module, event_module, compvector_module, searchindex_module });
    test::TestModule* editor_module = test_manager.addModule<EditorTests>({ simulation_module, widget_module });
    //test::TestModule* editor_module = test_manager.addModule<EditorTests>();
    test_manager.runAllModules();
}

int main() {
    run_tests();
    return 0;
}
