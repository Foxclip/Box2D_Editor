#include <cstdio>
#include <iostream>
#include "editor/editor.h"
#include "editor/scenes.h"
#include "common/logger.h"

void execute_app() {
    logger << "Starting app\n";
    Editor app(true);
    try {
        app.init("Box2D Editor");
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

    // TODO: replace other unique_ptr's with DataPointer
    // TODO: WidgetList: change case of variables in WidgetList (?)
    // TODO: Compvector: getUptr test
    // TODO: do memory debugging of loadFromFile again with custom smart pointers
    // TODO: Editor: Camera class
    // TODO: Editor: fix redundant serialize logger message
    // TODO: Outliner: fix selection after subtree collapse
    // TODO: Outliner: reparent object
    // TODO: Outliner: rename object
    // TODO: Outliner: delete object
    // TODO: Objectlist: OnAfterObjectRemoved test
    // TODO: Objectlist: OnObjectRenamed
    // TODO: ContainerWidget: test invisible widgets
    // TODO: Editor: render polygon indices
    // TODO: Widgets: make widgets a separate library
    // TODO: Editor: delay editing polygons with many vertices
    // TODO: Editor: joint editor
    // TODO: evolving cars
    // TODO: rename project to EvolvingCars
    // TODO: Editor: edit and simulate modes

    return 0;
}
