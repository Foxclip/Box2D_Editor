#include <cstdio>
#include <iostream>
#include "editor/editor.h"
#include "editor/scenes.h"
#include "common/logger.h"
#include "widgets/button_widget.h"

namespace fw {
    class fw::ButtonWidget;
}

void execute_app() {
    logger << "Starting app\n";
    Editor app(true);
    try {
        app.init("Box2D Editor");
        app.load("levels/level.txt");

        fw::ButtonWidget* button_widget = app.getWidgets().createButtonWidget(100.0f, 20.0f);
        button_widget->setGlobalPosition(app.getWindowCenter());

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
    LoggerDisableTag disable_outliner("outliner");
    LoggerDisableTag disable_history("history");

    execute_app();

    // TODO: Add ButtonWidget
    // TODO: TreeViewWidget: buttons up/down for reordering objects
    // TODO: Outliner: reorder objects
    // TODO: Outliner: reparent objects
    // TODO: RectangleWidget: rounded corners
    // TODO: Widget: remove quantize_rendered_position
    // TODO: Move widget library to the separate project
    // TODO: Simulation: rename getFromAll to getObject and getFromTop to getTopObject
    // TODO: Editor: put Camera in separate file
    // TODO: WidgetList: use addPending methods explicitly instead of sneakily calling them from other methods like moveToTop
    // TODO: Widget: make test for setParent with keep_pos = true
    // TODO: DataPointer: use snake_case for method names
    // TODO: Outliner: rename object
    // TODO: Outliner: delete object
    // TODO: TreeViewWidget: don't take grabbed widgets out of the hierarchy, create phantom copies instead
    // TODO: Objectlist: OnAfterObjectRemoved test
    // TODO: Objectlist: OnObjectRenamed
    // TODO: ContainerWidget: test invisible widgets
    // TODO: render children of the widget on the texture of the widget itself
    // TODO: Make AbstractHierachy class with methods like setParent, moveToTop, etc.
    // TODO: Editor: render polygon indices
    // TODO: Widgets: make widgets a separate library
    // TODO: Editor: delay editing polygons with many vertices
    // TODO: Editor: joint editor
    // TODO: evolving cars
    // TODO: rename project to EvolvingCars
    // TODO: Editor: edit and simulate modes

    return 0;
}
