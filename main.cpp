#include <cstdio>
#include <iostream>
#include "main.h"
#include "application.h"
#include "logger.h"
#include "scenes.h"

void execute_app() {
    Application app;
    try {
        app.init();
        //scene1(app);
        app.load("level.txt");
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

    LoggerDisableTag disable_serialize_tag("serialize");
    LoggerDisableTag disable_recut_tag("recut");
    LoggerDisableTag disable_set_focused_widget("setFocusedWidget");
    execute_app();

    // TODO: text selection in textbox
    // TODO: select text in texbox on click
    // TODO: don't move cursor to beginning after pressing enter or esc
    // TODO: change cursor when hovering over textbox
    // TDOO: ctrl-z in textbox
    // TODO: restore text in textbox when losing focus without confirmation
    // TODO: close edit mode with esc key
    // TODO: blinking textbox cursor
    // TODO: change color of textbox mouse highlight
    // TODO: make SearchIndex not use pointers for TObject type
    // TODO: Dropdown widget
    // TODO: object list window
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
