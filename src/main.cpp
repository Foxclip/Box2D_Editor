#include <cstdio>
#include <iostream>
#include "editor/editor.h"
#include "editor/scenes.h"
#include "common/logger.h"
#include "widgets/tree_view_widget.h"
#include "widgets/scroll_area_widget.h"

void execute_app() {
    logger << "Starting app\n";
    Editor app(true);
    try {
        app.init("Box2D Editor");
        
        fw::ScrollAreaWidget* scroll_area = app.getWidgets().createScrollAreaWidget(300.0f, 300.0f);
        scroll_area->setParentAnchor(fw::Widget::Anchor::CENTER);
        fw::TreeViewWidget* treeview = app.getWidgets().createTreeViewWidget(300.0f, 300.0f);
        scroll_area->setScrolledWidget(treeview);
        treeview->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
        fw::TreeViewEntry* entry_1 = treeview->addEntry("Entry 1");
        fw::TreeViewEntry* entry_2 = treeview->addEntry("Entry 2");
        fw::TreeViewEntry* entry_3 = treeview->addEntry("Entry 3");
        fw::TreeViewEntry* entry_4 = treeview->addEntry("Entry 4");
        fw::TreeViewEntry* entry_5 = treeview->addEntry("Entry 5");
        fw::TreeViewEntry* entry_6 = treeview->addEntry("Entry 6");
        fw::TreeViewEntry* entry_6_1 = treeview->addEntry("Entry 6_1");
        fw::TreeViewEntry* entry_6_2 = treeview->addEntry("Entry 6_2");
		fw::TreeViewEntry* entry_6_3 = treeview->addEntry("Entry 6_3");
		entry_6_1->setParent(entry_6);
		entry_6_2->setParent(entry_6);
		entry_6_3->setParent(entry_6);
        fw::TreeViewEntry* entry_7 = treeview->addEntry("Entry 7");
        fw::TreeViewEntry* entry_7_1 = treeview->addEntry("Entry 7_1");
        fw::TreeViewEntry* entry_7_2 = treeview->addEntry("Entry 7_2");
        fw::TreeViewEntry* entry_7_3 = treeview->addEntry("Entry 7_3");
        entry_7_1->setParent(entry_7);
        entry_7_2->setParent(entry_7);
        entry_7_3->setParent(entry_7);
        fw::TreeViewEntry* entry_8 = treeview->addEntry("Entry 8");
        fw::TreeViewEntry* entry_8_1 = treeview->addEntry("Entry 8_1");
        fw::TreeViewEntry* entry_8_2 = treeview->addEntry("Entry 8_2");
        fw::TreeViewEntry* entry_8_3 = treeview->addEntry("Entry 8_3");
        entry_8_1->setParent(entry_8);
        entry_8_2->setParent(entry_8);
        entry_8_3->setParent(entry_8);

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

    // TODO: Tests: WidgetTestsTreeView: put getWindow and getFont into WidgetTests
    // TODO: Tests: visualize mouse movements and clicks
    // TODO: TreeViewWidget: cancel dragging with Esc
    // TODO: TreeViewWidget: reorder entries
    // TODO: TreeViewWidget: reparent entries
    // TODO: Simulation: rename getFromAll to getObject and getFromTop to getTopObject
    // TODO: Editor: put Camera in separate file
    // TODO: WidgetList: use addPending methods explicitly instead of sneakily calling them from other methods like moveToTop
    // TODO: Widget: make test for setParent with keep_pos = true
    // TODO: Outliner: rename object
    // TODO: Outliner: delete object
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
