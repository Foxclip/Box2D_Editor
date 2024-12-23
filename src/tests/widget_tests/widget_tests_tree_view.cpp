#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_tree_view.h"
#include "widgets/tree_view_entry.h"

WidgetTestsTreeView::WidgetTestsTreeView(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : TestModule(name, parent, required_nodes) {
    test::Test* tree_view_widget_basic_test = addTest("basic", [&](test::Test& test) { treeviewWidgetBasicTest(test); });
    test::Test* tree_view_widget_entries_test = addTest("entries", { tree_view_widget_basic_test }, [&](test::Test& test) { treeviewWidgetEntriesTest(test); });
    test::Test* tree_view_widget_parent1_test = addTest("parent_1", { tree_view_widget_entries_test }, [&](test::Test& test) { treeviewWidgetParent1Test(test); });
    test::Test* tree_view_widget_parent2_test = addTest("parent_2", { tree_view_widget_entries_test }, [&](test::Test& test) { treeviewWidgetParent2Test(test); });
    test::Test* tree_view_widget_select_test = addTest("select", { tree_view_widget_parent1_test }, [&](test::Test& test) { treeviewWidgetSelectTest(test); });
    test::Test* tree_view_widget_reorder_test = addTest("reorder", { tree_view_widget_parent2_test }, [&](test::Test& test) { treeviewWidgetReorderTest(test); });
    test::Test* tree_view_widget_remove_test = addTest("remove", { tree_view_widget_parent2_test }, [&](test::Test& test) { treeviewWidgetRemoveTest(test); });
    test::Test* tree_view_widget_clear_test = addTest("clear", { tree_view_widget_parent2_test }, [&](test::Test& test) { treeviewWidgetClearTest(test); });
    test::Test* tree_view_widget_drag_1_test = addTest("drag_1", { tree_view_widget_reorder_test }, [&](test::Test& test) { treeviewWidgetDrag1Test(test); });
    test::Test* tree_view_widget_drag_2_test = addTest("drag_2", { tree_view_widget_drag_1_test }, [&](test::Test& test) { treeviewWidgetDrag2Test(test); });
	test::Test* tree_view_widget_drag_3_test = addTest("drag_3", { tree_view_widget_drag_2_test }, [&](test::Test& test) { treeviewWidgetDrag3Test(test); });
    test::Test* tree_view_widget_drag_4_test = addTest("drag_4", { tree_view_widget_drag_3_test }, [&](test::Test& test) { treeviewWidgetDrag4Test(test); });
    test::Test* tree_view_widget_drag_5_test = addTest("drag_5", { tree_view_widget_drag_2_test }, [&](test::Test& test) { treeviewWidgetDrag5Test(test); });
}

void WidgetTestsTreeView::treeviewWidgetBasicTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(tree_view_widget));
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = tree_view_widget;
    gwt.total_widgets = 3;
    gwt.type = fw::Widget::WidgetType::TreeView;
    gwt.name = "treeview";
    gwt.fullname = "root|treeview";
    gwt.is_visual_position_quantized = false;
    fw::WidgetVisibility visibility;
    visibility.addedToRoot = true;
    visibility.allParentsVisible = true;
    visibility.hasUnclippedRegion = true;
    visibility.nonZeroSize = true;
    visibility.onScreen = true;
    visibility.opaque = true;
    visibility.renderableSetting = true;
    visibility.visibleSetting = true;
    gwt.visibility = visibility;
    gwt.is_click_through = false;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
    gwt.is_focused = false;
    gwt.clip_children = true;
    gwt.force_custom_cursor = false;
    gwt.parent = root_widget;
    gwt.local_bounds = sf::FloatRect(sf::Vector2f(), size);
    gwt.global_bounds = sf::FloatRect(position, size);
    gwt.parent_local_bounds = gwt.global_bounds;
    gwt.visual_local_bounds = gwt.local_bounds;
    gwt.visual_global_bounds = gwt.global_bounds;
    gwt.visual_parent_local_bounds = gwt.global_bounds;
    T_WRAP_CONTAINER(WidgetTests::genericWidgetTest(gwt));
}

void WidgetTestsTreeView::treeviewWidgetEntriesTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    tree_view_widget->addEntry("Entry 1");
    tree_view_widget->addEntry("Entry 2");
    tree_view_widget->addEntry("Entry 3");
    application.advance();
    auto rect_to_str = &WidgetTests::floatRectToStr;
    auto rect_approx_cmp = &WidgetTests::rectApproxCmp;
    float treeview_height = calcTreeViewHeight(tree_view_widget);
    sf::FloatRect local_bounds(0.0f, 0.0f, size.x, treeview_height);
    T_COMPARE(tree_view_widget->getLocalBounds(), local_bounds, rect_to_str, rect_approx_cmp);
}

void WidgetTestsTreeView::treeviewWidgetParent1Test(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewEntry* entry_2_child_1 = tree_view_widget->addEntry("Entry 2 Child 1");
    fw::TreeViewEntry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewEntry* entry_3_child_1 = tree_view_widget->addEntry("Entry 3 Child 1");
    fw::TreeViewEntry* entry_3_child_2 = tree_view_widget->addEntry("Entry 3 Child 2");
    entry_2_child_1->setParent(entry_2);
    entry_3_child_1->setParent(entry_3);
    entry_3_child_2->setParent(entry_3);
    tree_view_widget->expandAll();
    application.advance();
    auto rect_to_str = &WidgetTests::floatRectToStr;
    auto rect_approx_cmp = &WidgetTests::rectApproxCmp;
    float treeview_height = calcTreeViewHeight(tree_view_widget);
    sf::FloatRect local_bounds(0.0f, 0.0f, size.x, treeview_height);
    T_COMPARE(tree_view_widget->getLocalBounds(), local_bounds, rect_to_str, rect_approx_cmp);
}

