#include <cstdio>
#include <iostream>
#include "application.h"
#include "logger.h"

void execute_app() {
    Application app;
    app.init("level.txt");
    //app.init();
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

    // TODO: cut polygons with b2maxvertices
    // TODO: editing shapes
    // TODO: select tool rectangle select
    // TODO: set whether object is static or dynamic in edit mode
    // TODO: evolving cars
    // TODO: rename project to EvolvingCars
    // TODO: edit and simulate modes

    return 0;
}
