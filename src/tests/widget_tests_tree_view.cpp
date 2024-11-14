#include "tests/widget_tests.h"

void WidgetTests::treeviewWidgetBasicTest(test::Test& test) {
    fw::Application application(window);
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
    T_WRAP_CONTAINER(genericWidgetTest(gwt));
}

void WidgetTests::treeviewWidgetEntriesTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
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

void WidgetTests::treeviewWidgetParent1Test(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
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

void WidgetTests::treeviewWidgetParent2Test(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
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

void WidgetTests::treeviewWidgetSelectTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
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

void WidgetTests::treeviewWidgetReorderTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
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

void WidgetTests::treeviewWidgetRemoveTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
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

void WidgetTests::treeviewWidgetClearTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
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

void WidgetTests::treeviewWidgetDrag1Test(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
    application.start(true);
    application.advance();
    sf::Vector2f size(200.0f, 100.0f);
    fw::TreeViewWidget* tree_view_widget = application.getWidgets().createTreeViewWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    tree_view_widget->setPosition(position);

    fw::TreeViewEntry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewEntry* entry_2 = tree_view_widget->addEntry("Entry 2");
    application.advance();

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
    if (T_COMPARE(tree_view_widget->getTopEntryCount(), 2)) {
        T_CHECK(tree_view_widget->getTopEntry(0) == entry_1);
        T_CHECK(tree_view_widget->getTopEntry(1) == entry_2);
    }
    if (T_COMPARE(tree_view_widget->getChildrenCount(), 3)) {
        T_CHECK(tree_view_widget->getChild(0) == entry_1_widget);
        T_CHECK(tree_view_widget->getChild(1) == entry_2_widget);
    }

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
    if (T_COMPARE(tree_view_widget->getTopEntryCount(), 2)) {
        T_CHECK(tree_view_widget->getTopEntry(0) == entry_2);
        T_CHECK(tree_view_widget->getTopEntry(1) == entry_1);
    }
    if (T_COMPARE(tree_view_widget->getChildrenCount(), 3)) {
        T_CHECK(tree_view_widget->getChild(0) == entry_2_widget);
        T_CHECK(tree_view_widget->getChild(1) == entry_1_widget);
    }
}

void WidgetTests::treeviewWidgetDrag2Test(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
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
    auto drag_widget = [&](fw::Widget* widget, const sf::Vector2f& pos) {
        application.mouseMove(widget->getGlobalCenter());
        application.mouseLeftPress();
        application.advance();
        application.mouseMove(pos);
        application.advance();
    };
    application.advance();
    auto check_top_entries = [&](const std::initializer_list<fw::TreeViewEntry*>& entries) {
        std::vector<fw::TreeViewEntry*> entries_vec = std::vector<fw::TreeViewEntry*>(entries);
        if (T_COMPARE(tree_view_widget->getTopEntryCount(), entries_vec.size())) {
            for (size_t i = 0; i < entries_vec.size(); i++) {
                T_CHECK(tree_view_widget->getTopEntry(i) == entries_vec[i]);
            }
        }
    };
    auto check_top_widgets = [&](const std::initializer_list<fw::Widget*> widgets) {
        std::vector<fw::Widget*> widgets_vec = std::vector<fw::Widget*>(widgets);
        if (T_COMPARE(tree_view_widget->getChildrenCount(), widgets_vec.size() + 1)) { // + 1 because of target highlight widget
            for (size_t i = 0; i < widgets_vec.size(); i++) {
                T_CHECK(tree_view_widget->getChild(i) == widgets_vec[i]);
            }
        }
    };
    auto check_entries = [&](fw::TreeViewEntry* entry, const std::initializer_list<fw::TreeViewEntry*> entries) {
        std::vector<fw::TreeViewEntry*> entries_vec = std::vector<fw::TreeViewEntry*>(entries);
        if (T_COMPARE(entry->getChildrenCount(), entries_vec.size())) {
            for (size_t i = 0; i < entries_vec.size(); i++) {
                T_CHECK(entry->getChild(i) == entries_vec[i]);
            }
        }
    };
    auto check_widgets = [&](fw::TreeViewEntry* entry, const std::initializer_list<fw::Widget*> widgets) {
        std::vector<fw::Widget*> widgets_vec = std::vector<fw::Widget*>(widgets);
        fw::Widget* children_widget = entry->getChildrenWidget();
        if (T_COMPARE(children_widget->getChildrenCount(), widgets_vec.size())) {
            for (size_t i = 0; i < widgets_vec.size(); i++) {
                T_CHECK(children_widget->getChild(i) == widgets_vec[i]);
            }
        }
    };

    // drop to the top
    sf::Vector2f drag_top = tree_view_widget->getTop() + sf::Vector2f(0.0f, -50.0f);
    drag_widget(entry_1_widget, drag_top);
    application.advance();
    T_VEC2_COMPARE(target_highlight_widget->getGlobalOriginPosition(), entry_2_widget->getGlobalPosition());
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(!target_highlight_widget->isVisible());
    T_COMPARE(tree_view_widget->getAllEntryCount(), 3);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2 }));
    T_WRAP_CONTAINER(check_top_widgets({ entry_1_widget, entry_2_widget }));

    // drop to the bottom
    T_ASSERT_NO_ERRORS();
    sf::Vector2f drag_bottom = tree_view_widget->getBottom() + sf::Vector2f(0.0f, 50.0f);
    drag_widget(entry_1_widget, drag_bottom);
    application.advance();
    T_VEC2_COMPARE(target_highlight_widget->getGlobalPosition(), entry_2_widget->getGlobalBottomLeft());
    application.mouseLeftRelease();
    application.advance();

    // drop inside
    T_ASSERT_NO_ERRORS();
    entry_2->expand();
    application.advance();
    sf::Vector2f drag_inside = entry_3_widget->getGlobalTop();
    drag_widget(entry_1_widget, drag_inside);
    application.advance();
    T_VEC2_COMPARE(target_highlight_widget->getGlobalOriginPosition(), entry_3_widget->getGlobalPosition());
    application.mouseLeftRelease();
    application.advance();
    T_COMPARE(tree_view_widget->getAllEntryCount(), 3);
    T_WRAP_CONTAINER(check_top_entries({ entry_2 }));
    T_WRAP_CONTAINER(check_entries(entry_2, { entry_1, entry_3 }));
    T_WRAP_CONTAINER(check_top_widgets({ entry_2_widget }));
    T_WRAP_CONTAINER(check_widgets(entry_2, { entry_1_widget, entry_3_widget }));

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
    T_WRAP_CONTAINER(check_top_widgets({ entry_2_widget }));
    T_WRAP_CONTAINER(check_widgets(entry_2, { entry_1_widget, entry_3_widget }));

    // drop outside
    T_ASSERT_NO_ERRORS();
    drag_widget(entry_1_widget, drag_top);
    application.advance();
    application.mouseLeftRelease();
    application.advance();
    T_COMPARE(tree_view_widget->getAllEntryCount(), 3);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2 }));
    T_WRAP_CONTAINER(check_top_widgets({ entry_1_widget, entry_2_widget }));

    // drop outside the only child
    T_ASSERT_NO_ERRORS();
    drag_widget(entry_3_widget, drag_bottom);
    application.advance();
    application.mouseLeftRelease();
    application.advance();
    T_COMPARE(tree_view_widget->getAllEntryCount(), 3);
    T_WRAP_CONTAINER(check_top_entries({ entry_1, entry_2, entry_3 }));
    T_WRAP_CONTAINER(check_top_widgets({ entry_1_widget, entry_2_widget, entry_3_widget }));
}