void WidgetTestsTreeView::treeviewWidgetParent2Test(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_1_child_1 = tree_view_widget->addEntry("Entry 1 Child 1");
    fw::TreeViewEntry* entry_1_child_1_child_1 = tree_view_widget->addEntry("Entry 1 Child 1 Child 1");
    entry_1_child_1->setParent(entry_1);
    entry_1_child_1_child_1->setParent(entry_1_child_1);
    tree_view_widget->expandAll();
    application.advance();
    auto rect_to_str = &WidgetTests::floatRectToStr;
    auto rect_approx_cmp = &WidgetTests::rectApproxCmp;
    float treeview_height = calcTreeViewHeight(tree_view_widget);
    sf::FloatRect local_bounds(0.0f, 0.0f, size.x, treeview_height);
    T_COMPARE(tree_view_widget->getLocalBounds(), local_bounds, rect_to_str, rect_approx_cmp);
}

void WidgetTestsTreeView::treeviewWidgetSelectTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    auto click_entry = [&](fw::TreeViewEntry* entry, bool shift = false, bool ctrl = false) {
        sf::Vector2f entry_pos = entry->getRectangleWidget()->getGlobalCenter();
        application.mouseMove(entry_pos);
        if (shift) {
            application.keyPress(sf::Keyboard::LShift);
        }
        if (ctrl) {
            application.keyPress(sf::Keyboard::LControl);
        }
        application.mouseLeftPress();
        application.advance();
        application.mouseLeftRelease();
        application.advance();
        if (ctrl) {
            application.keyRelease(sf::Keyboard::LControl);
        }
        if (shift) {
            application.keyRelease(sf::Keyboard::LShift);
        }
        application.advance();
    };
    auto click_arrow = [&](fw::TreeViewEntry* entry) {
        sf::Vector2f arrow_pos = entry->getArrowAreaWidget()->getGlobalCenter();
        application.mouseMove(arrow_pos);
        application.mouseLeftPress();
        application.advance();
        application.mouseLeftRelease();
        application.advance();
    };
    CompVector<fw::TreeViewEntry*> clicked_entries;
    CompVector<fw::TreeViewEntry*> selected_entries;
    CompVector<fw::TreeViewEntry*> deselected_entries;
    tree_view_widget->OnEntryClicked += [&](fw::TreeViewEntry* entry) {
        clicked_entries.add(entry);
    };
    tree_view_widget->OnEntrySelected += [&](fw::TreeViewEntry* entry) {
        selected_entries.add(entry);
    };
    tree_view_widget->OnEntryDeselected += [&](fw::TreeViewEntry* entry) {
        deselected_entries.add(entry);
    };
    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewEntry* entry_2_child_1 = tree_view_widget->addEntry("Entry 2 Child 1");
    fw::TreeViewEntry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewEntry* entry_3_child_1 = tree_view_widget->addEntry("Entry 3 Child 1");
    fw::TreeViewEntry* entry_3_child_2 = tree_view_widget->addEntry("Entry 3 Child 2");
    entry_2_child_1->setParent(entry_2);
    entry_3_child_1->setParent(entry_3);
    entry_3_child_2->setParent(entry_3);
    application.advance();

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_entry(entry_1);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_1);
    T_ASSERT(T_CHECK(selected_entries.size() == 1));
    T_CHECK(selected_entries.back() == entry_1);

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_entry(entry_2);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_2);
    T_ASSERT(T_CHECK(deselected_entries.size() == 1));
    T_CHECK(deselected_entries.back() == entry_1);
    T_ASSERT(T_CHECK(selected_entries.size() == 1));
    T_CHECK(selected_entries.back() == entry_2);

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_arrow(entry_2);
    click_entry(entry_2_child_1);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_2_child_1);
    T_ASSERT(T_CHECK(deselected_entries.size() == 1));
    T_CHECK(deselected_entries.back() == entry_2);
    T_ASSERT(T_CHECK(selected_entries.size() == 1));
    T_CHECK(selected_entries.back() == entry_2_child_1);

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_arrow(entry_3);
    click_entry(entry_3_child_1, true, false);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_3_child_1);
    T_ASSERT(T_CHECK(deselected_entries.size() == 0));
    T_ASSERT(T_CHECK(selected_entries.size() == 1));
    T_CHECK(selected_entries.back() == entry_3_child_1);

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_entry(entry_1, true, false);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_1);
    T_ASSERT(T_CHECK(deselected_entries.size() == 0));
    T_ASSERT(T_CHECK(selected_entries.size() == 1));
    T_CHECK(selected_entries.back() == entry_1);

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_entry(entry_2_child_1, true, false);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_2_child_1);
    T_ASSERT(T_CHECK(deselected_entries.size() == 1));
    T_CHECK(deselected_entries.back() == entry_2_child_1);
    T_ASSERT(T_CHECK(selected_entries.size() == 0));

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_entry(entry_2, true, true);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_2);
    T_ASSERT(T_CHECK(deselected_entries.size() == 0));
    T_ASSERT(T_CHECK(selected_entries.size() == 2));
    T_CHECK(selected_entries[0] == entry_2_child_1);
    T_CHECK(selected_entries[1] == entry_2);

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_entry(entry_3, true, true);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_3);
    T_ASSERT(T_CHECK(deselected_entries.size() == 0));
    T_ASSERT(T_CHECK(selected_entries.size() == 2));
    T_CHECK(selected_entries[0] == entry_3_child_2);
    T_CHECK(selected_entries[1] == entry_3);

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_entry(entry_3, true, true);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_3);
    T_ASSERT(T_CHECK(deselected_entries.size() == 3));
    T_CHECK(deselected_entries[0] == entry_3_child_1);
    T_CHECK(deselected_entries[1] == entry_3_child_2);
    T_CHECK(deselected_entries[2] == entry_3);
    T_ASSERT(T_CHECK(selected_entries.size() == 0));

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_entry(entry_2, false, true);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_2);
    T_ASSERT(T_CHECK(deselected_entries.size() == 1));
    T_CHECK(deselected_entries[0] == entry_1);
    T_ASSERT(T_CHECK(selected_entries.size() == 0));

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_entry(entry_2, false, true);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_2);
    T_ASSERT(T_CHECK(deselected_entries.size() == 0));
    T_ASSERT(T_CHECK(selected_entries.size() == 0));

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    tree_view_widget->deselectAll();
    T_ASSERT(T_CHECK(clicked_entries.size() == 0));
    T_ASSERT(T_CHECK(deselected_entries.size() == 2));
    T_CHECK(deselected_entries[0] == entry_2_child_1);
    T_CHECK(deselected_entries[1] == entry_2);
    T_ASSERT(T_CHECK(selected_entries.size() == 0));

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_entry(entry_3_child_2);
    click_entry(entry_3);
    T_ASSERT(T_CHECK(clicked_entries.size() == 2));
    T_CHECK(clicked_entries[0] == entry_3_child_2);
    T_CHECK(clicked_entries[1] == entry_3);
    T_ASSERT(T_CHECK(deselected_entries.size() == 1));
    T_CHECK(deselected_entries[0] == entry_3_child_2);
    T_ASSERT(T_CHECK(selected_entries.size() == 2));
    T_CHECK(selected_entries[0] == entry_3_child_2);
    T_CHECK(selected_entries[1] == entry_3);

    clicked_entries.clear();
    selected_entries.clear();
    deselected_entries.clear();
    click_entry(entry_3, false, true);
    T_ASSERT(T_CHECK(clicked_entries.size() == 1));
    T_CHECK(clicked_entries.back() == entry_3);
    T_ASSERT(T_CHECK(deselected_entries.size() == 0));
    T_ASSERT(T_CHECK(selected_entries.size() == 2));
    T_CHECK(selected_entries[0] == entry_3_child_1);
    T_CHECK(selected_entries[1] == entry_3_child_2);
}

