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

    // TODO: recreate b2World on deserialization
    // TODO: create/delete objects in the scene
    // TODO: evolving cars
    // TODO: edit and simulate modes

    return 0;
}
