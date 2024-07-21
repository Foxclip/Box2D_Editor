#include <cstdio>
#include <iostream>
#include "editor/editor.h"
#include "editor/scenes.h"
#include "common/logger.h"

void execute_app() {
    logger << "Starting app\n";
    Editor app;
    try {
        app.init();
        app.load("levels/level.txt");
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
    LoggerDisableTag disable_mouse_gesture("mouseGesture");

    execute_app();

    // TODO: TreeViewWidget
    // TODO: reparent object in outliner
    // TODO: rename object in outliner
    // TODO: delete object in outliner
    // TODO: OnAfterObjectRemoved test
    // TODO: OnObjectRenamed event in GameObjectList
    // TODOL test invisible widgets in container
    // TODO: render polygon indices
    // TODO: make widgets a separate library
    // TODO: delay editing polygons with many vertices
    // TODO: evolving cars
    // TODO: rename project to EvolvingCars
    // TODO: edit and simulate modes
    // TODO: create tests with keyboard and mouse event recording
    // TODO: rotated widgets, masking
    // TODO: joint editor

    return 0;
}