void WidgetTestsTreeView::treeviewWidgetReorderTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(200.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewEntry* entry_2_child_1 = tree_view_widget->addEntry("Entry 2 Child 1");
    fw::TreeViewEntry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewEntry* entry_3_child_1 = tree_view_widget->addEntry("Entry 3 Child 1");
    fw::TreeViewEntry* entry_3_child_2 = tree_view_widget->addEntry("Entry 3 Child 2");
    fw::TreeViewEntry* entry_4 = tree_view_widget->addEntry("Entry 4");
    fw::TreeViewEntry* entry_4_child_1 = tree_view_widget->addEntry("Entry 4 Child 1");
    fw::TreeViewEntry* entry_4_child_1_child_1 = tree_view_widget->addEntry("Entry 4 Child 1 Child 1");
    fw::TreeViewEntry* entry_4_child_1_child_2 = tree_view_widget->addEntry("Entry 4 Child 1 Child 2");
    entry_2_child_1->setParent(entry_2);
    entry_3_child_1->setParent(entry_3);
    entry_3_child_2->setParent(entry_3);
    entry_4_child_1->setParent(entry_4);
    entry_4_child_1_child_1->setParent(entry_4_child_1);
    entry_4_child_1_child_2->setParent(entry_4_child_1);
    tree_view_widget->expandAll();
    application.advance();

    const CompVector<fw::TreeViewEntry*>& top_entries = tree_view_widget->getTopEntries();
    T_ASSERT(T_COMPARE(top_entries.size(), 4));
    T_CHECK(top_entries[0] == entry_1);
    T_CHECK(top_entries[1] == entry_2);
    T_CHECK(top_entries[2] == entry_3);
    T_CHECK(top_entries[3] == entry_4);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));
    {
        entry_1->moveToIndex(2);
        application.advance();
        T_ASSERT(T_COMPARE(top_entries.size(), 4));
        T_CHECK(top_entries[0] == entry_2);
        T_CHECK(top_entries[1] == entry_1);
        T_CHECK(top_entries[2] == entry_3);
        T_CHECK(top_entries[3] == entry_4);
        T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));
    }
    {
        entry_3_child_2->moveToIndex(0);
        application.advance();
        const CompVector<fw::TreeViewEntry*>& children = entry_3->getChildren();
        T_ASSERT(T_COMPARE(children.size(), 2));
        T_CHECK(children[0] == entry_3_child_2);
        T_CHECK(children[1] == entry_3_child_1);
        T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));
    }
    {
        entry_4_child_1_child_1->moveToIndex(2);
        application.advance();
        const CompVector<fw::TreeViewEntry*>& children = entry_4_child_1->getChildren();
        T_ASSERT(T_COMPARE(children.size(), 2));
        T_CHECK(children[0] == entry_4_child_1_child_2);
        T_CHECK(children[1] == entry_4_child_1_child_1);
        T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));
    }
}

