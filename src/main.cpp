#include <cstdio>
#include <iostream>
#include "editor/editor.h"
#include "editor/scenes.h"
#include "common/logger.h"
#include "widgets/tree_view_widget.h"

void execute_app() {
    logger << "Starting app\n";
    Editor app(true);
    try {
        app.init("Box2D Editor");
        
        fw::TreeViewWidget* treeview = app.getWidgets().createTreeViewWidget(300.0f, 300.0f);
        treeview->setParentAnchor(fw::Widget::Anchor::CENTER);
        treeview->setSizeYPolicy(fw::Widget::SizePolicy::NONE);
        treeview->addEntry("Entry 1");
        treeview->addEntry("Entry 2");

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
    LoggerDisableTag disable_outliner("outliner");
    LoggerDisableTag disable_history("history");

    execute_app();

    // TODO: TreeViewWidget: drag entries
    // TODO: Outliner: reparent object
    // TODO: Editor: put Camera in separate file
    // TODO: Tests: split widget_tests.cpp into several files
    // TODO: TreeViewWidget: put Entry in separate file
    // TODO: WidgetList: use addPending methods explicitly
    // TODOL Widget: make test for setParent with keep_pos = true
    // TODO: Outliner: rename object
    // TODO: Outliner: delete object
    // TODO: Objectlist: OnAfterObjectRemoved test
    // TODO: Objectlist: OnObjectRenamed
    // TODO: ContainerWidget: test invisible widgets
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
