#include <cstdio>
#include <iostream>
#include "application.h"
#include "logger.h"
#include "scenes.h"

void execute_app() {
    Application app;
    app.init();
    //scene1(app);
    app.load("level.txt");
    //ground_transform(app);
    //single_box(app);
    //single_ball(app);
    //single_car(app);
    //multiple_chains(app);
    //polygon(app);
    app.start();
}

int main() {
    
    try {
        execute_app();
    } catch (std::string msg) {
        logger << "ERROR: " << msg << "\n";
    } catch (std::exception exc) {
        logger << "ERROR: " << exc.what() << "\n";
    }

    // TODO: duplicate with Shift-D
    // TODO: fix select all not selecting child objects
    // TODO: rotate tool rotates objects around common center
    // TODO: add const to GameObject get methods
    // TODO: serialize car vertices
    // TODO: fix releasing left click on widgets in drag mode
    // TODO: cut polygons with b2maxvertices
    // TODO: set whether object is static or dynamic in edit mode
    // TODO: evolving cars
    // TODO: rename project to EvolvingCars
    // TODO: edit and simulate modes

    return 0;
}