void WidgetTestsTreeView::treeviewWidgetRemoveTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(200.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewEntry* entry_2_child_1 = tree_view_widget->addEntry("Entry 2 Child 1");
    fw::TreeViewEntry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewEntry* entry_3_child_1 = tree_view_widget->addEntry("Entry 3 Child 1");
    fw::TreeViewEntry* entry_3_child_2 = tree_view_widget->addEntry("Entry 3 Child 2");
    fw::TreeViewEntry* entry_4 = tree_view_widget->addEntry("Entry 4");
    fw::TreeViewEntry* entry_4_child_1 = tree_view_widget->addEntry("Entry 4 Child 1");
    fw::TreeViewEntry* entry_4_child_2 = tree_view_widget->addEntry("Entry 4 Child 2");
    fw::TreeViewEntry* entry_4_child_3 = tree_view_widget->addEntry("Entry 4 Child 3");
    fw::TreeViewEntry* entry_5 = tree_view_widget->addEntry("Entry 5");
    fw::TreeViewEntry* entry_5_child_1 = tree_view_widget->addEntry("Entry 5 Child 1");
    fw::TreeViewEntry* entry_5_child_1_child_1 = tree_view_widget->addEntry("Entry 5 Child 1 Child 1");
    entry_2_child_1->setParent(entry_2);
    entry_3_child_1->setParent(entry_3);
    entry_3_child_2->setParent(entry_3);
    entry_4_child_1->setParent(entry_4);
    entry_4_child_2->setParent(entry_4);
    entry_4_child_3->setParent(entry_4);
    entry_5_child_1->setParent(entry_5);
    entry_5_child_1_child_1->setParent(entry_5_child_1);
    tree_view_widget->expandAll();
    application.advance();

    const CompVector<fw::TreeViewEntry*>& top_entries = tree_view_widget->getTopEntries();
    auto check_treeview_height = [&]() {
        float treeview_height = calcTreeViewHeight(tree_view_widget);
        return tree_view_widget->getHeight() == treeview_height;
    };
    T_ASSERT(T_COMPARE(tree_view_widget->getTopEntryCount(), 5));
    T_ASSERT(T_COMPARE(tree_view_widget->getAllEntryCount(), 13));
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));

    // remove single
    entry_1->remove(false);
    application.advance();
    T_COMPARE(tree_view_widget->getTopEntryCount(), 4);
    T_COMPARE(tree_view_widget->getAllEntryCount(), 12);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));

    // remove the olny child
    entry_2_child_1->remove(false);
    application.advance();
    T_COMPARE(tree_view_widget->getTopEntryCount(), 4);
    T_COMPARE(tree_view_widget->getAllEntryCount(), 11);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));

    // remove single
    entry_2->remove(false);
    application.advance();
    T_COMPARE(tree_view_widget->getTopEntryCount(), 3);
    T_COMPARE(tree_view_widget->getAllEntryCount(), 10);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));

    // remove without children
    entry_4->remove(false);
    application.advance();
    if (T_COMPARE(tree_view_widget->getTopEntryCount(), 5)) {
        T_CHECK(top_entries[0] == entry_3);
        T_CHECK(top_entries[1] == entry_4_child_1);
        T_CHECK(top_entries[2] == entry_4_child_2);
        T_CHECK(top_entries[3] == entry_4_child_3);
        T_CHECK(top_entries[4] == entry_5);
    }
    T_COMPARE(tree_view_widget->getAllEntryCount(), 9);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));

    // remove with children
    entry_3->remove(true);
    application.advance();
    T_COMPARE(tree_view_widget->getTopEntryCount(), 4);
    T_COMPARE(tree_view_widget->getAllEntryCount(), 6);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));

    // remove child with a child
    entry_5_child_1->remove(true);
    application.advance();
    T_COMPARE(tree_view_widget->getTopEntryCount(), 4);
    T_COMPARE(tree_view_widget->getAllEntryCount(), 4);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));

    // add widgets
    fw::TreeViewEntry* entry_5_child_2 = tree_view_widget->addEntry("Entry 5 Child 2");
    fw::TreeViewEntry* entry_5_child_3 = tree_view_widget->addEntry("Entry 5 Child 3");
    fw::TreeViewEntry* entry_5_child_3_child_1 = tree_view_widget->addEntry("Entry 5 Child 3 Child 1");
    fw::TreeViewEntry* entry_5_child_3_child_2 = tree_view_widget->addEntry("Entry 5 Child 3 Child 2");
    fw::TreeViewEntry* entry_5_child_3_child_3 = tree_view_widget->addEntry("Entry 5 Child 3 Child 3");
    fw::TreeViewEntry* entry_5_child_4 = tree_view_widget->addEntry("Entry 5 Child 4");
    entry_5_child_2->setParent(entry_5);
    entry_5_child_3->setParent(entry_5);
    entry_5_child_3_child_1->setParent(entry_5_child_3);
    entry_5_child_3_child_2->setParent(entry_5_child_3);
    entry_5_child_3_child_3->setParent(entry_5_child_3);
    entry_5_child_4->setParent(entry_5);
    tree_view_widget->expandAll();
    application.advance();
    T_COMPARE(tree_view_widget->getTopEntryCount(), 4);
    T_COMPARE(tree_view_widget->getAllEntryCount(), 10);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));

    // remove child without children
    entry_5_child_3->remove(false);
    application.advance();
    if (T_COMPARE(entry_5->getChildrenCount(), 5)) {
        T_CHECK(entry_5->getChild(0) == entry_5_child_2);
        T_CHECK(entry_5->getChild(1) == entry_5_child_3_child_1);
        T_CHECK(entry_5->getChild(2) == entry_5_child_3_child_2);
        T_CHECK(entry_5->getChild(3) == entry_5_child_3_child_3);
        T_CHECK(entry_5->getChild(4) == entry_5_child_4);
    }
    T_COMPARE(tree_view_widget->getAllEntryCount(), 9);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));

    // remove all widgets
    entry_4_child_1->remove(true);
    entry_4_child_2->remove(true);
    entry_4_child_3->remove(true);
    entry_5->remove(true);
    application.advance();
    T_COMPARE(tree_view_widget->getTopEntryCount(), 0);
    T_COMPARE(tree_view_widget->getAllEntryCount(), 0);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));
}

