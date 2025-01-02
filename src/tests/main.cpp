#include "tests/tests.h"
#include "common/logger.h"

void run_tests() {
    test::TestModule root_module("Project tests", nullptr);
    root_module.print_summary_enabled = true;
    LoggerTests* logger_module = root_module.addModule<LoggerTests>("Logger");
    DataPointerTests* data_pointer_module = root_module.addModule<DataPointerTests>("DataPointer", { logger_module });
    CompVectorTests* compvector_module = root_module.addModule<CompVectorTests>("CompVector", { logger_module, data_pointer_module });
    SearchIndexTests* searchindex_module = root_module.addModule<SearchIndexTests>("SearchIndex", { logger_module, data_pointer_module });
    EventTests* event_module = root_module.addModule<EventTests>("Event", { logger_module, data_pointer_module });
    SimulationTests* simulation_module = root_module.addModule<SimulationTests>("Simulation", { logger_module, data_pointer_module, compvector_module });
    WidgetTests* widget_module = root_module.addModule<WidgetTests>("Widget", { logger_module, data_pointer_module, event_module, compvector_module, searchindex_module });
    EditorTests* editor_module = root_module.addModule<EditorTests>("Editor", { simulation_module, widget_module });
    root_module.run();
}

int main() {
    run_tests();
    return 0;
}
