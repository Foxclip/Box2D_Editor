#include <cstdio>
#include <iostream>
#include "application.h"
#include "logger.h"
#include "scenes.h"

void execute_app() {
    Application app;
    app.init();
    app.load("level.txt");
    //ground_transform(app);
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

    // TODO: remove ground pointer, allow multiple chains
    // TODO: edit mode with tab, move with G rotate with R, delete with X, select all with A
    // TODO: editing shapes
    // TODO: cut polygons with b2maxvertices
    // TODO: select tool rectangle select
    // TODO: set whether object is static or dynamic in edit mode
    // TODO: evolving cars
    // TODO: rename project to EvolvingCars
    // TODO: edit and simulate modes

    return 0;
}