void WidgetTestsTreeView::treeviewWidgetClearTest(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(200.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewEntry* entry_2_child_1 = tree_view_widget->addEntry("Entry 2 Child 1");
    fw::TreeViewEntry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewEntry* entry_3_child_1 = tree_view_widget->addEntry("Entry 3 Child 1");
    fw::TreeViewEntry* entry_3_child_2 = tree_view_widget->addEntry("Entry 3 Child 2");
    fw::TreeViewEntry* entry_4 = tree_view_widget->addEntry("Entry 4");
    fw::TreeViewEntry* entry_4_child_1 = tree_view_widget->addEntry("Entry 4 Child 1");
    fw::TreeViewEntry* entry_4_child_2 = tree_view_widget->addEntry("Entry 4 Child 2");
    fw::TreeViewEntry* entry_4_child_3 = tree_view_widget->addEntry("Entry 4 Child 3");
    fw::TreeViewEntry* entry_5 = tree_view_widget->addEntry("Entry 5");
    fw::TreeViewEntry* entry_5_child_1 = tree_view_widget->addEntry("Entry 5 Child 1");
    fw::TreeViewEntry* entry_5_child_1_child_1 = tree_view_widget->addEntry("Entry 5 Child 1 Child 1");
    entry_1->setParent(entry_2_child_1); // simple cycle will not work with hierarchy like this
    entry_2_child_1->setParent(entry_2);
    entry_3_child_1->setParent(entry_3);
    entry_3_child_2->setParent(entry_3);
    entry_4_child_1->setParent(entry_4);
    entry_4_child_2->setParent(entry_4);
    entry_4_child_3->setParent(entry_4);
    entry_5_child_1->setParent(entry_5);
    entry_5_child_1_child_1->setParent(entry_5_child_1);
    tree_view_widget->expandAll();
    application.advance();

    tree_view_widget->clear();
    application.advance();
    T_COMPARE(tree_view_widget->getTopEntryCount(), 0);
    T_COMPARE(tree_view_widget->getAllEntryCount(), 0);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));

    fw::TreeViewEntry* entry_6 = tree_view_widget->addEntry("Entry 6");
    application.advance();
    T_COMPARE(tree_view_widget->getTopEntryCount(), 1);
    T_COMPARE(tree_view_widget->getAllEntryCount(), 1);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));
}

void WidgetTestsTreeView::treeviewWidgetDrag1Test(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(200.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_2 = tree_view_widget->addEntry("Entry 2");
    application.advance();

    auto check_top_entries = [&](const std::initializer_list<fw::TreeViewEntry*>& entries) {
        checkTopEntries(test, tree_view_widget, entries);
    };
    auto check_entries = [&](fw::TreeViewEntry* entry, const std::initializer_list<fw::TreeViewEntry*> entries) {
        checkEntries(test, entry, entries);
    };

    sf::Vector2f center_offset = sf::Vector2f(0.0f, -2.0f);
    fw::Widget* entry_1_widget = entry_1->getWidget();
    fw::Widget* entry_2_widget = entry_2->getWidget();
    fw::Widget* target_highlight_widget = tree_view_widget->getTargetHighlightWidget();

    // drag to the right
    sf::Vector2f entry_1_center = entry_1_widget->getGlobalCenter();
    sf::Vector2f drag_offset_1 = sf::Vector2f(fw::TREEVIEW_ENTRY_DRAG_DISTANCE + 5.0f, 0.0f);
    sf::Vector2f drag_start = entry_1_center + center_offset;
    sf::Vector2f drag_pos_1 = drag_start + drag_offset_1;
    application.mouseMove(drag_start);
    application.advance();
    application.mouseLeftPress();
    application.advance();
    T_CHECK(!entry_1->isGrabbed());
    T_CHECK(!target_highlight_widget->isVisible());
    application.mouseMove(drag_pos_1);
    application.advance();
    T_CHECK(entry_1->isGrabbed());
    T_CHECK(!target_highlight_widget->isVisible());
    target_highlight_widget->setDebugRender(true);
    application.advance();
    target_highlight_widget->setDebugRender(false);
    T_CHECK(entry_1->isGrabbed());
    T_CHECK(target_highlight_widget->isVisible());
    T_VEC2_COMPARE(target_highlight_widget->getGlobalOriginPosition(), entry_2_widget->getGlobalPosition());

    // drag down
    sf::Vector2f drag_pos_2 = drag_pos_1 + sf::Vector2f(0.0f, 50.0f);
    application.mouseMove(drag_pos_2);
    application.advance();
    T_VEC2_COMPARE(target_highlight_widget->getGlobalPosition(), entry_2_widget->getGlobalBottomLeft());

    // drag up
    sf::Vector2f drag_pos_3 = drag_pos_1 + sf::Vector2f(0.0f, -50.0f);
    application.mouseMove(drag_pos_3);
    application.advance();
    T_VEC2_COMPARE(target_highlight_widget->getGlobalOriginPosition(), entry_2_widget->getGlobalPosition());

    // drop to the top
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(!target_highlight_widget->isVisible());
    T_COMPARE(tree_view_widget->getAllEntryCount(), 2);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2 }));

    // drop to the bottom
    application.mouseMove(drag_start);
    application.advance();
    application.mouseLeftPress();
    application.advance();
    application.mouseMove(drag_pos_2);
    application.advance();
    T_CHECK(!target_highlight_widget->isVisible());
    application.advance();
    T_CHECK(target_highlight_widget->isVisible());
    T_VEC2_COMPARE(target_highlight_widget->getGlobalPosition(), entry_2_widget->getGlobalBottomLeft());
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(!target_highlight_widget->isVisible());
    T_COMPARE(tree_view_widget->getAllEntryCount(), 2);
    T_WRAP_CONTAINER(check_top_entries({ entry_2, entry_1 }));
}

