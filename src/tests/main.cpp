#include "tests/tests.h"
#include "common/logger.h"

void run_tests() {
    test::TestModule root_module("Project tests", nullptr);
    root_module.print_summary_enabled = true;
    test::TestModule* logger_module = root_module.addModule<LoggerTests>("Logger");
    test::TestModule* data_pointer_module = root_module.addModule<DataPointerTests>("DataPointer", { logger_module });
    test::TestModule* compvector_module = root_module.addModule<CompVectorTests>("CompVector", { logger_module, data_pointer_module });
    test::TestModule* searchindex_module = root_module.addModule<SearchIndexTests>("SearchIndex", { logger_module, data_pointer_module });
    test::TestModule* event_module = root_module.addModule<EventTests>("Event", { logger_module, data_pointer_module });
    test::TestModule* simulation_module = root_module.addModule<SimulationTests>("Simulation", { logger_module, data_pointer_module, compvector_module });
    test::TestModule* widget_module = root_module.addModule<WidgetTests>("Widget", { logger_module, data_pointer_module, event_module, compvector_module, searchindex_module });
    test::TestModule* editor_module = root_module.addModule<EditorTests>("Editor", { simulation_module, widget_module });
    root_module.run();
}

int main() {
    run_tests();
    return 0;
}
