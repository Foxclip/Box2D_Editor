#include "tests/tests.h"
#include "logger/logger.h"

void run_tests() {
    test::TestModule root_module("Project tests", nullptr);
    root_module.print_summary_enabled = true;
    DataPointerTests* data_pointer_module = root_module.addModule<DataPointerTests>("DataPointer", { });
    CompVectorTests* compvector_module = root_module.addModule<CompVectorTests>("CompVector", { data_pointer_module });
    SearchIndexTests* searchindex_module = root_module.addModule<SearchIndexTests>("SearchIndex", { data_pointer_module });
    EventTests* event_module = root_module.addModule<EventTests>("Event", { data_pointer_module });
    SimulationTests* simulation_module = root_module.addModule<SimulationTests>("Simulation", { data_pointer_module, compvector_module });
    WidgetTests* widget_module = root_module.addModule<WidgetTests>("Widget", { data_pointer_module, event_module, compvector_module, searchindex_module });
    EditorTests* editor_module = root_module.addModule<EditorTests>("Editor", { simulation_module, widget_module });
    root_module.run();
}

int main() {
    run_tests();
    return 0;
}
