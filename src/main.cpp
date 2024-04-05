#include <cstdio>
#include <iostream>
#include "main.h"
#include "editor.h"
#include "logger.h"
#include "scenes.h"

#ifndef NDEBUG
void run_tests() {
    LoggerIndent test_modules_indent;
    test::TestManager test_manager;
    test_manager.addModule(std::make_unique<SimulationTests>());
    test_manager.addModule(std::make_unique<CompVectorTests>());
    test_manager.runAllModules();
    //LoggerTest();
    //SearchIndexTest();
}
#endif

void execute_app() {
    Editor app;
    try {
        app.init();
        //scene1(app);
        app.load("levels/level.txt");
        //ground_transform(app);
        //single_box(app);
        //single_ball(app);
        //single_car(app);
        //multiple_chains(app);
        //polygon(app);
        //box_parent(app);
        //convex_polygon(app);
        //duplication(app);
        //parent_loop(app);
        app.start();
    } catch (std::string msg) {
        logger << "ERROR: " << msg << "\n";
    } catch (std::exception exc) {
        logger << "ERROR: " << exc.what() << "\n";
    }
}

int main() {

#ifndef NDEBUG
    logger << "Running tests\n";
    run_tests();
#endif

    LoggerDisableTag disable_serialize_tag("serialize");
    LoggerDisableTag disable_recut_tag("recut");
    LoggerDisableTag disable_set_focused_widget("setFocusedWidget");
    LoggerDisableTag disable_mouse_gesture("mouseGesture");

    //logger << "Starting app\n";
    //execute_app();

    // TODO: rewrite CompVector tests with tCompare
    // TODO: remake compvector, logger, and searchindex tests with TestList
    // TODO: show total number of passed and failed tests at the end
    // TODO: Simulation tests
    // TODO: blinking textbox cursor
    // TODO: remove global.h
    // TODO: don't fill window with black color in Application::Render
    // TODO: CanvasWidget as a replacement to screen textures
    // TODO: EventRecorder for ui tests
    // TODO: make tool panel with move, rotate and duplicate tools
    // TODO: make SearchIndex not use pointers for TObject type
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
