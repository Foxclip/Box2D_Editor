#include <cstdio>
#include <iostream>
#include "main.h"
#include "application.h"
#include "logger.h"
#include "scenes.h"

#ifndef NDEBUG
void run_tests() {
    CompoundVectorTest();
    LoggerTest();
}
#endif

void execute_app() {
    Application app;
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
    std::cout << "Running tests\n";
    run_tests();
#endif

    LoggerDisableTag disable_serialize_tag("serialize");
    LoggerDisableTag disable_recut_tag("recut");
    LoggerDisableTag disable_set_focused_widget("setFocusedWidget");

    std::cout << "Starting app\n";
    execute_app();

    // TODO: remove global.h include from widgets
    // TDOO: ctrl-z in textbox
    // TODO: remove dependency on Application from UI classes
    // TODO: blinking textbox cursor
    // TODO: make SearchIndex not use pointers for TObject type
    // TODO: Dropdown widget
    // TODO: object list window (outliner)
    // TODO: render polygon indices
    // TODO: separate Simulation and Application,
    // interface in Application, physics and object management in Simulation
    // TODO: delay editing polygons with many vertices
    // TODO: evolving cars
    // TODO: rename project to EvolvingCars
    // TODO: edit and simulate modes
    // TODO: create tests with keyboard and mouse event recording
    // TODO: rotated widgets, masking
    // TODO: joint editor

    return 0;
}