void WidgetTestsTreeView::treeviewWidgetDrag2Test(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(200.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewEntry* entry_3 = tree_view_widget->addEntry("Entry 3");
    entry_3->setParent(entry_2);
    fw::Widget* entry_1_widget = entry_1->getWidget();
    fw::Widget* entry_2_widget = entry_2->getWidget();
    fw::Widget* entry_2_children_widget = entry_2->getChildrenWidget();
    fw::Widget* entry_3_widget = entry_3->getWidget();
    fw::Widget* target_highlight_widget = tree_view_widget->getTargetHighlightWidget();
    application.advance();

    auto check_top_entries = [&](const std::initializer_list<fw::TreeViewEntry*>& entries) {
        checkTopEntries(test, tree_view_widget, entries);
    };
    auto check_entries = [&](fw::TreeViewEntry* entry, const std::initializer_list<fw::TreeViewEntry*> entries) {
        checkEntries(test, entry, entries);
    };
    auto get_bottom_drag_pos = [&]() {
		return tree_view_widget->getBottom() + sf::Vector2f(0.0f, 50.0f);
    };
    auto drag_entry = [&](fw::TreeViewEntry* entry, const sf::Vector2f& pos, bool drop = false) {
        dragEntry(application, entry, pos, drop);
    };

    // drop to the top
    sf::Vector2f drag_top = tree_view_widget->getTop() + sf::Vector2f(0.0f, -50.0f);
    drag_entry(entry_1, drag_top);
    application.advance();
    T_VEC2_COMPARE(target_highlight_widget->getGlobalOriginPosition(), entry_2_widget->getGlobalPosition());
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(!target_highlight_widget->isVisible());
    T_COMPARE(tree_view_widget->getAllEntryCount(), 3);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2 }));

    // drop to the bottom
    T_ASSERT_NO_ERRORS();
    drag_entry(entry_1, get_bottom_drag_pos());
    application.advance();
    T_VEC2_COMPARE(target_highlight_widget->getGlobalPosition(), entry_2_widget->getGlobalBottomLeft());
    application.mouseLeftRelease();
    application.advance();

    // drop inside
    T_ASSERT_NO_ERRORS();
    entry_2->expand();
    application.advance();
    sf::Vector2f drag_inside = entry_3_widget->getGlobalTop();
    drag_entry(entry_1, drag_inside);
    application.advance();
    T_VEC2_COMPARE(target_highlight_widget->getGlobalOriginPosition(), entry_3_widget->getGlobalPosition());
    application.mouseLeftRelease();
    application.advance();
    T_COMPARE(tree_view_widget->getAllEntryCount(), 3);
    T_WRAP_CONTAINER(check_top_entries({ entry_2 }));
    T_WRAP_CONTAINER(check_entries(entry_2, { entry_1, entry_3 }));

    // drop to the same place inside
    T_ASSERT_NO_ERRORS();
    application.mouseMove(entry_1_widget->getGlobalCenter());
    application.mouseLeftPress();
    application.advance();
    application.mouseMove(drag_top);
    application.advance();
    application.mouseMove(drag_inside);
    application.advance();
    application.mouseLeftRelease();
    application.advance();
    T_COMPARE(tree_view_widget->getAllEntryCount(), 3);
    T_WRAP_CONTAINER(check_top_entries({ entry_2 }));
    T_WRAP_CONTAINER(check_entries(entry_2, { entry_1, entry_3 }));

    // drop outside
    T_ASSERT_NO_ERRORS();
    drag_entry(entry_1, drag_top);
    application.advance();
    application.mouseLeftRelease();
    application.advance();
    T_COMPARE(tree_view_widget->getAllEntryCount(), 3);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2 }));

    // drop outside the only child
    T_ASSERT_NO_ERRORS();
    drag_entry(entry_3, get_bottom_drag_pos());
    application.advance();
    application.mouseLeftRelease();
    application.advance();
    T_COMPARE(tree_view_widget->getAllEntryCount(), 3);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2, entry_3 }));

    // drop entry with a child to the bottom
    entry_3->setParent(entry_2);
    application.advance();
    sf::Vector2f entry_2_rect_center = entry_2->getRectangleWidget()->getGlobalCenter();
    application.mouseMove(entry_2_rect_center);
    application.mouseLeftPress();
    application.advance();
    sf::Vector2f bottom_drag_pos = get_bottom_drag_pos();
    application.mouseMove(bottom_drag_pos);
    application.advance();
    application.advance();
    application.mouseLeftRelease();
    application.advance();
	T_COMPARE(tree_view_widget->getAllEntryCount(), 3);
	T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2 }));
	T_WRAP_CONTAINER(check_entries(entry_2, { entry_3 }));

    // drop entry with a child to the top
	application.mouseMove(entry_2_rect_center);
	application.mouseLeftPress();
	application.advance();
	application.mouseMove(drag_top);
	application.advance();
	application.advance();
	application.mouseLeftRelease();
	application.advance();
	T_COMPARE(tree_view_widget->getAllEntryCount(), 3);
	T_WRAP_CONTAINER(check_top_entries({ entry_2, entry_1}));
	T_WRAP_CONTAINER(check_entries(entry_2, { entry_3 }));
}

