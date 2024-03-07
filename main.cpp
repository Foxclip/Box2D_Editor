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
    //box_parent(app);
    //convex_polygon(app);
    //duplication(app);
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

    // TODO: text box widget
    // TODO: change (*it). to it->
    // TODO: SearchIndex for widget children
    // TODO: replace toVec in CompoundVector with implicit conversion
    // TODO: render polygon indices
    // TODO: delay editing polygons with many vertices
    // TODO: evolving cars
    // TODO: rename project to EvolvingCars
    // TODO: edit and simulate modes

    return 0;
}
