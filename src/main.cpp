#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "editor/editor.h"
#include "editor/scenes.h"
#include "logger/logger.h"

int execute_app(bool screenshot, const std::string& screenshot_path) {
    logger << "Starting app\n";
    const bool maximized = !screenshot;
    Editor app(maximized);
    try {
        app.init("Box2D Editor", true, screenshot);
        app.load("levels/level.txt");
        if (screenshot) {
            if (!app.saveScreenshot(screenshot_path)) {
                logger << "ERROR: Failed to save screenshot to " << screenshot_path << "\n";
                return 1;
            }
            return 0;
        }
        app.start();
    } catch (std::string msg) {
        logger << "ERROR: " << msg << "\n";
    } catch (std::exception exc) {
        logger << "ERROR: " << exc.what() << "\n";
    }
    return 0;
}

int main(int argc, char* argv[]) {
    bool screenshot = false;
    std::string screenshot_path;
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--screenshot") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "ERROR: --screenshot requires a file path argument" << std::endl;
                return 1;
            }
            screenshot = true;
            screenshot_path = argv[i + 1];
        }
    }

    LoggerDisableTag disable_serialize_tag("serialize");
    LoggerDisableTag disable_recut_tag("recut");
    LoggerDisableTag disable_set_focused_widget("setFocusedWidget");
    LoggerDisableTag disable_mouse_gesture("mouseGesture");
    LoggerDisableTag disable_outliner("outliner");
    LoggerDisableTag disable_history("history");

    int exit_code = execute_app(screenshot, screenshot_path);

    // TODO: TreeViewWidget: buttons up/down for reordering objects
    // TODO: TreeViewWidget: reparent object by dragging
    // TODO: Editor: add mouse zooming
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

    return exit_code;
}
