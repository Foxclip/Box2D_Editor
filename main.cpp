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

    return 0;
}
