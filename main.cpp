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

    //TODO: hold shift for edge edit, hold ctrl for creating edge vertices
    //TODO: replace get_screen_normal with one function
    //TODO: move tool-related constants to tool classes
    //TODO: edit mode box selection
    //TODO: save/load levels to file
    //TODO: create/delete objects in the scene

    return 0;
}