void WidgetTestsTreeView::treeviewWidgetDrag3Test(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(200.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    auto drag_entry = [&](fw::TreeViewEntry* entry, const sf::Vector2f& pos, bool drop = false) {
        dragEntry(application, entry, pos, drop);
    };
    auto check_top_entries = [&](const std::initializer_list<fw::TreeViewEntry*>& entries) {
        checkTopEntries(test, tree_view_widget, entries);
    };
    auto check_entries = [&](fw::TreeViewEntry* entry, const std::initializer_list<fw::TreeViewEntry*> entries) {
        checkEntries(test, entry, entries);
    };

    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_2 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewEntry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewEntry* entry_4 = tree_view_widget->addEntry("Entry 4");
    fw::TreeViewEntry* entry_4_1 = tree_view_widget->addEntry("Entry 4_1");
    fw::TreeViewEntry* entry_4_2 = tree_view_widget->addEntry("Entry 4_2");
    entry_4_1->setParent(entry_4);
    entry_4_2->setParent(entry_4);
    fw::TreeViewEntry* entry_5 = tree_view_widget->addEntry("Entry 5");
    fw::TreeViewEntry* entry_5_1 = tree_view_widget->addEntry("Entry 5_1");
    fw::TreeViewEntry* entry_5_2 = tree_view_widget->addEntry("Entry 5_2");
    entry_5_1->setParent(entry_5);
    entry_5_2->setParent(entry_5);
    fw::TreeViewEntry* entry_6 = tree_view_widget->addEntry("Entry 6");
    fw::TreeViewEntry* entry_6_1 = tree_view_widget->addEntry("Entry 6_1");
    fw::TreeViewEntry* entry_6_2 = tree_view_widget->addEntry("Entry 6_2");
    entry_6_1->setParent(entry_6);
    entry_6_2->setParent(entry_6);
    application.advance();
    T_COMPARE(tree_view_widget->getAllEntryCount(), 12);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2, entry_3, entry_4, entry_5, entry_6 }));
    T_WRAP_CONTAINER(check_entries(entry_4, { entry_4_1, entry_4_2 }));
    T_WRAP_CONTAINER(check_entries(entry_5, { entry_5_1, entry_5_2 }));
    T_WRAP_CONTAINER(check_entries(entry_6, { entry_6_1, entry_6_2 }));

    // drag entry 6 in between entry 4 and entry 5
    sf::Vector2f drag_pos_1 = entry_5->getRectangleWidget()->getGlobalTop();
    drag_entry(entry_6, drag_pos_1, true);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2, entry_3, entry_4, entry_6, entry_5 }));
    T_WRAP_CONTAINER(check_entries(entry_4, { entry_4_1, entry_4_2 }));
    T_WRAP_CONTAINER(check_entries(entry_5, { entry_5_1, entry_5_2 }));
    T_WRAP_CONTAINER(check_entries(entry_6, { entry_6_1, entry_6_2 }));

    // drag entry 6 before the first child of entry 4
    entry_4->expand();
    application.advance();
    sf::Vector2f drag_pos_2 = entry_4_1->getRectangleWidget()->getGlobalTop();
    drag_entry(entry_6, drag_pos_2, true);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2, entry_3, entry_4, entry_5 }));
    T_WRAP_CONTAINER(check_entries(entry_4, { entry_6, entry_4_1, entry_4_2 }));
    T_WRAP_CONTAINER(check_entries(entry_5, { entry_5_1, entry_5_2 }));
    T_WRAP_CONTAINER(check_entries(entry_6, { entry_6_1, entry_6_2 }));
}

// same as treeviewWidgetDrag3Test but with a scroll area
void WidgetTestsTreeView::treeviewWidgetDrag4Test(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(200.0f, 200.0f);
    sf::Vector2f position(100.0f, 100.0f);
	fw::ScrollAreaWidget* scroll_area_widget = application.getWidgets().createScrollAreaWidget(size);
    scroll_area_widget->setPosition(position);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    tree_view_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    tree_view_widget->setParent(scroll_area_widget);
    scroll_area_widget->setScrolledWidget(tree_view_widget);

    auto drag_entry = [&](fw::TreeViewEntry* entry, const sf::Vector2f& pos, bool drop = false) {
        dragEntry(application, entry, pos, drop);
    };
    auto check_top_entries = [&](const std::initializer_list<fw::TreeViewEntry*>& entries) {
        checkTopEntries(test, tree_view_widget, entries);
    };
    auto check_entries = [&](fw::TreeViewEntry* entry, const std::initializer_list<fw::TreeViewEntry*> entries) {
        checkEntries(test, entry, entries);
    };

    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_2 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewEntry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewEntry* entry_4 = tree_view_widget->addEntry("Entry 4");
    fw::TreeViewEntry* entry_4_1 = tree_view_widget->addEntry("Entry 4_1");
    fw::TreeViewEntry* entry_4_2 = tree_view_widget->addEntry("Entry 4_2");
    entry_4_1->setParent(entry_4);
    entry_4_2->setParent(entry_4);
    fw::TreeViewEntry* entry_5 = tree_view_widget->addEntry("Entry 5");
    fw::TreeViewEntry* entry_5_1 = tree_view_widget->addEntry("Entry 5_1");
    fw::TreeViewEntry* entry_5_2 = tree_view_widget->addEntry("Entry 5_2");
    entry_5_1->setParent(entry_5);
    entry_5_2->setParent(entry_5);
    fw::TreeViewEntry* entry_6 = tree_view_widget->addEntry("Entry 6");
    fw::TreeViewEntry* entry_6_1 = tree_view_widget->addEntry("Entry 6_1");
    fw::TreeViewEntry* entry_6_2 = tree_view_widget->addEntry("Entry 6_2");
    entry_6_1->setParent(entry_6);
    entry_6_2->setParent(entry_6);
    application.advance();
    T_COMPARE(tree_view_widget->getAllEntryCount(), 12);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2, entry_3, entry_4, entry_5, entry_6 }));
    T_WRAP_CONTAINER(check_entries(entry_4, { entry_4_1, entry_4_2 }));
    T_WRAP_CONTAINER(check_entries(entry_5, { entry_5_1, entry_5_2 }));
    T_WRAP_CONTAINER(check_entries(entry_6, { entry_6_1, entry_6_2 }));

    // drag entry 6 in between entry 4 and entry 5
    sf::Vector2f drag_pos_1 = entry_5->getRectangleWidget()->getGlobalTop();
    drag_entry(entry_6, drag_pos_1, true);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2, entry_3, entry_4, entry_6, entry_5 }));
    T_WRAP_CONTAINER(check_entries(entry_4, { entry_4_1, entry_4_2 }));
    T_WRAP_CONTAINER(check_entries(entry_5, { entry_5_1, entry_5_2 }));
    T_WRAP_CONTAINER(check_entries(entry_6, { entry_6_1, entry_6_2 }));

    // drag entry 6 before the first child of entry 4
    entry_4->expand();
    application.advance();
    sf::Vector2f drag_pos_2 = entry_4_1->getRectangleWidget()->getGlobalTop();
    drag_entry(entry_6, drag_pos_2, true);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2, entry_3, entry_4, entry_5 }));
    T_WRAP_CONTAINER(check_entries(entry_4, { entry_6, entry_4_1, entry_4_2 }));
    T_WRAP_CONTAINER(check_entries(entry_5, { entry_5_1, entry_5_2 }));
    T_WRAP_CONTAINER(check_entries(entry_6, { entry_6_1, entry_6_2 }));
}

