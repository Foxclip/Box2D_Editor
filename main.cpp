#include <cstdio>
#include <iostream>
#include "application.h"
#include "logger.h"

int main() {
    
    try {
        Application app;
        app.init();
        app.start();
    } catch (std::string msg) {
        logger << "ERROR: " << msg << "\n";
    } catch (std::exception exc) {
        logger << "ERROR: " << exc.what() << "\n";
    }

    // TODO: Fps counter from SFML_Toy
    // TODO: use size_t and ptrdiff instead of int
    // TODO: logger messages on screen
    // TODO: editing shapes
    // TODO: select tool rectangle select
    // TODO: set whether object is static or dynamic in edit mode
    // TODO: evolving cars
    // TODO: rename project to EvolvingCars
    // TODO: edit and simulate modes

    return 0;
}
