#include <cstdio>
#include <iostream>
#include "application.h"

int main() {
    
    try {
        Application app;
        app.init();
        app.start();
    } catch (std::string msg) {
        std::cout << "ERROR: " << msg << "\n";
    } catch (std::exception exc) {
        std::cout << "ERROR: " << exc.what() << "\n";
    }

    // TODO: createShape in CarObject constructor
    // TODO: ctrl-z
    // TODO: move mouseBody with the mouse
    // TODO: edit mode box selection
    // TODO: create/delete objects in the scene

    return 0;
}