void WidgetTestsTreeView::treeviewWidgetDrag5Test(test::Test& test) {
    fw::Application application(getWindow());
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(getFont());
    application.start(true);
    application.advance();
    sf::Vector2f size(200.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewEntry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewEntry* entry_4 = tree_view_widget->addEntry("Entry 4");
    fw::Widget* entry_1_widget = entry_1->getWidget();
    fw::Widget* entry_2_widget = entry_2->getWidget();
    fw::Widget* entry_3_widget = entry_3->getWidget();
    fw::Widget* entry_4_widget = entry_4->getWidget();
    fw::Widget* target_highlight_widget = tree_view_widget->getTargetHighlightWidget();
    application.advance();

    auto check_top_entries = [&](const std::initializer_list<fw::TreeViewEntry*>& entries) {
        checkTopEntries(test, tree_view_widget, entries);
    };
    auto drag_entry = [&](fw::TreeViewEntry* entry, const sf::Vector2f& pos, bool drop = false) {
        dragEntry(application, entry, pos, drop);
    };

    // drop entry 1 between entries 3 and 4
    // not using drag_entry because entries shift positions when entry 1 is taken
    sf::Vector2f entry_1_center = entry_1_widget->getGlobalCenter();
    application.mouseMove(entry_1_center);
    application.mouseLeftPress();
    application.advance();
    // taking entry 1
    application.mouseMove(entry_1_center + sf::Vector2f(fw::TREEVIEW_ENTRY_DRAG_DISTANCE + 1.0f, 0.0f));
    application.advance();
    // dropping it to the current entry 4 top
    sf::Vector2f entry_4_top = entry_4_widget->getGlobalTop();
    application.mouseMove(entry_4_top);
    application.advance();
    application.advance();
    application.mouseLeftRelease();
    application.advance();

    T_COMPARE(tree_view_widget->getAllEntryCount(), 4);
    T_WRAP_CONTAINER(check_top_entries({ entry_2, entry_3, entry_1, entry_4 }));
}

sf::RenderWindow& WidgetTestsTreeView::getWindow() {
    return dynamic_cast<WidgetTests*>(parent)->window;
}

fw::Font& WidgetTestsTreeView::getFont() {
    return dynamic_cast<WidgetTests*>(parent)->textbox_font;
}

float WidgetTestsTreeView::calcTreeViewEntryHeight(fw::TreeViewEntry* entry) {
    float result = 0.0f;
    result += fw::TREEVIEW_ENTRY_HEIGHT;
    if (entry->isExpanded()) {
        for (size_t i = 0; i < entry->getChildrenCount(); i++) {
            result += fw::TREEVIEW_CONTAINER_PADDING;
            fw::TreeViewEntry* child = entry->getChild(i);
            float child_height = calcTreeViewEntryHeight(child);
            result += child_height;
        }
    }
    return result;
}

float WidgetTestsTreeView::calcTreeViewHeight(fw::TreeViewWidget* treeview) {
    float result = 0.0f;
    if (treeview->getChildrenCount() == 0) {
        return fw::TREEVIEW_CONTAINER_PADDING;
    }
    result += fw::TREEVIEW_CONTAINER_PADDING;
    for (size_t i = 0; i < treeview->getTopEntryCount(); i++) {
        fw::TreeViewEntry* entry = treeview->getTopEntry(i);
        float entry_height = calcTreeViewEntryHeight(entry);
        result += entry_height;
        if (i < treeview->getTopEntryCount() - 1) {
            result += fw::TREEVIEW_CONTAINER_PADDING;
        }
    }
    result += fw::TREEVIEW_CONTAINER_PADDING;
    return result;
}

void WidgetTestsTreeView::checkTopEntries(test::Test& test, fw::TreeViewWidget* tree_view_widget, const std::initializer_list<fw::TreeViewEntry*>& entries) {
    std::vector<fw::TreeViewEntry*> entries_vec = std::vector<fw::TreeViewEntry*>(entries);
    if (T_COMPARE(tree_view_widget->getTopEntryCount(), entries.size()) && T_COMPARE(tree_view_widget->getChildrenCount(), entries.size() + 1)) {
        for (size_t i = 0; i < entries_vec.size(); i++) {
            fw::TreeViewEntry* entry = tree_view_widget->getTopEntry(i);
            T_CHECK(entry == entries_vec[i]);
            T_CHECK(entry->getWidget() == entries_vec[i]->getWidget());
        }
    }
}

void WidgetTestsTreeView::checkEntries(test::Test& test, fw::TreeViewEntry* entry, const std::initializer_list<fw::TreeViewEntry*> entries) {
    std::vector<fw::TreeViewEntry*> entries_vec = std::vector<fw::TreeViewEntry*>(entries);
    fw::Widget* children_widget = entry->getChildrenWidget();
    if (T_COMPARE(entry->getChildrenCount(), entries_vec.size()) && T_COMPARE(children_widget->getChildrenCount(), entries.size())) {
        for (size_t i = 0; i < entries_vec.size(); i++) {
            T_CHECK(entry->getChild(i) == entries_vec[i]);
            T_CHECK(children_widget->getChild(i) == entries_vec[i]->getWidget());
        }
    }
}

void WidgetTestsTreeView::dragEntry(fw::Application& application, fw::TreeViewEntry* entry, const sf::Vector2f& pos, bool drop) {
	sf::Vector2f entry_rect_center = entry->getRectangleWidget()->getGlobalCenter();
	application.mouseMove(entry_rect_center);
	application.mouseLeftPress();
	application.advance();
	application.mouseMove(pos);
	application.advance();
	application.advance();
    if (drop) {
        application.mouseLeftRelease();
        application.advance();
    }
}
