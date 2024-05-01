#include <cstdio>
#include <iostream>
#include "main.h"
#include "editor.h"
#include "logger.h"
#include "scenes.h"
#include "tests/tests.h"
#include <tests/widget_tests.h>

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

void execute_app() {
    logger << "Starting app\n";
    Editor app;
    try {
        app.init();
        app.load("levels/level.txt");
        app.start();
    } catch (std::string msg) {
        logger << "ERROR: " << msg << "\n";
    } catch (std::exception exc) {
        logger << "ERROR: " << exc.what() << "\n";
    }
}

int main() {

    LoggerDisableTag disable_serialize_tag("serialize");
    LoggerDisableTag disable_recut_tag("recut");
    LoggerDisableTag disable_set_focused_widget("setFocusedWidget");
    LoggerDisableTag disable_mouse_gesture("mouseGesture");

    run_tests();
    //execute_app();

    // TODO: remove Application reference from Toolbox and other ui elements
    // TODO: make tool panel with move, rotate and duplicate tools
    // TODO: Window widget
    // TODO: align textboxes in EditWindow to the right
    // TODO: Dropdown widget
    // TODO: object list window (outliner)
    // TODO: render polygon indices
    // TODO: make widgets a separate library
    // TODO: delay editing polygons with many vertices
    // TODO: evolving cars
    // TODO: rename project to EvolvingCars
    // TODO: edit and simulate modes
    // TODO: create tests with keyboard and mouse event recording
    // TODO: rotated widgets, masking
    // TODO: joint editor

    return 0;
}
