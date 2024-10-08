#include "tests/widget_tests.h"
#include <common/utils.h>

#define PRESS_MOUSE_LEFT(pos) \
    application.mouseMove(pos); \
    application.mouseLeftPress(); \
    application.advance();

#define RELEASE_MOUSE_LEFT() \
    application.mouseLeftRelease(); \
    application.advance();

#define CLICK_MOUSE(pos) \
    PRESS_MOUSE_LEFT(pos); \
    RELEASE_MOUSE_LEFT();

#define MOVE_MOUSE(pos) \
    application.mouseMove(pos); \
    application.advance();

#define PRESS_KEY(key) \
    application.keyPress(key); \
    application.advance();

#define RELEASE_KEY(key) \
    application.keyRelease(key); \
    application.advance();

#define TAP_KEY(key) \
    PRESS_KEY(key) \
    RELEASE_KEY(key)

#define ENTER_TEXT(key, code) \
    application.keyPress(key); \
    application.textEntered(code); \
    application.advance(); \
    application.keyRelease(key); \
    application.advance();

#define SELECT_ALL() \
    application.keyPress(sf::Keyboard::LControl); \
    application.keyPress(sf::Keyboard::A); \
    application.advance(); \
    application.keyRelease(sf::Keyboard::A); \
    application.keyRelease(sf::Keyboard::LControl); \
    application.advance();

#define COPY() \
    PRESS_KEY(sf::Keyboard::LControl); \
    ENTER_TEXT(sf::Keyboard::C, 'c'); \
    RELEASE_KEY(sf::Keyboard::LControl);

#define PASTE() \
    PRESS_KEY(sf::Keyboard::LControl); \
    ENTER_TEXT(sf::Keyboard::V, 'v'); \
    RELEASE_KEY(sf::Keyboard::LControl);

#define CUT() \
    PRESS_KEY(sf::Keyboard::LControl); \
    ENTER_TEXT(sf::Keyboard::X, 'x'); \
    RELEASE_KEY(sf::Keyboard::LControl);

#define _CHECK_SELECTION(active, text, cursor_pos, left, right) \
    T_CHECK(textbox_widget->isSelectionActive() == active); \
    T_COMPARE(textbox_widget->getSelectedText(), text); \
    T_COMPARE(textbox_widget->getCursorPos(), cursor_pos); \
    T_COMPARE(textbox_widget->getSelectionLeft(), left); \
    T_COMPARE(textbox_widget->getSelectionRight(), right); \

#define CHECK_SELECTION(active, text, cursor_pos, left, right) \
    T_WRAP_CONTAINER(_CHECK_SELECTION(active, text, cursor_pos, left, right))

WidgetTests::WidgetTests(test::TestManager& manager, const std::vector<TestModule*>& required_modules) : TestModule("Widgets", manager, required_modules) { }

void WidgetTests::createTestLists() {
    test::TestList* toposort_list = createTestList("Toposort");
    test::Test* empty_test = toposort_list->addTest("empty", [&](test::Test& test) { toposortEmptyTest(test); });
    test::Test* one_node_test = toposort_list->addTest("one_node", { empty_test }, [&](test::Test& test) { toposort1NodeTest(test); });
    test::Test* three_nodes_test = toposort_list->addTest("three_nodes", { one_node_test }, [&](test::Test& test) { toposort3NodesTest(test); });
    test::Test* four_nodes_diamond_test = toposort_list->addTest("four_nodes_diamond", { three_nodes_test }, [&](test::Test& test) { toposort4NodesDiamondTest(test); });
    test::Test* five_nodes_x_test = toposort_list->addTest("five_nodes_x", { three_nodes_test }, [&](test::Test& test) { toposort5NodesXTest(test); });
    test::Test* five_nodes_random_test = toposort_list->addTest("five_nodes_random", { three_nodes_test }, [&](test::Test& test) { toposort5NodesRandomTest(test); });
    test::Test* hair_test = toposort_list->addTest("hair", { three_nodes_test }, [&](test::Test& test) { toposortHairTest(test); });
    test::Test* inverse_hair_test = toposort_list->addTest("inverse_hair", { three_nodes_test }, [&](test::Test& test) { toposortInverseHairTest(test); });
    test::Test* loop_exception_test = toposort_list->addTest("loop_exception", { three_nodes_test }, [&](test::Test& test) { toposortLoopExceptionTest(test); });
    test::Test* loop_event_test = toposort_list->addTest("loop_event", { three_nodes_test }, [&](test::Test& test) { toposortLoopEventTest(test); });
    test::Test* two_loops_test = toposort_list->addTest("two_loops", { loop_event_test }, [&](test::Test& test) { toposortTwoLoopsTest(test); });
    test::Test* forking_loop_test = toposort_list->addTest("forking_loop", { loop_event_test }, [&](test::Test& test) { toposortForkingLoopTest(test); });
    test::Test* triangles_loop_test = toposort_list->addTest("triangles_loop", { forking_loop_test }, [&](test::Test& test) { toposortTrianglesLoopTest(test); });

    test::TestList* application_list = createTestList("Application", { toposort_list });
    test::Test* basic_test = application_list->addTest("basic", [&](test::Test& test) { basicTest(test); });
    test::Test* init_test = application_list->addTest("init", { basic_test }, [&](test::Test& test) { initTest(test); });
    test::Test* start_test = application_list->addTest("start", { init_test }, [&](test::Test& test) { startTest(test); });
    test::Test* advance_test = application_list->addTest("advance", { start_test }, [&](test::Test& test) { advanceTest(test); });
    test::Test* close_test = application_list->addTest("close", { start_test }, [&](test::Test& test) { closeTest(test); });
    test::Test* mouse_events_test = application_list->addTest("mouse_events", { advance_test }, [&](test::Test& test) { mouseEventsTest(test); });
    test::Test* keyboard_events_test = application_list->addTest("keyboard_events", { advance_test }, [&](test::Test& test) { keyboardEventsTest(test); });

    test::TestList* widgets_basic_list = createTestList("WidgetsBasic", { application_list });
    test::Test* root_widget_test = widgets_basic_list->addTest("root_widget", [&](test::Test& test) { rootWidgetTest(test); });
    test::Test* empty_widget_test = widgets_basic_list->addTest("empty_widget", { root_widget_test }, [&](test::Test& test) { emptyWidgetTest(test); });
    test::Test* rectangle_widget_test = widgets_basic_list->addTest("rectangle_widget", { root_widget_test }, [&](test::Test& test) { rectangleWidgetTest(test); });
    test::Test* polygon_widget_basic_test = widgets_basic_list->addTest("polygon_widget_basic", { root_widget_test }, [&](test::Test& test) { polygonWidgetBasicTest(test); });
    test::Test* set_parent_test = widgets_basic_list->addTest("set_parent", { root_widget_test }, [&](test::Test& test) { setParentTest(test); });
    test::Test* widget_mouse_events_1_test = widgets_basic_list->addTest("mouse_events_1", { root_widget_test }, [&](test::Test& test) { widgetMouseEvents1(test); });
    test::Test* widget_mouse_events_2_test = widgets_basic_list->addTest("mouse_events_2", { root_widget_test }, [&](test::Test& test) { widgetMouseEvents2(test); });
    test::Test* drag_gesture_event_test = widgets_basic_list->addTest("drag_gesture_event", { root_widget_test }, [&](test::Test& test) { dragGestureEventTest(test); });
    test::Test* events_test = widgets_basic_list->addTest("events", { root_widget_test }, [&](test::Test& test) { eventsTest(test); });
    test::Test* coordinates_test = widgets_basic_list->addTest("coordinates", { set_parent_test }, [&](test::Test& test) { coordinatesTest(test); });
    test::Test* find_test = widgets_basic_list->addTest("find", { set_parent_test }, [&](test::Test& test) { findTest(test); });
    test::Test* anchor_test = widgets_basic_list->addTest("anchor", { set_parent_test }, [&](test::Test& test) { anchorTest(test); });
    test::Test* remove_1_test = widgets_basic_list->addTest("remove_1", { set_parent_test }, [&](test::Test& test) { remove1Test(test); });
    test::Test* remove_2_test = widgets_basic_list->addTest("remove_2", { set_parent_test }, [&](test::Test& test) { remove2Test(test); }); 

    test::TestList* text_widget_list = createTestList("TextWidget", { application_list });
    test::Test* text_widget_basic_test = text_widget_list->addTest("basic", [&](test::Test& test) { textWidgetTest(test); });
    test::Test* text_widget_default_font_test = text_widget_list->addTest("default_font", { text_widget_basic_test }, [&](test::Test& test) { textWidgetDefaultFontTest(test); });

    test::TestList* checkbox_widget_list = createTestList("CheckboxWidget", { widgets_basic_list });
    test::Test* checkbox_widget_basic_test = checkbox_widget_list->addTest("basic", [&](test::Test& test) { checkboxWidgetBasicTest(test); });
    test::Test* checkbox_widget_toggle_test = checkbox_widget_list->addTest("toggle", { checkbox_widget_basic_test }, [&](test::Test& test) { checkboxWidgetToggleTest(test); });

    test::TestList* container_widget_list = createTestList("ContainerWidget", { widgets_basic_list });
    test::Test* container_widget_basic_test = container_widget_list->addTest("basic", [&](test::Test& test) { containerWidgetBasicTest(test); });
    test::Test* container_widget_children_test = container_widget_list->addTest("children", { container_widget_basic_test }, [&](test::Test& test) { containerWidgetChildrenTest(test); });
    test::Test* container_widget_padding_test = container_widget_list->addTest("padding", { container_widget_children_test }, [&](test::Test& test) { containerWidgetPaddingTest(test); });

    test::TestList* size_policy_list = createTestList("SizePolicy", { widgets_basic_list });
    test::Test* size_policy_basic_test = size_policy_list->addTest("basic", [&](test::Test& test) { sizePolicyTest(test); });
    test::Test* size_policy_position_test = size_policy_list->addTest("position", [&](test::Test& test) { sizePolicyPositionTest(test); });
    test::Test* size_policy_expand_test = size_policy_list->addTest("expand", [&](test::Test& test) { sizePolicyExpandTest(test); });
    test::Test* size_policy_limits_test = size_policy_list->addTest("limits", { size_policy_expand_test }, [&](test::Test& test) { sizePolicyLimitsTest(test); });
    std::vector<test::Test*> size_policy_tests = { size_policy_basic_test, size_policy_position_test, size_policy_expand_test, size_policy_limits_test };
    test::Test* size_policy_combined_test = size_policy_list->addTest("combined", size_policy_tests, [&](test::Test& test) { sizePolicyCombinedTest(test); });

    test::TestList* widget_link_list = createTestList("WidgetLink", { widgets_basic_list, size_policy_list });
    test::Test* widget_link_basic_test = widget_link_list->addTest("basic", [&](test::Test& test) { widgetLinkBasicTest(test); });
    test::Test* widget_link_targets_func_test = widget_link_list->addTest("target_func", [&](test::Test& test) { widgetLinkTargetsFuncTest(test); });
    test::Test* widget_link_container_test = widget_link_list->addTest("container", { widget_link_basic_test }, [&](test::Test& test) { widgetLinkContainerTest(test); });
    test::Test* widget_link_remove_test = widget_link_list->addTest("remove", { widget_link_basic_test }, [&](test::Test& test) { widgetLinkRemoveTest(test); });

    test::TestList* textbox_widget_list = createTestList("TextBoxWidget", { widgets_basic_list, text_widget_list });
    test::Test* textbox_widget_basic_test = textbox_widget_list->addTest("basic", [&](test::Test& test) { textboxWidgetBasicTest(test); });
    test::Test* textbox_widget_input_test = textbox_widget_list->addTest("input", { textbox_widget_basic_test }, [&](test::Test& test) { textboxWidgetInputTest(test); });
    test::Test* textbox_widget_events_test = textbox_widget_list->addTest("events", { textbox_widget_input_test }, [&](test::Test& test) { textboxWidgetEventsTest(test); });
    test::Test* textbox_widget_cursor_test = textbox_widget_list->addTest("cursor", { textbox_widget_basic_test }, [&](test::Test& test) { textboxWidgetCursorTest(test); });
    test::Test* textbox_widget_scroll_test = textbox_widget_list->addTest("scroll", { textbox_widget_cursor_test }, [&](test::Test& test) { textboxWidgetScrollTest(test); });
    test::Test* textbox_widget_resizing_test = textbox_widget_list->addTest("resizing", { textbox_widget_scroll_test }, [&](test::Test& test) { textboxWidgetResizingTest(test); });
    test::Test* textbox_widget_selection_test = textbox_widget_list->addTest("selection", { textbox_widget_cursor_test }, [&](test::Test& test) { textboxWidgetSelectionTest(test); });
    test::Test* textbox_widget_mouse_click_test = textbox_widget_list->addTest("mouse_click", { textbox_widget_cursor_test }, [&](test::Test& test) { textboxWidgetMouseClickTest(test); });
    test::Test* textbox_widget_mouse_drag_test = textbox_widget_list->addTest("mouse_drag", { textbox_widget_mouse_click_test }, [&](test::Test& test) { textboxWidgetMouseDragTest(test); });
    test::Test* textbox_widget_copypaste_test = textbox_widget_list->addTest("copypaste", { textbox_widget_selection_test }, [&](test::Test& test) { textboxWidgetCopyPasteTest(test); });
    test::Test* textbox_widget_history_test = textbox_widget_list->addTest("history", { textbox_widget_copypaste_test }, [&](test::Test& test) { textboxWidgetHistoryTest(test); });
    test::Test* textbox_widget_integer_test = textbox_widget_list->addTest("integer", { textbox_widget_copypaste_test }, [&](test::Test& test) { textboxWidgetIntegerTest(test); });
    test::Test* textbox_widget_float_test = textbox_widget_list->addTest("float", { textbox_widget_copypaste_test }, [&](test::Test& test) { textboxWidgetFloatTest(test); });

    test::TestList* canvas_widget_list = createTestList("CanvasWidget", { widgets_basic_list });
    test::Test* canvas_widget_basic_test = canvas_widget_list->addTest("basic", [&](test::Test& test) { canvasWidgetBasicTest(test); });
    test::Test* canvas_widget_draw_test = canvas_widget_list->addTest("draw", { canvas_widget_basic_test }, [&](test::Test& test) { canvasWidgetDrawTest(test); });
    test::Test* canvas_widget_alpha_test = canvas_widget_list->addTest("alpha", { canvas_widget_basic_test }, [&](test::Test& test) { canvasWidgetAlphaTest(test); });

    test::TestList* window_widget_list = createTestList("WindowWidget", { widgets_basic_list, text_widget_list });
    test::Test* window_widget_basic_test = window_widget_list->addTest("basic", [&](test::Test& test) { windowWidgetBasicTest(test); });
    test::Test* window_widget_drag_test = window_widget_list->addTest("drag", { window_widget_basic_test }, [&](test::Test& test) { windowWidgetDragTest(test); });
    test::Test* window_widget_children_test = window_widget_list->addTest("children", { window_widget_basic_test }, [&](test::Test& test) { windowWidgetChildrenTest(test); });
    test::Test* window_widget_resize_test = window_widget_list->addTest("resize", { window_widget_basic_test }, [&](test::Test& test) { windowWidgetResizeTest(test); });
    test::Test* window_widget_chain_test = window_widget_list->addTest("chain", { window_widget_children_test, window_widget_resize_test }, [&](test::Test& test) { windowWidgetChainTest(test); });
    test::Test* window_widget_drag_limits_test = window_widget_list->addTest("drag_limits", { window_widget_drag_test, window_widget_chain_test }, [&](test::Test& test) { windowWidgetDragLimitsTest(test); });
    test::Test* window_widget_resize_limits_test = window_widget_list->addTest("resize_limits", { window_widget_chain_test }, [&](test::Test& test) { windowWidgetResizeLimitsTest(test); });
    test::Test* window_widget_move_to_top_drag_test = window_widget_list->addTest("move_to_top_drag", { window_widget_drag_test }, [&](test::Test& test) { windowWidgetMoveToTopDragTest(test); });
    test::Test* window_widget_move_to_top_resize_test = window_widget_list->addTest("move_to_top_resize", { window_widget_drag_test, window_widget_resize_test }, [&](test::Test& test) { windowWidgetMoveToTopResizeTest(test); });

    test::TestList* dropdown_widget_list = createTestList("DropdownWidget", { widgets_basic_list, text_widget_list });
    test::Test* dropdown_widget_basic_test = dropdown_widget_list->addTest("basic", [&](test::Test& test) { dropdownWidgetBasicTest(test); });
    test::Test* dropdown_widget_options_1_test = dropdown_widget_list->addTest("options_1", { dropdown_widget_basic_test }, [&](test::Test& test) { dropdownWidgetOptions1Test(test); });
    test::Test* dropdown_widget_options_2_test = dropdown_widget_list->addTest("options_2", { dropdown_widget_options_1_test }, [&](test::Test& test) { dropdownWidgetOptions2Test(test); });

    test::TestList* scroll_area_widget_list = createTestList("ScrollAreaWidget", { widgets_basic_list });
    test::Test* scroll_area_widget_basic_test = scroll_area_widget_list->addTest("basic", [&](test::Test& test) { scrollAreaWidgetBasicTest(test); });
    test::Test* scroll_area_widget_scroll_test = scroll_area_widget_list->addTest("scroll", { scroll_area_widget_basic_test }, [&](test::Test& test) { scrollAreaWidgetScrollTest(test); });
    test::Test* scroll_area_widget_scrollbar_visibility_test = scroll_area_widget_list->addTest("scrollbar_visibility", { scroll_area_widget_basic_test }, [&](test::Test& test) { scrollAreaWidgetScrollbarVisibilityTest(test); });
    test::Test* scroll_area_widget_scrollbar_container_test = scroll_area_widget_list->addTest("scrollbar_container", { scroll_area_widget_scrollbar_visibility_test }, [&](test::Test& test) { scrollAreaWidgetScrollbarContainerTest(test); });

    test::TestList* tree_view_widget_list = createTestList("TreeViewWidget", { widgets_basic_list });
    test::Test* tree_view_widget_basic_test = tree_view_widget_list->addTest("basic", [&](test::Test& test) { treeviewWidgetBasicTest(test); });
    test::Test* tree_view_widget_entries_test = tree_view_widget_list->addTest("entries", { tree_view_widget_basic_test }, [&](test::Test& test) { treeviewWidgetEntriesTest(test); });
    test::Test* tree_view_widget_parent1_test = tree_view_widget_list->addTest("parent_1", { tree_view_widget_entries_test }, [&](test::Test& test) { treeviewWidgetParent1Test(test); });
    test::Test* tree_view_widget_parent2_test = tree_view_widget_list->addTest("parent_2", { tree_view_widget_entries_test }, [&](test::Test& test) { treeviewWidgetParent2Test(test); });
    test::Test* tree_view_widget_select_test = tree_view_widget_list->addTest("select", { tree_view_widget_parent1_test }, [&](test::Test& test) { treeviewWidgetSelectTest(test); });
    test::Test* tree_view_widget_reorder_test = tree_view_widget_list->addTest("reorder", { tree_view_widget_parent2_test }, [&](test::Test& test) { treeviewWidgetReorderTest(test); });
    test::Test* tree_view_widget_remove_test = tree_view_widget_list->addTest("remove", { tree_view_widget_parent2_test }, [&](test::Test& test) { treeviewWidgetRemoveTest(test); });
    test::Test* tree_view_widget_clear_test = tree_view_widget_list->addTest("clear", { tree_view_widget_parent2_test }, [&](test::Test& test) { treeviewWidgetClearTest(test); });
}

void WidgetTests::beforeRunModule() {
    sf::ContextSettings cs_window;
    window.create(sf::VideoMode(800, 600), "Widget tests", sf::Style::Default, cs_window);
    textbox_font = fw::Font("fonts/verdana.ttf");
}

void WidgetTests::afterRunModule() {
    window.close();
}

void WidgetTests::toposortEmptyTest(test::Test& test) {
    NodeList list;
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        list.getNodes(), &Node::getParents
    );
    T_CHECK(sorted.size() == 0);
}

void WidgetTests::toposort1NodeTest(test::Test& test) {
    NodeList list;
    Node* node = list.createNode("A");
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        list.getNodes(), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 1)) {
        if (T_CHECK(sorted[0].size() == 1)) {
            T_CHECK(layer_contains(sorted[0], "A"));
        }
    }
}

void WidgetTests::toposort3NodesTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    nodeC->addParent(nodeA);
    nodeB->addParent(nodeC);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 3)) {
        if (T_CHECK(sorted[0].size() == 1)) {
            T_CHECK(layer_contains(sorted[0], "A"));
        }
        if (T_CHECK(sorted[1].size() == 1)) {
            T_CHECK(layer_contains(sorted[1], "C"));
        }
        if (T_CHECK(sorted[2].size() == 1)) {
            T_CHECK(layer_contains(sorted[2], "B"));
        }
    }
}

void WidgetTests::toposort4NodesDiamondTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    Node* nodeD = list.createNode("D");
    nodeB->addParent(nodeA);
    nodeC->addParent(nodeA);
    nodeD->addParent(nodeB);
    nodeD->addParent(nodeC);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 3)) {
        if (T_CHECK(sorted[0].size() == 1)) {
            T_CHECK(layer_contains(sorted[0], "A"));
        }
        if (T_CHECK(sorted[1].size() == 2)) {
            T_CHECK(layer_contains(sorted[1], "B"));
            T_CHECK(layer_contains(sorted[1], "C"));
        }
        if (T_CHECK(sorted[2].size() == 1)) {
            T_CHECK(layer_contains(sorted[2], "D"));
        }
    }
}

void WidgetTests::toposort5NodesXTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    Node* nodeD = list.createNode("D");
    Node* nodeE = list.createNode("E");
    nodeC->addParent(nodeA);
    nodeC->addParent(nodeB);
    nodeD->addParent(nodeC);
    nodeE->addParent(nodeC);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 3)) {
        if (T_CHECK(sorted[0].size() == 2)) {
            T_CHECK(layer_contains(sorted[0], "A"));
            T_CHECK(layer_contains(sorted[0], "B"));
        }
        if (T_CHECK(sorted[1].size() == 1)) {
            T_CHECK(layer_contains(sorted[1], "C"));
        }
        if (sorted[2].size() == 2) {
            T_CHECK(layer_contains(sorted[2], "D"));
            T_CHECK(layer_contains(sorted[2], "E"));
        }
    }
}

void WidgetTests::toposort5NodesRandomTest(test::Test& test) {
    NodeList list;
    Node* node0 = list.createNode("N0");
    Node* node1 = list.createNode("N1");
    Node* node2 = list.createNode("N2");
    Node* node3 = list.createNode("N3");
    Node* node4 = list.createNode("N4");
    Node* node5 = list.createNode("N5");
    node0->addParent(node2);
    node2->addParent(node3);
    node4->addParent(node0);
    node4->addParent(node5);
    node5->addParent(node2);
    node5->addParent(node1);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 4)) {
        if (T_CHECK(sorted[0].size() == 2)) {
            T_CHECK(layer_contains(sorted[0], "N1"));
            T_CHECK(layer_contains(sorted[0], "N3"));
        }
        if (T_CHECK(sorted[1].size() == 1)) {
            T_CHECK(layer_contains(sorted[1], "N2"));
        }
        if (T_CHECK(sorted[2].size() == 2)) {
            T_CHECK(layer_contains(sorted[2], "N0"));
            T_CHECK(layer_contains(sorted[2], "N5"));
        }
        if (T_CHECK(sorted[3].size() == 1)) {
            T_CHECK(layer_contains(sorted[3], "N4"));
        }
    }
}

void WidgetTests::toposortHairTest(test::Test& test) {
    NodeList list;
    Node* center = list.createNode("center");
    Node* short0 = list.createNode("short0");
    Node* short1 = list.createNode("short1");
    Node* short2 = list.createNode("short2");
    Node* long0 = list.createNode("long0");
    Node* long1 = list.createNode("long1");
    Node* long2 = list.createNode("long2");
    short0->addParent(center);
    short1->addParent(center);
    short2->addParent(center);
    long0->addParent(center);
    long1->addParent(long0);
    long2->addParent(long1);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 4)) {
        if (T_CHECK(sorted[0].size() == 1)) {
            T_CHECK(layer_contains(sorted[0], "center"));
        }
        if (T_CHECK(sorted[1].size() == 4)) {
            T_CHECK(layer_contains(sorted[1], "short0"));
            T_CHECK(layer_contains(sorted[1], "short1"));
            T_CHECK(layer_contains(sorted[1], "short2"));
            T_CHECK(layer_contains(sorted[1], "long0"));
        }
        if (T_CHECK(sorted[2].size() == 1)) {
            T_CHECK(layer_contains(sorted[2], "long1"));
        }
        if (T_CHECK(sorted[3].size() == 1)) {
            T_CHECK(layer_contains(sorted[3], "long2"));
        }
    }
}

void WidgetTests::toposortInverseHairTest(test::Test& test) {
    NodeList list;
    Node* center = list.createNode("center");
    Node* short0 = list.createNode("short0");
    Node* short1 = list.createNode("short1");
    Node* short2 = list.createNode("short2");
    Node* long0 = list.createNode("long0");
    Node* long1 = list.createNode("long1");
    Node* long2 = list.createNode("long2");
    center->addParent(short0);
    center->addParent(short1);
    center->addParent(short2);
    center->addParent(long0);
    long0->addParent(long1);
    long1->addParent(long2);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 4)) {
        if (T_CHECK(sorted[0].size() == 4)) {
            T_CHECK(layer_contains(sorted[0], "long2"));
            T_CHECK(layer_contains(sorted[0], "short0"));
            T_CHECK(layer_contains(sorted[0], "short1"));
            T_CHECK(layer_contains(sorted[0], "short2"));
        }
        if (T_CHECK(sorted[1].size() == 1)) {
            T_CHECK(layer_contains(sorted[1], "long1"));
        }
        if (T_CHECK(sorted[2].size() == 1)) {
            T_CHECK(layer_contains(sorted[2], "long0"));
        }
        if (T_CHECK(sorted[3].size() == 1)) {
            T_CHECK(layer_contains(sorted[3], "center"));
        }
    }
}

void WidgetTests::toposortLoopExceptionTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    nodeC->addParent(nodeA);
    nodeB->addParent(nodeC);
    nodeA->addParent(nodeB);
    bool exception = false;
    try {
        std::vector<std::vector<Node*>> sorted = fw::toposort(
            get_shuffled(list.getNodes()), &Node::getParents
        );
    } catch (std::exception exc) {
        exception = true;
    }
    T_CHECK(exception);
}

void WidgetTests::toposortLoopEventTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    nodeC->addParent(nodeA);
    nodeB->addParent(nodeC);
    nodeA->addParent(nodeB);
    std::vector<Node*> loop;
    std::function<void(const std::vector<Node*>&)> on_loop_detected = [&](const std::vector<Node*>& p_loop) {
        loop = p_loop;
    };
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents, &on_loop_detected
    );
    T_CHECK(sorted.empty());
    if (T_COMPARE(loop.size(), 3)) {
        T_CHECK(loop[0] == nodeB);
        T_CHECK(loop[1] == nodeC);
        T_CHECK(loop[2] == nodeA);
    }
}

void WidgetTests::toposortTwoLoopsTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    Node* nodeD = list.createNode("D");
    Node* nodeE = list.createNode("E");
    Node* nodeF = list.createNode("F");
    nodeA->addParent(nodeB);
    nodeB->addParent(nodeC);
    nodeC->addParent(nodeA);
    nodeD->addParent(nodeE);
    nodeE->addParent(nodeF);
    nodeF->addParent(nodeD);
    std::vector<std::vector<Node*>> loops;
    std::function<void(const std::vector<Node*>&)> on_loop_detected = [&](const std::vector<Node*>& p_loop) {
        loops.push_back(p_loop);
    };
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents, &on_loop_detected
    );
    T_CHECK(sorted.empty());
    if (T_COMPARE(loops.size(), 2)) {
        if (T_COMPARE(loops[0].size(), 3)) {
            T_CHECK(loops[0][0] == nodeF);
            T_CHECK(loops[0][1] == nodeD);
            T_CHECK(loops[0][2] == nodeE);
        }
        if (T_COMPARE(loops[1].size(), 3)) {
            T_CHECK(loops[1][0] == nodeB);
            T_CHECK(loops[1][1] == nodeC);
            T_CHECK(loops[1][2] == nodeA);
        }
    }
}

void WidgetTests::toposortForkingLoopTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeL1 = list.createNode("L1");
    Node* nodeL2 = list.createNode("L2");
    Node* nodeR1 = list.createNode("R1");
    Node* nodeR2 = list.createNode("R2");
    nodeA->addParent(nodeB);
    nodeB->addParent(nodeL1);
    nodeB->addParent(nodeR1);
    nodeL1->addParent(nodeL2);
    nodeR1->addParent(nodeR2);
    nodeL2->addParent(nodeB);
    nodeR2->addParent(nodeA);
    std::vector<std::vector<Node*>> loops;
    std::function<void(const std::vector<Node*>&)> on_loop_detected = [&](const std::vector<Node*>& p_loop) {
        loops.push_back(p_loop);
    };
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents, &on_loop_detected
    );
    T_CHECK(sorted.empty());
    if (T_COMPARE(loops.size(), 2)) {
        if (T_COMPARE(loops[0].size(), 3)) {
            T_CHECK(loops[0][0] == nodeB);
            T_CHECK(loops[0][1] == nodeL1);
            T_CHECK(loops[0][2] == nodeL2);
        }
        if (T_COMPARE(loops[1].size(), 4)) {
            T_CHECK(loops[1][0] == nodeR2);
            T_CHECK(loops[1][1] == nodeA);
            T_CHECK(loops[1][2] == nodeB);
            T_CHECK(loops[1][3] == nodeR1);
        }
    }
}

void WidgetTests::toposortTrianglesLoopTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    Node* nodeD = list.createNode("D");
    Node* nodeE = list.createNode("E");
    Node* nodeF = list.createNode("F");
    nodeA->addParent(nodeC);
    nodeB->addParent(nodeA);
    nodeB->addParent(nodeE);
    nodeC->addParent(nodeB);
    nodeC->addParent(nodeF);
    nodeD->addParent(nodeB);
    nodeE->addParent(nodeD);
    nodeE->addParent(nodeC);
    nodeF->addParent(nodeE);
    std::vector<std::vector<Node*>> loops;
    std::function<void(const std::vector<Node*>&)> on_loop_detected = [&](const std::vector<Node*>& p_loop) {
        loops.push_back(p_loop);
    };
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents, &on_loop_detected
    );
    T_CHECK(sorted.empty());
    if (T_COMPARE(loops.size(), 5)) {
        if (T_COMPARE(loops[0].size(), 3)) {
            T_CHECK(loops[0][0] == nodeB);
            T_CHECK(loops[0][1] == nodeA);
            T_CHECK(loops[0][2] == nodeC);
        }
        if (T_COMPARE(loops[1].size(), 6)) {
            T_CHECK(loops[1][0] == nodeE);
            T_CHECK(loops[1][1] == nodeD);
            T_CHECK(loops[1][2] == nodeB);
            T_CHECK(loops[1][3] == nodeA);
            T_CHECK(loops[1][4] == nodeC);
            T_CHECK(loops[1][5] == nodeF);
        }
        if (T_COMPARE(loops[2].size(), 3)) {
            T_CHECK(loops[2][0] == nodeE);
            T_CHECK(loops[2][1] == nodeD);
            T_CHECK(loops[2][2] == nodeB);
        }
        if (T_COMPARE(loops[3].size(), 3)) {
            T_CHECK(loops[3][0] == nodeE);
            T_CHECK(loops[3][1] == nodeC);
            T_CHECK(loops[3][2] == nodeB);
        }
        if (T_COMPARE(loops[4].size(), 3)) {
            T_CHECK(loops[4][0] == nodeE);
            T_CHECK(loops[4][1] == nodeC);
            T_CHECK(loops[4][2] == nodeF);
        }
    }
}

void WidgetTests::basicTest(test::Test& test) {
    fw::Application application(window);
}

void WidgetTests::initTest(test::Test& test) {
    TestApplication application(window);
    application.init(test.name, 800, 600, 0, false);
    T_ASSERT(T_CHECK(application.initialized));
    T_COMPARE(application.getWindowSize(), sf::Vector2u(800, 600), &WidgetTests::sfVec2uToStr);
}

void WidgetTests::startTest(test::Test& test) {
    TestApplication application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    T_ASSERT(T_CHECK(application.started));
}

void WidgetTests::advanceTest(test::Test& test) {
    TestApplication application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    T_CHECK(application.initialized);
    T_CHECK(application.started);
    T_CHECK(application.frame_begin);
    T_CHECK(application.frame_end);
    T_CHECK(application.process_widgets);
    T_CHECK(!application.process_window_event);
    T_CHECK(!application.process_keyboard_event);
    T_CHECK(!application.process_left_press);
    T_CHECK(!application.process_left_release);
    T_CHECK(!application.process_mouse_move);
    T_CHECK(!application.process_mouse_scroll_x);
    T_CHECK(!application.process_mouse_scroll_y);
    T_CHECK(application.process_keyboard);
    T_CHECK(application.process_mouse);
    T_CHECK(application.after_process_input);
    T_CHECK(application.process_world);
    T_CHECK(application.rendered);
}

void WidgetTests::closeTest(test::Test& test) {
    TestApplication application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    application.close();
    T_CHECK(application.closed);
}

void WidgetTests::mouseEventsTest(test::Test& test) {
    TestApplication application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    {
        sf::Vector2i pos(100, 100);
        application.mouseMove(pos);
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
        T_CHECK(application.process_mouse_move);
    }
    {
        sf::Vector2i pos(100, 100);
        application.mouseLeftPress();
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
        T_COMPARE(application.left_click_pos, pos, &WidgetTests::sfVec2iToStr);
    }
    {
        sf::Vector2i pos(150, 150);
        application.mouseMove(pos);
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
    }
    {
        sf::Vector2i pos(200, 200);
        application.mouseMove(pos);
        application.mouseLeftRelease();
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
        T_COMPARE(application.left_release_pos, pos, &WidgetTests::sfVec2iToStr);
    }
    {
        sf::Vector2i pos(100, 100);
        application.mouseMove(pos);
        application.mouseRightPress();
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
        T_COMPARE(application.right_click_pos, pos, &WidgetTests::sfVec2iToStr);
    }
    {
        sf::Vector2i pos(150, 150);
        application.mouseMove(pos);
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
    }
    {
        sf::Vector2i pos(200, 200);
        application.mouseMove(pos);
        application.mouseRightRelease();
        application.advance();
        T_COMPARE(application.getMousePos(), pos, &WidgetTests::sfVec2iToStr);
        T_COMPARE(application.right_release_pos, pos, &WidgetTests::sfVec2iToStr);
    }
    {
        float delta = 5.5f;
        application.mouseScrollX(delta);
        application.advance();
        T_COMPARE(application.scroll_x_delta, delta);
    }
    {
        float delta = 5.5f;
        application.mouseScrollY(delta);
        application.advance();
        T_COMPARE(application.scroll_y_delta, delta);
    }
}

void WidgetTests::keyboardEventsTest(test::Test& test) {
    TestApplication application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    T_CHECK(!application.space_key_pressed);
    {
        application.keyPress(sf::Keyboard::Space);
        application.advance();
        T_CHECK(application.space_key_pressed);
    }
    {
        application.keyRelease(sf::Keyboard::Space);
        application.advance();
        T_CHECK(!application.space_key_pressed);
    }
}

void WidgetTests::rootWidgetTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::EmptyWidget* root_widget = dynamic_cast<fw::EmptyWidget*>(application.getWidgets().getRootWidget());
    T_ASSERT(T_CHECK(root_widget));

    GenericWidgetTest gwt(application, test);
    gwt.widget = root_widget;
    gwt.total_widgets = 1;
    gwt.type = fw::Widget::WidgetType::Empty;
    gwt.name = "root";
    gwt.fullname = "root";
    gwt.is_visual_position_quantized = false;
    fw::WidgetVisibility visibility;
    visibility.addedToRoot = true;
    visibility.allParentsVisible = true;
    visibility.hasUnclippedRegion = true;
    visibility.nonZeroSize = true;
    visibility.onScreen = true;
    visibility.opaque = false;
    visibility.renderableSetting = false;
    visibility.visibleSetting = true;
    gwt.visibility = visibility;
    gwt.is_click_through = false;
    gwt.is_mouse_over = true;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
    gwt.is_focused = false;
    gwt.clip_children = true;
    gwt.force_custom_cursor = false;
    gwt.parent = nullptr;
    gwt.local_bounds = sf::FloatRect(sf::Vector2f(), fw::to2f(application.getWindowSize()));
    gwt.global_bounds = gwt.local_bounds;
    gwt.parent_local_bounds = gwt.local_bounds;
    gwt.visual_local_bounds = gwt.local_bounds;
    gwt.visual_global_bounds = gwt.local_bounds;
    gwt.visual_parent_local_bounds = gwt.local_bounds;
    T_WRAP_CONTAINER(genericWidgetTest(gwt));

    T_COMPARE(root_widget->getFillColor(), sf::Color::Transparent, &WidgetTests::colorToStr);

    T_COMPARE(root_widget->getParentChain().size(), 0);
    T_COMPARE(root_widget->getChildren().size(), 0);
}

void WidgetTests::emptyWidgetTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::EmptyWidget* empty_widget = application.getWidgets().createEmptyWidget();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(empty_widget));
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(50.0f, 30.0f);
    empty_widget->setPosition(position);
    empty_widget->setSize(size);

    GenericWidgetTest gwt(application, test);
    gwt.widget = empty_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Empty;
    gwt.name = "empty";
    gwt.fullname = "root|empty";
    gwt.is_visual_position_quantized = false;
    fw::WidgetVisibility visibility;
    visibility.addedToRoot = true;
    visibility.allParentsVisible = true;
    visibility.hasUnclippedRegion = true;
    visibility.nonZeroSize = true;
    visibility.onScreen = true;
    visibility.opaque = false;
    visibility.renderableSetting = false;
    visibility.visibleSetting = true;
    gwt.visibility = visibility;
    gwt.is_click_through = true;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
    gwt.is_focused = false;
    gwt.clip_children = false;
    gwt.force_custom_cursor = false;
    gwt.parent = root_widget;
    gwt.local_bounds = sf::FloatRect(sf::Vector2f(), size);
    gwt.global_bounds = sf::FloatRect(position, size);
    gwt.parent_local_bounds = gwt.global_bounds;
    gwt.visual_local_bounds = gwt.local_bounds;
    gwt.visual_global_bounds = gwt.global_bounds;
    gwt.visual_parent_local_bounds = gwt.global_bounds;
    T_WRAP_CONTAINER(genericWidgetTest(gwt));

    T_COMPARE(empty_widget->getFillColor(), sf::Color::Transparent, &WidgetTests::colorToStr);

    T_COMPARE(empty_widget->getChildren().size(), 0);
}

void WidgetTests::rectangleWidgetTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::RectangleWidget* rectangle_widget = application.getWidgets().createRectangleWidget(size);
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(rectangle_widget));
    sf::Vector2f position(100.0f, 100.0f);
    rectangle_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = rectangle_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Rectangle;
    gwt.name = "rectangle";
    gwt.fullname = "root|rectangle";
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
    gwt.is_click_through = true;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
    gwt.is_focused = false;
    gwt.clip_children = false;
    gwt.force_custom_cursor = false;
    gwt.parent = root_widget;
    gwt.local_bounds = sf::FloatRect(sf::Vector2f(), size);
    gwt.global_bounds = sf::FloatRect(position, size);
    gwt.parent_local_bounds = gwt.global_bounds;
    gwt.visual_local_bounds = gwt.local_bounds;
    gwt.visual_global_bounds = gwt.global_bounds;
    gwt.visual_parent_local_bounds = gwt.global_bounds;
    T_WRAP_CONTAINER(genericWidgetTest(gwt));

    T_CHECK(rectangle_widget->getParent() == root_widget);
    const CompVector<fw::Widget*>& parent_chain = rectangle_widget->getParentChain();
    T_ASSERT(T_COMPARE(parent_chain.size(), 1));
    T_CHECK(parent_chain[0] == root_widget);
    T_COMPARE(rectangle_widget->getChildren().size(), 0);
    const CompVector<fw::Widget*>& root_children = root_widget->getChildren();
    T_ASSERT(T_COMPARE(root_children.size(), 1));
    T_CHECK(root_children[0] == rectangle_widget);
    T_CHECK(root_widget->getChild(0) == rectangle_widget);

    T_COMPARE(rectangle_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
}

void WidgetTests::polygonWidgetBasicTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    std::vector<sf::Vector2f> vertices = fw::get_regular_polygon<sf::Vector2f>(6, 1.0f);
    fw::PolygonWidget* polygon_widget = application.getWidgets().createPolygonWidget(vertices);
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(polygon_widget));
    sf::Vector2f position(100.0f, 100.0f);
    polygon_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = polygon_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Polygon;
    gwt.name = "polygon";
    gwt.fullname = "root|polygon";
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
    gwt.is_click_through = true;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
    gwt.is_focused = false;
    gwt.clip_children = false;
    gwt.force_custom_cursor = false;
    gwt.parent = root_widget;
    sf::Vector2f local_pos(-1.0f, -vertices[2].y);
    sf::Vector2f size(2.0f, vertices[2].y * 2);
    gwt.local_bounds = sf::FloatRect(local_pos, size);
    gwt.global_bounds = sf::FloatRect(position + local_pos, size);
    gwt.parent_local_bounds = gwt.global_bounds;
    gwt.visual_local_bounds = gwt.local_bounds;
    gwt.visual_global_bounds = gwt.global_bounds;
    gwt.visual_parent_local_bounds = gwt.global_bounds;
    T_WRAP_CONTAINER(genericWidgetTest(gwt));

    T_CHECK(polygon_widget->getParent() == root_widget);
    T_COMPARE(polygon_widget->getChildren().size(), 0);

    T_COMPARE(polygon_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
}

void WidgetTests::setParentTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    sf::Vector2f parent_local_pos_before = parent_widget->getPosition();
    sf::Vector2f parent_global_pos_before = parent_widget->getGlobalPosition();
    sf::Vector2f child_local_pos_before = child_widget->getPosition();
    sf::Vector2f child_global_pos_before = child_widget->getGlobalPosition();
    child_widget->setParent(parent_widget);
    T_CHECK(child_widget->getParent() == parent_widget);
    const CompVector<fw::Widget*>& parent_chain = child_widget->getParentChain();
    T_ASSERT(T_COMPARE(parent_chain.size(), 2));
    T_CHECK(parent_chain[0] == parent_widget);
    T_CHECK(parent_chain[1] == root_widget);
    T_CHECK(parent_widget->getParent() == root_widget);
    T_ASSERT(T_COMPARE(child_widget->getChildren().size(), 0));
    const CompVector<fw::Widget*>& parent_children = parent_widget->getChildren();
    T_ASSERT(T_COMPARE(parent_children.size(), 1));
    T_CHECK(parent_children[0] == child_widget);
    T_CHECK(parent_widget->getChild(0) == child_widget);
    T_ASSERT(T_COMPARE(parent_widget->getChildren().size(), 1));
    T_CHECK(parent_widget->getChild(0) == child_widget);
    CompVector<fw::Widget*> root_children = root_widget->getAllChildren();
    T_ASSERT(T_COMPARE(root_children.size(), 2));
    T_CHECK(root_children[0] == parent_widget);
    T_CHECK(root_children[1] == child_widget);
    sf::Vector2f parent_local_pos_after = parent_widget->getPosition();
    sf::Vector2f parent_global_pos_after = parent_widget->getGlobalPosition();
    sf::Vector2f child_local_pos_after = child_widget->getPosition();
    sf::Vector2f child_global_pos_after = child_widget->getGlobalPosition();
    T_VEC2_APPROX_COMPARE(parent_local_pos_after, parent_local_pos_before);
    T_VEC2_APPROX_COMPARE(parent_global_pos_after, parent_global_pos_before);
    T_VEC2_APPROX_COMPARE(child_local_pos_after, child_local_pos_before - parent_local_pos_before);
    T_VEC2_APPROX_COMPARE(child_global_pos_after, child_global_pos_before);
}

void WidgetTests::widgetMouseEvents1(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::RectangleWidget* rectangle_widget = application.getWidgets().createRectangleWidget(size);
    bool mouse_entered = false;
    bool mouse_left_pressed = false;
    bool mouse_right_pressed = false;
    bool mouse_left_released_global = false;
    bool mouse_left_released_blockable = false;
    bool mouse_right_released_global = false;
    bool mouse_right_released_blockable = false;
    bool mouse_left_clicked = false;
    bool mouse_right_clicked = false;
    bool mouse_moved = false;
    bool mouse_scroll_x = false;
    bool mouse_scroll_y = false;
    bool mouse_exited = false;
    bool mouse_processed = false;
    rectangle_widget->OnMouseEnter += [&](const sf::Vector2f& pos) {
        mouse_entered = true;
    };
    rectangle_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
        mouse_left_pressed = true;
    };
    rectangle_widget->OnRightPress += [&](const sf::Vector2f& pos) {
        mouse_right_pressed = true;
    };
    rectangle_widget->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_left_released_global = true;
    };
    rectangle_widget->OnBlockableLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_left_released_blockable = true;
    };
    rectangle_widget->OnGlobalRightRelease += [&](const sf::Vector2f& pos) {
        mouse_right_released_global = true;
    };
    rectangle_widget->OnBlockableRightRelease += [&](const sf::Vector2f& pos) {
        mouse_right_released_blockable = true;
    };
    rectangle_widget->OnLeftClick += [&](const sf::Vector2f& pos) {
        mouse_left_clicked = true;
    };
    rectangle_widget->OnRightClick += [&](const sf::Vector2f& pos) {
        mouse_right_clicked = true;
    };
    rectangle_widget->OnMouseMoved += [&](const sf::Vector2f& pos) {
        mouse_moved = true;
    };
    rectangle_widget->OnScrollX += [&](const sf::Vector2f& pos, float delta) {
        mouse_scroll_x = true;
    };
    rectangle_widget->OnScrollY += [&](const sf::Vector2f& pos, float delta) {
        mouse_scroll_y = true;
    };
    rectangle_widget->OnMouseExit += [&](const sf::Vector2f& pos) {
        mouse_exited = true;
    };
    rectangle_widget->OnProcessMouse += [&](const sf::Vector2f& pos) {
        mouse_processed = true;
    };
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2i mouse_pos_1(150, 150);
    sf::Vector2i mouse_pos_2(300, 300);
    rectangle_widget->setPosition(position);
    T_CHECK(!rectangle_widget->isMouseOver());
    T_CHECK(!mouse_entered);
    T_CHECK(!mouse_left_pressed);
    T_CHECK(!mouse_left_released_global);
    T_CHECK(!mouse_left_released_blockable);
    T_CHECK(!mouse_right_pressed);
    T_CHECK(!mouse_right_released_global);
    T_CHECK(!mouse_right_released_blockable);
    T_CHECK(!mouse_left_clicked);
    T_CHECK(!mouse_right_clicked);
    T_CHECK(!mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(!mouse_processed);
    application.mouseMove(mouse_pos_1);
    application.advance();
    T_CHECK(rectangle_widget->isMouseOver());
    T_CHECK(mouse_entered);
    T_CHECK(!mouse_left_pressed);
    T_CHECK(!mouse_left_released_global);
    T_CHECK(!mouse_left_released_blockable);
    T_CHECK(!mouse_right_pressed);
    T_CHECK(!mouse_right_released_global);
    T_CHECK(!mouse_right_released_blockable);
    T_CHECK(!mouse_left_clicked);
    T_CHECK(!mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseLeftPress();
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(!mouse_left_released_global);
    T_CHECK(!mouse_left_released_blockable);
    T_CHECK(!mouse_right_pressed);
    T_CHECK(!mouse_right_released_global);
    T_CHECK(!mouse_right_released_blockable);
    T_CHECK(!mouse_left_clicked);
    T_CHECK(!mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(!mouse_right_pressed);
    T_CHECK(!mouse_right_released_global);
    T_CHECK(!mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(!mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseRightPress();
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(mouse_right_pressed);
    T_CHECK(!mouse_right_released_global);
    T_CHECK(!mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(!mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseRightRelease();
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(mouse_right_pressed);
    T_CHECK(mouse_right_released_global);
    T_CHECK(mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(!mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseScrollX(1.0f);
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(mouse_right_pressed);
    T_CHECK(mouse_right_released_global);
    T_CHECK(mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(mouse_scroll_x);
    T_CHECK(!mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseScrollY(1.0f);
    application.advance();
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(mouse_right_pressed);
    T_CHECK(mouse_right_released_global);
    T_CHECK(mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(mouse_scroll_x);
    T_CHECK(mouse_scroll_y);
    T_CHECK(!mouse_exited);
    T_CHECK(mouse_processed);
    application.mouseMove(mouse_pos_2);
    application.advance();
    T_CHECK(!rectangle_widget->isMouseOver());
    T_CHECK(mouse_entered);
    T_CHECK(mouse_left_pressed);
    T_CHECK(mouse_left_released_global);
    T_CHECK(mouse_left_released_blockable);
    T_CHECK(mouse_right_pressed);
    T_CHECK(mouse_right_released_global);
    T_CHECK(mouse_right_released_blockable);
    T_CHECK(mouse_left_clicked);
    T_CHECK(mouse_right_clicked);
    T_CHECK(mouse_moved);
    T_CHECK(mouse_scroll_x);
    T_CHECK(mouse_scroll_y);
    T_CHECK(mouse_exited);
    T_CHECK(mouse_processed);
}

void WidgetTests::widgetMouseEvents2(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::RectangleWidget* rectangle_widget_1 = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_widget_2 = application.getWidgets().createRectangleWidget(size);
    rectangle_widget_1->setClickThrough(false);
    bool mouse_entered_1 = false;
    bool mouse_pressed_1 = false;
    bool mouse_released_global_1 = false;
    bool mouse_released_blockable_1 = false;
    bool mouse_clicked_1 = false;
    bool mouse_exited_1 = false;
    bool mouse_processed_1 = false;
    bool mouse_entered_2 = false;
    bool mouse_pressed_2 = false;
    bool mouse_released_global_2 = false;
    bool mouse_released_blockable_2 = false;
    bool mouse_clicked_2 = false;
    bool mouse_exited_2 = false;
    bool mouse_processed_2 = false;
    rectangle_widget_1->OnMouseEnter += [&](const sf::Vector2f& pos) {
        mouse_entered_1 = true;
    };
    rectangle_widget_1->OnLeftPress += [&](const sf::Vector2f& pos) {
        mouse_pressed_1 = true;
    };
    rectangle_widget_1->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_released_global_1 = true;
    };
    rectangle_widget_1->OnBlockableLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_released_blockable_1 = true;
    };
    rectangle_widget_1->OnLeftClick += [&](const sf::Vector2f& pos) {
        mouse_clicked_1 = true;
    };
    rectangle_widget_1->OnMouseExit += [&](const sf::Vector2f& pos) {
        mouse_exited_1 = true;
    };
    rectangle_widget_1->OnProcessMouse += [&](const sf::Vector2f& pos) {
        mouse_processed_1 = true;
    };
    rectangle_widget_2->OnMouseEnter += [&](const sf::Vector2f& pos) {
        mouse_entered_2 = true;
    };
    rectangle_widget_2->OnLeftPress += [&](const sf::Vector2f& pos) {
        mouse_pressed_2 = true;
    };
    rectangle_widget_2->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_released_global_2 = true;
    };
    rectangle_widget_2->OnBlockableLeftRelease += [&](const sf::Vector2f& pos) {
        mouse_released_blockable_2 = true;
    };
    rectangle_widget_2->OnLeftClick += [&](const sf::Vector2f& pos) {
        mouse_clicked_2 = true;
    };
    rectangle_widget_2->OnMouseExit += [&](const sf::Vector2f& pos) {
        mouse_exited_2 = true;
    };
    rectangle_widget_2->OnProcessMouse += [&](const sf::Vector2f& pos) {
        mouse_processed_2 = true;
    };
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2i mouse_pos_1(150, 150);
    sf::Vector2i mouse_pos_2(300, 300);
    rectangle_widget_1->setPosition(position);
    rectangle_widget_2->setPosition(position);
    T_CHECK(!rectangle_widget_1->isMouseOver());
    T_CHECK(!mouse_entered_1);
    T_CHECK(!mouse_pressed_1);
    T_CHECK(!mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(!mouse_processed_1);
    T_CHECK(!rectangle_widget_2->isMouseOver());
    T_CHECK(!mouse_entered_2);
    T_CHECK(!mouse_pressed_2);
    T_CHECK(!mouse_released_global_2);
    T_CHECK(!mouse_released_blockable_2);
    T_CHECK(!mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(!mouse_processed_2);
    application.mouseMove(mouse_pos_1);
    application.advance();
    T_CHECK(rectangle_widget_1->isMouseOver());
    T_CHECK(mouse_entered_1);
    T_CHECK(!mouse_pressed_1);
    T_CHECK(!mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(rectangle_widget_2->isMouseOver());
    T_CHECK(mouse_entered_2);
    T_CHECK(!mouse_pressed_2);
    T_CHECK(!mouse_released_global_2);
    T_CHECK(!mouse_released_blockable_2);
    T_CHECK(!mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(mouse_processed_2);
    rectangle_widget_2->setClickThrough(true);
    application.mouseLeftPress();
    application.advance();
    T_CHECK(mouse_entered_1);
    T_CHECK(mouse_pressed_1);
    T_CHECK(!mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(mouse_entered_2);
    T_CHECK(mouse_pressed_2);
    T_CHECK(!mouse_released_global_2);
    T_CHECK(!mouse_released_blockable_2);
    T_CHECK(!mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(mouse_processed_2);
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(mouse_entered_1);
    T_CHECK(mouse_pressed_1);
    T_CHECK(mouse_released_global_1);
    T_CHECK(mouse_released_blockable_1);
    T_CHECK(mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(mouse_entered_2);
    T_CHECK(mouse_pressed_2);
    T_CHECK(mouse_released_global_2);
    T_CHECK(mouse_released_blockable_2);
    T_CHECK(mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(mouse_processed_2);
    mouse_pressed_1 = false;
    mouse_pressed_2 = false;
    mouse_released_global_1 = false;
    mouse_released_blockable_1 = false;
    mouse_clicked_1 = false;
    mouse_released_global_2 = false;
    mouse_released_blockable_2 = false;
    mouse_clicked_2 = false;
    rectangle_widget_2->setClickThrough(false);
    application.mouseLeftPress();
    application.advance();
    T_CHECK(mouse_entered_1);
    T_CHECK(!mouse_pressed_1);
    T_CHECK(!mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(mouse_entered_2);
    T_CHECK(mouse_pressed_2);
    T_CHECK(!mouse_released_global_2);
    T_CHECK(!mouse_released_blockable_2);
    T_CHECK(!mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(mouse_processed_2);
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(mouse_entered_1);
    T_CHECK(!mouse_pressed_1);
    T_CHECK(mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(!mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(mouse_entered_2);
    T_CHECK(mouse_pressed_2);
    T_CHECK(mouse_released_global_2);
    T_CHECK(mouse_released_blockable_2);
    T_CHECK(mouse_clicked_2);
    T_CHECK(!mouse_exited_2);
    T_CHECK(mouse_processed_2);
    application.mouseMove(mouse_pos_2);
    application.advance();
    T_CHECK(!rectangle_widget_1->isMouseOver());
    T_CHECK(mouse_entered_1);
    T_CHECK(!mouse_pressed_1);
    T_CHECK(mouse_released_global_1);
    T_CHECK(!mouse_released_blockable_1);
    T_CHECK(!mouse_clicked_1);
    T_CHECK(mouse_exited_1);
    T_CHECK(mouse_processed_1);
    T_CHECK(!rectangle_widget_2->isMouseOver());
    T_CHECK(mouse_entered_2);
    T_CHECK(mouse_pressed_2);
    T_CHECK(mouse_released_global_2);
    T_CHECK(mouse_released_blockable_2);
    T_CHECK(mouse_clicked_2);
    T_CHECK(mouse_exited_2);
    T_CHECK(mouse_processed_2);
}

void WidgetTests::dragGestureEventTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::RectangleWidget* rectangle_widget_1 = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_widget_2 = application.getWidgets().createRectangleWidget(size);
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2i mouse_pos_1(150, 150);
    sf::Vector2i mouse_pos_2(300, 300);
    rectangle_widget_1->setPosition(position);
    rectangle_widget_2->setPosition(position);
    rectangle_widget_1->setClickThrough(false);
    sf::Vector2f drag_pos;
    bool dragged_1 = false;
    bool dragged_2 = false;
    rectangle_widget_1->OnProcessDragGesture += [&](sf::Mouse::Button button, const sf::Vector2f& pos) {
        dragged_1 = true;
        drag_pos = pos;
    };
    rectangle_widget_2->OnProcessDragGesture += [&](sf::Mouse::Button button, const sf::Vector2f& pos) {
        dragged_2 = true;
    };
    application.advance();
    T_CHECK(!dragged_1);
    T_CHECK(!dragged_2);

    application.mouseMove(mouse_pos_1);
    application.advance();
    T_CHECK(!dragged_1);
    T_CHECK(!dragged_2);
    application.mouseLeftPress();
    application.advance();
    T_CHECK(dragged_1);
    T_CHECK(!dragged_2);
    T_VEC2_APPROX_COMPARE(drag_pos, fw::to2f(mouse_pos_1));
    application.mouseMove(mouse_pos_2);
    application.advance();
    T_CHECK(dragged_1);
    T_CHECK(!dragged_2);
    T_VEC2_APPROX_COMPARE(drag_pos, fw::to2f(mouse_pos_2));
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(dragged_1);
    T_CHECK(!dragged_2);
    T_VEC2_APPROX_COMPARE(drag_pos, fw::to2f( mouse_pos_2));
}

void WidgetTests::eventsTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    bool pre_update = false;
    bool post_update = false;
    bool before_global_render = false;
    bool before_render = false;
    bool after_render = false;
    bool after_global_render = false;
    unsigned int window_width = 0;
    unsigned int window_height = 0;
    fw::RectangleWidget* widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    widget->OnPreUpdate += [&]() {
        pre_update = true;
    };
    widget->OnPostUpdate += [&]() {
        post_update = true;
    };
    widget->OnBeforeGlobalRender += [&](sf::RenderTarget& target) {
        before_global_render = true;
    };
    widget->OnBeforeRender += [&](sf::RenderTarget& target) {
        before_render = true;
    };
    widget->OnAfterRender += [&](sf::RenderTarget& target) {
        after_render = true;
    };
    widget->OnAfterGlobalRender += [&](sf::RenderTarget& target) {
        after_global_render = true;
    };
    widget->OnWindowResized += [&](unsigned int width, unsigned int height) {
        window_width = width;
        window_height = height;
    };
    application.advance();
    T_CHECK(pre_update);
    T_CHECK(post_update);
    T_CHECK(before_global_render);
    T_CHECK(after_global_render);
    T_COMPARE(window_width, 0);
    T_COMPARE(window_height, 0);
    application.setWindowSize(640, 480);
    application.advance();
    T_COMPARE(window_width, 640);
    T_COMPARE(window_height, 480);
}

void WidgetTests::coordinatesTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    sf::Vector2f parent_pos(100.0f, 100.0f);
    sf::Vector2f child_local_pos(30.0f, 30.0f);
    child_widget->setParent(parent_widget);
    parent_widget->setPosition(parent_pos);
    child_widget->setPosition(child_local_pos);
    T_VEC2_APPROX_COMPARE(child_widget->toGlobal(sf::Vector2f(10.0f, 5.0f)), sf::Vector2f(140.0f, 135.0f));
    T_VEC2_APPROX_COMPARE(child_widget->toLocal(sf::Vector2f(140.0f, 135.0f)), sf::Vector2f(10.0f, 5.0f));

}

void WidgetTests::findTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    parent_widget->setName("parent");
    child_widget->setName("child");
    child_widget->setParent(parent_widget);
    T_CHECK(application.getWidgets().find("root") == application.getWidgets().getRootWidget());
    T_CHECK(application.getWidgets().find("parent") == parent_widget);
    T_CHECK(application.getWidgets().find("child") == child_widget);
}

void WidgetTests::anchorTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f parent_size(100.0f, 100.0f);
    sf::Vector2f child_size(30.0f, 30.0f);
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(parent_size);
    fw::RectangleWidget* child_widget = application.getWidgets().createRectangleWidget(child_size);
    child_widget->setParent(parent_widget);
    sf::Vector2f anchor_offset(5.0f, 7.0f);
    child_widget->setParentAnchor(fw::Widget::Anchor::TOP_LEFT);
    child_widget->setAnchorOffset(anchor_offset);
    T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f() + anchor_offset);
    child_widget->setParentAnchor(fw::Widget::Anchor::TOP_CENTER);
    T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x / 2.0f, 0.0f) + anchor_offset);
    child_widget->setParentAnchor(fw::Widget::Anchor::TOP_RIGHT);
    T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x, 0.0f) + anchor_offset);
    child_widget->setParentAnchor(fw::Widget::Anchor::CENTER_LEFT);
    T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(0.0f, parent_size.y / 2.0f) + anchor_offset);
    child_widget->setParentAnchor(fw::Widget::Anchor::CENTER);
    T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x / 2.0f, parent_size.y / 2.0f) + anchor_offset);
    child_widget->setParentAnchor(fw::Widget::Anchor::CENTER_RIGHT);
    T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x, parent_size.y / 2.0f) + anchor_offset);
    child_widget->setParentAnchor(fw::Widget::Anchor::BOTTOM_LEFT);
    T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(0.0f, parent_size.y) + anchor_offset);
    child_widget->setParentAnchor(fw::Widget::Anchor::BOTTOM_CENTER);
    T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x / 2.0f, parent_size.y) + anchor_offset);
    child_widget->setParentAnchor(fw::Widget::Anchor::BOTTOM_RIGHT);
    T_VEC2_APPROX_COMPARE(child_widget->getPosition(), sf::Vector2f(parent_size.x, parent_size.y) + anchor_offset);
}

void WidgetTests::remove1Test(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child0_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child1_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    parent_widget->setName("parent");
    child0_widget->setName("child0");
    child0_widget->setParent(parent_widget);
    child1_widget->setName("child1");
    child1_widget->setParent(parent_widget);
    if (T_COMPARE(application.getWidgets().getSize(), 4)) {
        if (T_COMPARE(root_widget->getChildrenCount(), 1)) {
            T_CHECK(root_widget->getChild(0) == parent_widget);
            if (T_COMPARE(parent_widget->getChildrenCount(), 2)) {
                T_CHECK(parent_widget->getChild(0) == child0_widget);
                T_CHECK(parent_widget->getChild(1) == child1_widget);
                T_COMPARE(child0_widget->getChildrenCount(), 0);
                T_COMPARE(child1_widget->getChildrenCount(), 0);
            }
        }
    }

    application.advance();

    T_ASSERT_NO_ERRORS();
    child0_widget->remove();
    if (T_COMPARE(application.getWidgets().getSize(), 3)) {
        if (T_COMPARE(root_widget->getChildrenCount(), 1)) {
            T_CHECK(root_widget->getChild(0) == parent_widget);
            if (T_COMPARE(parent_widget->getChildrenCount(), 1)) {
                T_CHECK(parent_widget->getChild(0) == child1_widget);
                T_COMPARE(child1_widget->getChildrenCount(), 0);
            }
        }
    }

    application.advance();

    T_ASSERT_NO_ERRORS();
    parent_widget->remove();
    if (T_COMPARE(application.getWidgets().getSize(), 1)) {
        T_COMPARE(root_widget->getChildrenCount(), 0);
    }

    application.advance();
}

void WidgetTests::remove2Test(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* subparent_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child0_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    fw::RectangleWidget* child1_widget = application.getWidgets().createRectangleWidget(100.0f, 100.0f);
    parent_widget->setName("parent");
    subparent_widget->setName("subparent");
    subparent_widget->setParent(parent_widget);
    child0_widget->setName("child0");
    child0_widget->setParent(subparent_widget);
    child1_widget->setName("child1");
    child1_widget->setParent(subparent_widget);
    if (T_COMPARE(application.getWidgets().getSize(), 5)) {
        if (T_COMPARE(root_widget->getChildrenCount(), 1)) {
            T_CHECK(root_widget->getChild(0) == parent_widget);
            if (T_COMPARE(parent_widget->getChildrenCount(), 1)) {
                T_CHECK(parent_widget->getChild(0) == subparent_widget);
                if (T_COMPARE(subparent_widget->getChildrenCount(), 2)) {
                    T_CHECK(subparent_widget->getChild(0) == child0_widget);
                    T_CHECK(subparent_widget->getChild(1) == child1_widget);
                    T_COMPARE(child0_widget->getChildrenCount(), 0);
                    T_COMPARE(child1_widget->getChildrenCount(), 0);
                }
            }
        }
    }

    application.advance();

    T_ASSERT_NO_ERRORS();
    subparent_widget->remove(false);
    if (T_COMPARE(application.getWidgets().getSize(), 4)) {
        if (T_COMPARE(root_widget->getChildrenCount(), 1)) {
            T_CHECK(root_widget->getChild(0) == parent_widget);
            if (T_COMPARE(parent_widget->getChildrenCount(), 2)) {
                T_CHECK(parent_widget->getChild(0) == child0_widget);
                T_CHECK(parent_widget->getChild(1) == child1_widget);
                T_COMPARE(child0_widget->getChildrenCount(), 0);
                T_COMPARE(child1_widget->getChildrenCount(), 0);
            }
        }
    }

    application.advance();
}

void WidgetTests::textWidgetTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::TextWidget* text_widget = application.getWidgets().createTextWidget();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(text_widget));
    text_widget->setCharacterSize(20);
    text_widget->setFont(textbox_font);
    text_widget->setString("Text");
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(41.0f, 20.0f);
    sf::Vector2f local_bounds_offset(0.0f, 6.0f);
    text_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = text_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Text;
    gwt.name = "text";
    gwt.fullname = "root|text";
    gwt.is_visual_position_quantized = true;
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
    gwt.is_click_through = true;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
    gwt.is_focused = false;
    gwt.clip_children = false;
    gwt.force_custom_cursor = false;
    gwt.parent = root_widget;
    gwt.local_bounds = sf::FloatRect(sf::Vector2f(), size);
    gwt.global_bounds = sf::FloatRect(position, size);
    gwt.parent_local_bounds = gwt.global_bounds;
    gwt.visual_local_bounds = sf::FloatRect(local_bounds_offset, size - local_bounds_offset);
    gwt.visual_global_bounds = sf::FloatRect(position + local_bounds_offset, size - local_bounds_offset);
    gwt.visual_parent_local_bounds = gwt.visual_global_bounds;
    T_WRAP_CONTAINER(genericWidgetTest(gwt));

    T_COMPARE(text_widget->getChildren().size(), 0);

    T_COMPARE(text_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
}

void WidgetTests::textWidgetDefaultFontTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
    application.start(true);
    application.advance();
    fw::TextWidget* text_widget = application.getWidgets().createTextWidget();
    text_widget->setCharacterSize(20);
    text_widget->setString("Text");
    sf::Vector2f position(100.0f, 100.0f);
    text_widget->setPosition(position);
    application.advance();
}

void WidgetTests::checkboxWidgetBasicTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::CheckboxWidget* checkbox_widget = application.getWidgets().createCheckboxWidget();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(checkbox_widget));
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(20.0f, 20.0f);
    checkbox_widget->setPosition(position);
    checkbox_widget->setSize(size);

    GenericWidgetTest gwt(application, test);
    gwt.widget = checkbox_widget;
    gwt.total_widgets = 3;
    gwt.type = fw::Widget::WidgetType::Checkbox;
    gwt.name = "checkbox";
    gwt.fullname = "root|checkbox";
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
    gwt.clip_children = false;
    gwt.force_custom_cursor = false;
    gwt.parent = root_widget;
    gwt.local_bounds = sf::FloatRect(sf::Vector2f(), size);
    gwt.global_bounds = sf::FloatRect(position, size);
    gwt.parent_local_bounds = gwt.global_bounds;
    gwt.visual_local_bounds = gwt.local_bounds;
    gwt.visual_global_bounds = gwt.global_bounds;
    gwt.visual_parent_local_bounds = gwt.global_bounds;
    T_WRAP_CONTAINER(genericWidgetTest(gwt));

    if (T_COMPARE(checkbox_widget->getChildren().size(), 1)) {
        fw::RectangleWidget* check_widget = dynamic_cast<fw::RectangleWidget*>(checkbox_widget->getChild(0));
        T_CHECK(check_widget, "Check widget is not a RectangleWidget");
        T_COMPARE(check_widget->getParentAnchor(), fw::Widget::Anchor::CENTER, &WidgetTests::anchorToStr);
    }

    T_COMPARE(checkbox_widget->getFillColor(), sf::Color(50, 50, 50), &WidgetTests::colorToStr);
}

void WidgetTests::checkboxWidgetToggleTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    fw::CheckboxWidget* checkbox_widget = application.getWidgets().createCheckboxWidget();
    T_CHECK(!checkbox_widget->getValue());
    application.mouseMove(checkbox_widget->getGlobalCenter());
    application.advance();
    application.mouseLeftPress();
    application.advance();
    T_CHECK(checkbox_widget->getValue());
    application.mouseLeftRelease();
    application.advance();
    T_CHECK(checkbox_widget->getValue());
}

void WidgetTests::containerWidgetBasicTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(size);
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(container_widget));
    sf::Vector2f position(100.0f, 100.0f);
    container_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = container_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Container;
    gwt.name = "container";
    gwt.fullname = "root|container";
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
    gwt.is_click_through = true;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
    gwt.is_focused = false;
    gwt.clip_children = false;
    gwt.force_custom_cursor = false;
    gwt.parent = root_widget;
    gwt.local_bounds = sf::FloatRect(sf::Vector2f(), size);
    gwt.global_bounds = sf::FloatRect(position, size);
    gwt.parent_local_bounds = gwt.global_bounds;
    gwt.visual_local_bounds = gwt.local_bounds;
    gwt.visual_global_bounds = gwt.global_bounds;
    gwt.visual_parent_local_bounds = gwt.global_bounds;
    T_WRAP_CONTAINER(genericWidgetTest(gwt));

    T_COMPARE(container_widget->getChildren().size(), 0);

    T_COMPARE(container_widget->getFillColor(), sf::Color::White, &WidgetTests::colorToStr);
}

void WidgetTests::containerWidgetChildrenTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    sf::Vector2f container_size(100.0f, 100.0f);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    sf::Vector2f position(100.0f, 100.0f);
    float padding_x = 5.0f;
    float padding_y = 10.0f;
    container_widget->setPosition(position);
    container_widget->setPaddingX(padding_x);
    container_widget->setPaddingY(padding_y);
    T_VEC2_APPROX_COMPARE(container_widget->getSize(), container_size);
    application.advance();
    T_VEC2_APPROX_COMPARE(container_widget->getSize(), sf::Vector2f(padding_x, padding_y));
    sf::Vector2f child_1_size(35.0f, 28.0f);
    sf::Vector2f child_2_size(46.0f, 54.0f);
    fw::RectangleWidget* child_1_widget = application.getWidgets().createRectangleWidget(child_1_size);
    fw::RectangleWidget* child_2_widget = application.getWidgets().createRectangleWidget(child_2_size);
    child_1_widget->setParent(container_widget);
    child_2_widget->setParent(container_widget);
    application.advance();
    sf::Vector2f new_container_size = sf::Vector2f(
        padding_x + child_1_widget->getWidth() + padding_x + child_2_widget->getWidth() + padding_x,
        padding_y + std::max(child_1_widget->getHeight(), child_2_widget->getHeight()) + padding_y
    );
    T_VEC2_APPROX_COMPARE(container_widget->getSize(), new_container_size);
    T_VEC2_APPROX_COMPARE(child_1_widget->getPosition(), sf::Vector2f(padding_x, padding_y));
    T_VEC2_APPROX_COMPARE(child_2_widget->getPosition(), sf::Vector2f(padding_x + child_1_widget->getWidth() + padding_x, padding_y));
}

void WidgetTests::containerWidgetPaddingTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    sf::Vector2f container_size(100.0f, 100.0f);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    sf::Vector2f position(100.0f, 100.0f);
    float inner_padding_x = 5.0f;
    float inner_padding_y = 11.0f;
    float top_padding = 1.0f;
    float bottom_padding = 7.0f;
    float left_padding = 2.0f;
    float right_padding = 3.0f;
    container_widget->setPosition(position);
    container_widget->setInnerPaddingX(inner_padding_x);
    container_widget->setInnerPaddingY(inner_padding_y);
    container_widget->setTopPadding(top_padding);
    container_widget->setBottomPadding(bottom_padding);
    container_widget->setLeftPadding(left_padding);
    container_widget->setRightPadding(right_padding);
    T_VEC2_APPROX_COMPARE(container_widget->getSize(), container_size);
    application.advance();
    T_VEC2_APPROX_COMPARE(container_widget->getSize(), sf::Vector2f(right_padding, bottom_padding));
    sf::Vector2f child_1_size(35.0f, 28.0f);
    sf::Vector2f child_2_size(46.0f, 54.0f);
    sf::Vector2f child_3_size(27.0f, 14.0f);
    fw::RectangleWidget* child_1_widget = application.getWidgets().createRectangleWidget(child_1_size);
    fw::RectangleWidget* child_2_widget = application.getWidgets().createRectangleWidget(child_2_size);
    fw::RectangleWidget* child_3_widget = application.getWidgets().createRectangleWidget(child_3_size);
    child_1_widget->setParent(container_widget);
    child_2_widget->setParent(container_widget);
    child_3_widget->setParent(container_widget);
    {
        // horizontal
        application.advance();
        sf::Vector2f new_container_size = sf::Vector2f(
            left_padding + child_1_widget->getWidth() + inner_padding_x + child_2_widget->getWidth() + inner_padding_x + child_3_widget->getWidth() + right_padding,
            top_padding + std::max(child_1_widget->getHeight(), child_2_widget->getHeight()) + bottom_padding
        );
        float child_1_x = left_padding;
        float child_2_x = child_1_x + child_1_widget->getWidth() + inner_padding_x;
        float child_3_x = child_2_x + child_2_widget->getWidth() + inner_padding_x;
        T_VEC2_APPROX_COMPARE(container_widget->getSize(), new_container_size);
        T_VEC2_APPROX_COMPARE(child_1_widget->getPosition(), sf::Vector2f(child_1_x, top_padding));
        T_VEC2_APPROX_COMPARE(child_2_widget->getPosition(), sf::Vector2f(child_2_x, top_padding));
        T_VEC2_APPROX_COMPARE(child_3_widget->getPosition(), sf::Vector2f(child_3_x, top_padding));
    }
    {
        // vertical
        container_widget->setHorizontal(false);
        application.advance();
        sf::Vector2f new_container_size = sf::Vector2f(
            left_padding + std::max(child_1_widget->getWidth(), child_2_widget->getWidth()) + right_padding,
            top_padding + child_1_widget->getHeight() + inner_padding_y + child_2_widget->getHeight() + inner_padding_y + child_3_widget->getHeight() + bottom_padding
        );
        float child_1_y = top_padding;
        float child_2_y = child_1_y + child_1_widget->getHeight() + inner_padding_y;
        float child_3_y = child_2_y + child_2_widget->getHeight() + inner_padding_y;
        T_VEC2_APPROX_COMPARE(container_widget->getSize(), new_container_size);
        T_VEC2_APPROX_COMPARE(child_1_widget->getPosition(), sf::Vector2f(left_padding, child_1_y));
        T_VEC2_APPROX_COMPARE(child_2_widget->getPosition(), sf::Vector2f(left_padding, child_2_y));
        T_VEC2_APPROX_COMPARE(child_3_widget->getPosition(), sf::Vector2f(left_padding, child_3_y));
    }
}

void WidgetTests::sizePolicyTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f parent_size(150.0f, 150.0f);
    sf::Vector2f container_size(100.0f, 100.0f);
    sf::Vector2f child_size(30.0f, 30.0f);
    float container_padding = 10.0f;
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(parent_size);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::RectangleWidget* red_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* green_widget = application.getWidgets().createRectangleWidget(child_size);
    parent_widget->setFillColor(sf::Color(128, 128, 128));
    container_widget->setFillColor(sf::Color(100, 100, 100));
    red_widget->setFillColor(sf::Color::Red);
    green_widget->setFillColor(sf::Color::Green);
    container_widget->setPadding(container_padding);
    container_widget->setHorizontal(false);
    container_widget->setParent(parent_widget);
    red_widget->setParent(container_widget);
    green_widget->setParent(container_widget);
    container_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    red_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    application.advance();
    auto rect_to_str = &WidgetTests::floatRectToStr;
    float container_height = child_size.y * 2 + container_padding * 3;
    T_COMPARE(parent_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(container_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, container_height), rect_to_str);
    T_COMPARE(red_widget->getParentLocalBounds(), sf::FloatRect(container_padding, container_padding, parent_size.x - container_padding * 2, child_size.y), rect_to_str);
    T_COMPARE(green_widget->getParentLocalBounds(), sf::FloatRect(container_padding, container_padding * 2 + child_size.y, child_size.x, child_size.y), rect_to_str);
}

void WidgetTests::sizePolicyPositionTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f parent_size(150.0f, 150.0f);
    sf::Vector2f child0_size(100.0f, 100.0f);
    sf::Vector2f child1_size(30.0f, 30.0f);
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(parent_size);
    fw::RectangleWidget* child0_widget = application.getWidgets().createRectangleWidget(child0_size);
    fw::RectangleWidget* child1_widget = application.getWidgets().createRectangleWidget(child1_size);
    parent_widget->setName("parent");
    child0_widget->setName("child0");
    child1_widget->setName("child1");
    parent_widget->setFillColor(sf::Color(128, 128, 128));
    child0_widget->setFillColor(sf::Color::Red);
    child1_widget->setFillColor(sf::Color::Green);
    child0_widget->setParent(parent_widget);
    child1_widget->setParent(child0_widget);
    auto rect_to_str = &WidgetTests::floatRectToStr;

    child0_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    child1_widget->setParentAnchor(fw::Widget::Anchor::TOP_RIGHT);
    application.advance();
    T_COMPARE(parent_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(child0_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, child0_size.y), rect_to_str);
    T_COMPARE(child1_widget->getParentLocalBounds(), sf::FloatRect(parent_size.x, 0.0f, child1_size.x, child1_size.y), rect_to_str);

    child1_widget->setParentAnchor(fw::Widget::Anchor::CENTER);
    application.advance();
    T_COMPARE(parent_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(child0_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, child0_size.y), rect_to_str);
    T_COMPARE(child1_widget->getParentLocalBounds(), sf::FloatRect(parent_size.x / 2.0f, child0_size.y / 2.0f, child1_size.x, child1_size.y), rect_to_str);

    child0_widget->setSizeYPolicy(fw::Widget::SizePolicy::PARENT);
    application.advance();
    T_COMPARE(parent_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(child0_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(child1_widget->getParentLocalBounds(), sf::FloatRect(parent_size.x / 2.0f, parent_size.y / 2.0f, child1_size.x, child1_size.y), rect_to_str);
}

void WidgetTests::sizePolicyExpandTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f container_size(200.0f, 150.0f);
    sf::Vector2f child_size(30.0f, 30.0f);
    float container_padding = 10.0f;
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::RectangleWidget* red_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* green_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* blue_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* yellow_widget = application.getWidgets().createRectangleWidget(child_size);
    container_widget->setName("container");
    container_widget->setPadding(container_padding);
    red_widget->setName("red");
    green_widget->setName("green");
    blue_widget->setName("blue");
    yellow_widget->setName("yellow");
    red_widget->setParent(container_widget);
    green_widget->setParent(container_widget);
    blue_widget->setParent(container_widget);
    yellow_widget->setParent(container_widget);
    container_widget->setFillColor(sf::Color(100, 100, 100));
    red_widget->setFillColor(sf::Color::Red);
    green_widget->setFillColor(sf::Color::Green);
    blue_widget->setFillColor(sf::Color::Blue);
    yellow_widget->setFillColor(sf::Color::Yellow);
    container_widget->setSizeXPolicy(fw::Widget::SizePolicy::NONE);
    green_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    yellow_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    auto rect_to_str = &WidgetTests::floatRectToStr;

    float red_x = container_padding;
    float green_x = container_padding * 2 + child_size.x;
    float fixed_widgets_size = child_size.x * 2;
    float padding_size = container_padding * (container_widget->getChildren().size() + 1);
    float free_space = container_size.x - fixed_widgets_size - padding_size;
    float green_width = free_space / 2;
    float blue_x = green_x + green_width + container_padding;
    float yellow_x = blue_x + child_size.x + container_padding;
    float yellow_width = green_width;
    application.advance();
    T_COMPARE(container_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, container_size.x, child_size.y + container_padding * 2), rect_to_str);
    T_COMPARE(red_widget->getParentLocalBounds(), sf::FloatRect(red_x, container_padding, child_size.x, child_size.y), rect_to_str);
    T_COMPARE(green_widget->getParentLocalBounds(), sf::FloatRect(green_x, container_padding, green_width, child_size.y), rect_to_str);
    T_COMPARE(blue_widget->getParentLocalBounds(), sf::FloatRect(blue_x, container_padding, child_size.x, child_size.y), rect_to_str);
    T_COMPARE(yellow_widget->getParentLocalBounds(), sf::FloatRect(yellow_x, container_padding, yellow_width, child_size.y), rect_to_str);
}

void WidgetTests::sizePolicyLimitsTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f container_size(300.0f, 150.0f);
    sf::Vector2f child_size(30.0f, 30.0f);
    float container_padding = 10.0f;
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::RectangleWidget* red_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* green_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* blue_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* yellow_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* magenta_widget = application.getWidgets().createRectangleWidget(child_size);
    container_widget->setName("container");
    container_widget->setPadding(container_padding);
    container_widget->setFillColor(sf::Color(100, 100, 100));
    container_widget->setSizeXPolicy(fw::Widget::SizePolicy::NONE);
    red_widget->setName("red");
    red_widget->setParent(container_widget);
    red_widget->setFillColor(sf::Color::Red);
    green_widget->setName("green");
    green_widget->setParent(container_widget);
    green_widget->setFillColor(sf::Color::Green);
    green_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    green_widget->setMinSize(75.0f, 0.0f);
    blue_widget->setName("blue");
    blue_widget->setParent(container_widget);
    blue_widget->setFillColor(sf::Color::Blue);
    blue_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    blue_widget->setMaxSize(15.0f, -1.0f);
    yellow_widget->setName("yellow");
    yellow_widget->setParent(container_widget);
    yellow_widget->setFillColor(sf::Color::Yellow);
    yellow_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    yellow_widget->setMaxSize(500.0f, -1.0f);
    magenta_widget->setName("purple");
    magenta_widget->setParent(container_widget);
    magenta_widget->setFillColor(sf::Color::Magenta);
    magenta_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    auto rect_to_str = &WidgetTests::floatRectToStr;

    float red_x = container_padding;
    float green_x = container_padding * 2 + child_size.x;
    float padding_size = container_padding * (container_widget->getChildren().size() + 1);
    float free_space = container_size.x - child_size.x - green_widget->getMinSize().x - blue_widget->getMaxSize().x - padding_size;
    float exp_delta = free_space / 3;
    float green_width = green_widget->getMinSize().x + exp_delta;
    float blue_x = green_x + green_width + container_padding;
    float blue_width = blue_widget->getMaxSize().x;
    float yellow_x = blue_x + blue_width + container_padding;
    float yellow_width = exp_delta;
    float magenta_x = yellow_x + yellow_width + container_padding;
    float magenta_width = exp_delta;
    application.advance();
    T_COMPARE(container_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, container_size.x, child_size.y + container_padding * 2), rect_to_str);
    T_COMPARE(red_widget->getParentLocalBounds(), sf::FloatRect(red_x, container_padding, child_size.x, child_size.y), rect_to_str);
    T_COMPARE(green_widget->getParentLocalBounds(), sf::FloatRect(green_x, container_padding, green_width, child_size.y), rect_to_str);
    T_COMPARE(blue_widget->getParentLocalBounds(), sf::FloatRect(blue_x, container_padding, blue_width, child_size.y), rect_to_str);
    T_COMPARE(yellow_widget->getParentLocalBounds(), sf::FloatRect(yellow_x, container_padding, yellow_width, child_size.y), rect_to_str);
    T_COMPARE(magenta_widget->getParentLocalBounds(), sf::FloatRect(magenta_x, container_padding, magenta_width, child_size.y), rect_to_str);
}

void WidgetTests::sizePolicyCombinedTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f parent_size(200.0f, 150.0f);
    sf::Vector2f container_size(100.0f, 50.0f);
    sf::Vector2f child_size(30.0f, 30.0f);
    float container_padding = 10.0f;
    fw::RectangleWidget* parent_widget = application.getWidgets().createRectangleWidget(parent_size);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::ContainerWidget* inner_container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::RectangleWidget* red_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* green_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* blue_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* yellow_widget = application.getWidgets().createRectangleWidget(child_size);
    parent_widget->setName("parent");
    parent_widget->setFillColor(sf::Color(128, 128, 128));
    container_widget->setName("container");
    container_widget->setHorizontal(false);
    container_widget->setPadding(container_padding);
    container_widget->setFillColor(sf::Color(100, 100, 100));
    container_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    container_widget->setSizeYPolicy(fw::Widget::SizePolicy::PARENT);
    container_widget->setParent(parent_widget);
    inner_container_widget->setName("inner container");
    inner_container_widget->setPadding(container_padding);
    inner_container_widget->setFillColor(sf::Color(75, 75, 75));
    inner_container_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    inner_container_widget->setParent(container_widget);
    red_widget->setName("red");
    red_widget->setParent(inner_container_widget);
    red_widget->setFillColor(sf::Color::Red);
    green_widget->setName("green");
    green_widget->setParent(inner_container_widget);
    green_widget->setFillColor(sf::Color::Green);
    green_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    blue_widget->setName("blue");
    blue_widget->setParent(inner_container_widget);
    blue_widget->setFillColor(sf::Color::Blue);
    yellow_widget->setName("yellow");
    yellow_widget->setParent(container_widget);
    yellow_widget->setFillColor(sf::Color::Yellow);
    yellow_widget->setSizeYPolicy(fw::Widget::SizePolicy::EXPAND);
    auto rect_to_str = &WidgetTests::floatRectToStr;

    float red_x = container_padding;
    float green_x = container_padding * 2 + child_size.x;
    float fixed_widgets_size = child_size.x * 2;
    float inner_padding_size = container_padding * (inner_container_widget->getChildren().size() + 1);
    float inner_container_width = parent_size.x - container_padding * 2;
    float green_width = inner_container_width - fixed_widgets_size - inner_padding_size;
    float blue_x = green_x + green_width + container_padding;
    float yellow_y = child_size.y + container_padding * 4;
    float yellow_height = parent_size.y - child_size.y - container_padding * 5;
    application.advance();
    T_COMPARE(parent_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(container_widget->getParentLocalBounds(), sf::FloatRect(0.0f, 0.0f, parent_size.x, parent_size.y), rect_to_str);
    T_COMPARE(inner_container_widget->getParentLocalBounds(), sf::FloatRect(
        container_padding, container_padding, parent_size.x - container_padding * 2, child_size.y + container_padding * 2), rect_to_str
    );
    T_COMPARE(red_widget->getParentLocalBounds(), sf::FloatRect(red_x, container_padding, child_size.x, child_size.y), rect_to_str);
    T_COMPARE(green_widget->getParentLocalBounds(), sf::FloatRect(green_x, container_padding, green_width, child_size.y), rect_to_str);
    T_COMPARE(blue_widget->getParentLocalBounds(), sf::FloatRect(blue_x, container_padding, child_size.x, child_size.y), rect_to_str);
    T_COMPARE(yellow_widget->getParentLocalBounds(), sf::FloatRect(container_padding, yellow_y, child_size.x, yellow_height), rect_to_str);
}

void WidgetTests::widgetLinkBasicTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    sf::Vector2f size(30.0f, 30.0f);
    sf::Vector2f position1(100.0f, 100.0f);
    sf::Vector2f position1_1(50.0f, 50.0f);
    sf::Vector2f position1_2(position1.x + size.x, position1.y);
    fw::RectangleWidget* rectangle_1_widget = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_2_widget = application.getWidgets().createRectangleWidget(size);
    rectangle_1_widget->setName("rect1");
    rectangle_1_widget->setPosition(position1);
    rectangle_2_widget->setName("rect2");
    rectangle_2_widget->setPosition(position1_1);
    rectangle_2_widget->addLink(
        "POS_X",
        { rectangle_1_widget->getPosXTarget(), rectangle_1_widget->getSizeXTarget() },
        [=]() {
            rectangle_2_widget->setGlobalPositionX(rectangle_1_widget->getGlobalTopRight().x);
        }
    );
    rectangle_2_widget->addLink(
        "POS_Y",
        rectangle_1_widget->getPosYTarget(),
        [=]() {
            rectangle_2_widget->setGlobalPositionY(rectangle_1_widget->getGlobalTopRight().y);
        }
    );
    application.advance();
    T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position1_2);
}

void WidgetTests::widgetLinkTargetsFuncTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    sf::Vector2f size(30.0f, 30.0f);
    sf::Vector2f position1(100.0f, 100.0f);
    sf::Vector2f position1_1(50.0f, 50.0f);
    sf::Vector2f position1_2(position1.x + size.x, position1.y);
    fw::RectangleWidget* rectangle_1_widget = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_2_widget = application.getWidgets().createRectangleWidget(size);
    rectangle_1_widget->setName("rect1");
    rectangle_1_widget->setPosition(position1);
    rectangle_2_widget->setName("rect2");
    rectangle_2_widget->setPosition(position1_1);
    rectangle_2_widget->addLink(
        "POS_X",
        [=]() {
            return CompVector<fw::WidgetUpdateTarget*>{ rectangle_1_widget->getPosXTarget(), rectangle_1_widget->getSizeXTarget() };
        },
        [=]() {
            rectangle_2_widget->setGlobalPositionX(rectangle_1_widget->getGlobalTopRight().x);
        }
    );
    rectangle_2_widget->addLink(
        "POS_Y",
        [=]() {
            return CompVector<fw::WidgetUpdateTarget*>{ rectangle_1_widget->getPosYTarget() };
        },
        [=]() {
            rectangle_2_widget->setGlobalPositionY(rectangle_1_widget->getGlobalTopRight().y);
        }
    );
    application.advance();
    T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position1_2);
}

void WidgetTests::widgetLinkContainerTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    float container_padding = 10.0f;
    sf::Vector2f size(30.0f, 30.0f);
    sf::Vector2f position1(100.0f, 100.0f);
    sf::Vector2f position1_1(50.0f, 50.0f);
    sf::Vector2f position1_2(position1.x + container_padding + size.x, position1.y + container_padding);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(size);
    fw::RectangleWidget* rectangle_1_widget = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_2_widget = application.getWidgets().createRectangleWidget(size);
    container_widget->setPosition(position1);
    container_widget->setPadding(container_padding);
    rectangle_1_widget->setName("rect1");
    rectangle_1_widget->setPosition(position1);
    rectangle_1_widget->setParent(container_widget);
    rectangle_2_widget->setName("rect2");
    rectangle_2_widget->setPosition(position1_1);
    rectangle_2_widget->addLink(
        "POS_X",
        { rectangle_1_widget->getPosXTarget(), rectangle_1_widget->getSizeXTarget() },
        [=]() {
            rectangle_2_widget->setGlobalPositionX(rectangle_1_widget->getGlobalTopRight().x);
        }
    );
    rectangle_2_widget->addLink(
        "POS_Y",
        rectangle_1_widget->getPosYTarget(),
        [=]() {
            rectangle_2_widget->setGlobalPositionY(rectangle_1_widget->getGlobalTopRight().y);
        }
    );
    application.advance();
    T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position1_2);
}

void WidgetTests::widgetLinkRemoveTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    sf::Vector2f size(30.0f, 30.0f);
    sf::Vector2f position1(100.0f, 100.0f);
    sf::Vector2f position2(200.0f, 300.0f);
    sf::Vector2f position1_1(50.0f, 50.0f);
    sf::Vector2f position1_2(position1.x + size.x, position1.y);
    sf::Vector2f position2_1(position1.x + size.x, position2.y);
    sf::Vector2f position3(400.0f, 400.0f);
    fw::RectangleWidget* rectangle_1_widget = application.getWidgets().createRectangleWidget(size);
    fw::RectangleWidget* rectangle_2_widget = application.getWidgets().createRectangleWidget(size);
    rectangle_1_widget->setName("rect1");
    rectangle_1_widget->setPosition(position1);
    rectangle_2_widget->setName("rect2");
    rectangle_2_widget->setPosition(position1_1);
    fw::WidgetLink* link1 = rectangle_2_widget->addLink(
        "POS_X",
        { rectangle_1_widget->getPosXTarget(), rectangle_1_widget->getSizeXTarget() },
        [=]() {
            rectangle_2_widget->setGlobalPositionX(rectangle_1_widget->getGlobalTopRight().x);
        }
    );
    fw::WidgetLink* link2 = rectangle_2_widget->addLink(
        "POS_Y",
        rectangle_1_widget->getPosYTarget(),
        [=]() {
            rectangle_2_widget->setGlobalPositionY(rectangle_1_widget->getGlobalTopRight().y);
        }
    );
    fw::WidgetLink* link3 = rectangle_2_widget->addLink(
        "LINK_3",
        link1,
        [=]() {
            // nothing
        }
    );
    fw::WidgetLink* link4 = rectangle_2_widget->addLink(
        "LINK_4",
        link2,
        [=]() {
            // nothing
        }
    );
    application.advance();
    T_ASSERT(T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position1_2));

    link1->remove();
    rectangle_1_widget->setPosition(position2);
    application.advance();
    T_ASSERT(T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position2_1));

    rectangle_1_widget->remove();
    rectangle_2_widget->setPosition(position3);
    application.advance();
    T_ASSERT(T_VEC2_APPROX_COMPARE(rectangle_2_widget->getPosition(), position3));
}

void WidgetTests::textboxWidgetBasicTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::TextBoxWidget* textbox_widget = application.getWidgets().createTextBoxWidget();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(textbox_widget));
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(40.0f, 20.0f);
    textbox_widget->setPosition(position);
    textbox_widget->setSize(size);

    GenericWidgetTest gwt(application, test);
    gwt.widget = textbox_widget;
    gwt.total_widgets = 5;
    gwt.type = fw::Widget::WidgetType::Textbox;
    gwt.name = "textbox";
    gwt.fullname = "root|textbox";
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
    gwt.focusable_type = fw::Widget::FocusableType::NORMAL;
    gwt.is_focused = false;
    gwt.clip_children = true;
    gwt.force_custom_cursor = true;
    gwt.parent = root_widget;
    gwt.local_bounds = sf::FloatRect(sf::Vector2f(), size);
    gwt.global_bounds = sf::FloatRect(position, size);
    gwt.parent_local_bounds = gwt.global_bounds;
    gwt.visual_local_bounds = gwt.local_bounds;
    gwt.visual_global_bounds = gwt.global_bounds;
    gwt.visual_parent_local_bounds = gwt.global_bounds;
    T_WRAP_CONTAINER(genericWidgetTest(gwt));

    T_COMPARE(textbox_widget->getFillColor(), sf::Color(50, 50, 50), &WidgetTests::colorToStr);
}

void WidgetTests::textboxWidgetInputTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 80.0f, 20.0f);
    CLICK_MOUSE(textbox_widget->getGlobalCenter());

    T_CHECK(textbox_widget->isFocused());
    T_CHECK(application.getWidgets().getFocusedWidget() == textbox_widget);
    T_CHECK(textbox_widget->isEditMode());
    T_COMPARE(textbox_widget->getValue(), "Text");
    T_CHECK(textbox_widget->isValidValue());
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    ENTER_TEXT(sf::Keyboard::BackSpace, '\b');
    T_COMPARE(textbox_widget->getValue(), "");
    CHECK_SELECTION(false, "", 0, -1, -1);
    ENTER_TEXT(sf::Keyboard::A, 'a');
    T_COMPARE(textbox_widget->getValue(), "a");
    CHECK_SELECTION(false, "", 1, -1, -1);
    ENTER_TEXT(sf::Keyboard::B, 'b');
    T_COMPARE(textbox_widget->getValue(), "ab");
    CHECK_SELECTION(false, "", 2, -1, -1);
    ENTER_TEXT(sf::Keyboard::C, 'c');
    T_COMPARE(textbox_widget->getValue(), "abc");
    CHECK_SELECTION(false, "", 3, -1, -1);
    ENTER_TEXT(sf::Keyboard::Enter, '\n');
    T_CHECK(!textbox_widget->isEditMode());
    T_COMPARE(textbox_widget->getValue(), "abc");
    T_CHECK(textbox_widget->isFocused());
    T_CHECK(application.getWidgets().getFocusedWidget() == textbox_widget);
}

void WidgetTests::textboxWidgetEventsTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 80.0f, 20.0f);
    bool edit_mode = false;
    sf::String value;
    bool confirmed = false;
    bool cancelled = false;
    textbox_widget->OnEditModeToggle = [&](bool value) {
        edit_mode = value;
    };
    textbox_widget->OnValueChanged = [&](const sf::String& new_value) {
        value = new_value;
    };
    textbox_widget->OnConfirm = [&](const sf::String& value) {
        confirmed = true;
    };
    textbox_widget->OnCancel = [&]() {
        cancelled = true;
    };
    CLICK_MOUSE(textbox_widget->getGlobalCenter());
    T_CHECK(edit_mode);
    CLICK_MOUSE(textbox_widget->getGlobalTopRight() + sf::Vector2f(10.0f, 0.0f));
    T_CHECK(!edit_mode);
    CLICK_MOUSE(textbox_widget->getGlobalCenter());
    T_CHECK(edit_mode);
    ENTER_TEXT(sf::Keyboard::A, 'a');
    T_COMPARE(value, "a");
    ENTER_TEXT(sf::Keyboard::A, 'b');
    T_COMPARE(value, "ab");
    ENTER_TEXT(sf::Keyboard::A, 'c');
    T_COMPARE(value, "abc");
    ENTER_TEXT(sf::Keyboard::Enter, '\n');
    T_CHECK(!edit_mode);
    T_CHECK(confirmed);
    CLICK_MOUSE(textbox_widget->getGlobalCenter());
    T_CHECK(edit_mode);
    TAP_KEY(sf::Keyboard::Escape);
    T_CHECK(!edit_mode);
    T_CHECK(cancelled);
}

void WidgetTests::textboxWidgetCursorTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 80.0f, 20.0f);
    CLICK_MOUSE(textbox_widget->getGlobalCenter());

    ENTER_TEXT(sf::Keyboard::BackSpace, '\b');
    TAP_KEY(sf::Keyboard::Left);
    T_COMPARE(textbox_widget->getValue(), "");
    CHECK_SELECTION(false, "", 0, -1, -1);
    TAP_KEY(sf::Keyboard::Right);
    T_COMPARE(textbox_widget->getValue(), "");
    CHECK_SELECTION(false, "", 0, -1, -1);
    ENTER_TEXT(sf::Keyboard::A, 'a');
    ENTER_TEXT(sf::Keyboard::B, 'b');
    ENTER_TEXT(sf::Keyboard::C, 'c');
    ENTER_TEXT(sf::Keyboard::D, 'd');
    T_ASSERT(T_COMPARE(textbox_widget->getValue(), "abcd"));
    CHECK_SELECTION(false, "", 4, -1, -1);
    auto move_cursor = [&](sf::Keyboard::Key key, size_t pos) {
        application.keyPress(key);
        application.advance();
        T_COMPARE(textbox_widget->getValue(), "abcd");
        CHECK_SELECTION(false, "", pos, -1, -1);
    };
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 4));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 4));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 3));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 2));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 1));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 0));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 0));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Left, 0));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 1));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 2));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 3));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 4));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 4));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Right, 4));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Home, 0));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::Home, 0));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::End, 4));
    T_WRAP_CONTAINER(move_cursor(sf::Keyboard::End, 4));
}

void WidgetTests::textboxWidgetScrollTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 20.0f, 20.0f);
    CLICK_MOUSE(textbox_widget->getGlobalCenter());

    const fw::TextWidget* text_widget = textbox_widget->getTextWidget();
    float zero_pos = fw::TEXTBOX_TEXT_VIEW_ZERO_POS.x;
    float right_margin = textbox_widget->getWidth() - fw::TEXTBOX_CURSOR_MOVE_MARGIN;
    auto calc_text_pos = [&](size_t cursor_pos_index) {
        float cursor_pos = zero_pos + text_widget->getLocalCharPos(cursor_pos_index).x;
        float past_right_margin = std::max(0.0f, cursor_pos - right_margin);
        float text_pos = zero_pos - past_right_margin;
        return text_pos;
    };
    T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(4));
    TAP_KEY(sf::Keyboard::Home);
    T_APPROX_COMPARE(text_widget->getPosition().x, zero_pos);

    T_ASSERT_NO_ERRORS();
    TAP_KEY(sf::Keyboard::Right);
    T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(1));
    TAP_KEY(sf::Keyboard::Right);
    T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(2));
    TAP_KEY(sf::Keyboard::Right);
    T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(3));
    TAP_KEY(sf::Keyboard::Right);
    T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(4));
    TAP_KEY(sf::Keyboard::Right);
    T_APPROX_COMPARE(text_widget->getPosition().x, calc_text_pos(4));

    T_ASSERT_NO_ERRORS();
    float begin_pos = text_widget->getPosition().x;
    TAP_KEY(sf::Keyboard::Left);
    T_COMPARE(text_widget->getPosition().x, begin_pos);
    TAP_KEY(sf::Keyboard::Left);
    T_COMPARE(textbox_widget->getLocalCharPos(2).x, zero_pos);
    TAP_KEY(sf::Keyboard::Left);
    T_COMPARE(textbox_widget->getLocalCharPos(1).x, zero_pos);
    TAP_KEY(sf::Keyboard::Left);
    T_COMPARE(textbox_widget->getLocalCharPos(0).x, zero_pos);
    TAP_KEY(sf::Keyboard::Left);
    T_COMPARE(textbox_widget->getLocalCharPos(0).x, zero_pos);
}

void WidgetTests::textboxWidgetResizingTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 150.0f, 20.0f);
    textbox_widget->setValue("ABCDEFGH");
    const fw::TextWidget* text_widget = textbox_widget->getTextWidget();
    float zero_pos = fw::TEXTBOX_TEXT_VIEW_ZERO_POS.x;

    application.advance();
    T_COMPARE(text_widget->getPosition().x, zero_pos);

    textbox_widget->setWidth(10.0f);
    application.advance();
    T_COMPARE(text_widget->getPosition().x, zero_pos);

    textbox_widget->setWidth(0.0f);
    application.advance();
    T_COMPARE(text_widget->getPosition().x, zero_pos);
    application.advance();
    T_COMPARE(text_widget->getPosition().x, zero_pos);
}

void WidgetTests::textboxWidgetSelectionTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 80.0f, 20.0f);
    CLICK_MOUSE(textbox_widget->getGlobalCenter());

    CHECK_SELECTION(true, "Text", 4, 0, 4);
    TAP_KEY(sf::Keyboard::Left);
    CHECK_SELECTION(false, "", 0, -1, -1);

    T_ASSERT_NO_ERRORS();
    SELECT_ALL();
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    TAP_KEY(sf::Keyboard::Right);
    CHECK_SELECTION(false, "", 4, -1, -1);
    SELECT_ALL();
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    TAP_KEY(sf::Keyboard::Home);
    CHECK_SELECTION(false, "", 0, -1, -1);
    SELECT_ALL();
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    TAP_KEY(sf::Keyboard::End);
    CHECK_SELECTION(false, "", 4, -1, -1);

    T_ASSERT_NO_ERRORS();
    TAP_KEY(sf::Keyboard::Home);
    PRESS_KEY(sf::Keyboard::LShift);
    TAP_KEY(sf::Keyboard::Right);
    CHECK_SELECTION(true, "T", 1, 0, 1);
    TAP_KEY(sf::Keyboard::Right);
    CHECK_SELECTION(true, "Te", 2, 0, 2);
    TAP_KEY(sf::Keyboard::Right);
    CHECK_SELECTION(true, "Tex", 3, 0, 3);
    TAP_KEY(sf::Keyboard::Right);
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    TAP_KEY(sf::Keyboard::Right);
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    RELEASE_KEY(sf::Keyboard::LShift);
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    TAP_KEY(sf::Keyboard::Left);
    CHECK_SELECTION(false, "", 0, -1, -1);

    T_ASSERT_NO_ERRORS();
    TAP_KEY(sf::Keyboard::Right);
    TAP_KEY(sf::Keyboard::Right);
    TAP_KEY(sf::Keyboard::Right);
    PRESS_KEY(sf::Keyboard::LShift);
    TAP_KEY(sf::Keyboard::Left);
    CHECK_SELECTION(true, "x", 2, 2, 3);
    TAP_KEY(sf::Keyboard::Left);
    CHECK_SELECTION(true, "ex", 1, 1, 3);
    TAP_KEY(sf::Keyboard::Left);
    CHECK_SELECTION(true, "Tex", 0, 0, 3);
    RELEASE_KEY(sf::Keyboard::LShift);
    TAP_KEY(sf::Keyboard::Right);
    CHECK_SELECTION(false, "", 3, -1, -1);

    T_ASSERT_NO_ERRORS();
    TAP_KEY(sf::Keyboard::Home);
    TAP_KEY(sf::Keyboard::Right);
    PRESS_KEY(sf::Keyboard::LShift);
    TAP_KEY(sf::Keyboard::Right);
    TAP_KEY(sf::Keyboard::Right);
    RELEASE_KEY(sf::Keyboard::LShift);
    CHECK_SELECTION(true, "ex", 3, 1, 3);
    SELECT_ALL();
    CHECK_SELECTION(true, "Text", 4, 0, 4);
}

void WidgetTests::textboxWidgetMouseClickTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 80.0f, 20.0f);
    CLICK_MOUSE(textbox_widget->getGlobalCenter());

    auto click_at_char = [&](size_t index) {
        T_ASSERT_NO_ERRORS();
        SELECT_ALL();
        CHECK_SELECTION(true, "Text", 4, 0, 4);
        CLICK_MOUSE(textbox_widget->getGlobalCharPos(index));
        T_CHECK(textbox_widget->isFocused());
        CHECK_SELECTION(false, "", index, -1, -1);
    };
    T_WRAP_CONTAINER(click_at_char(0));
    T_WRAP_CONTAINER(click_at_char(1));
    T_WRAP_CONTAINER(click_at_char(2));
    T_WRAP_CONTAINER(click_at_char(3));
    T_WRAP_CONTAINER(click_at_char(4));

    T_ASSERT_NO_ERRORS();
    CLICK_MOUSE(textbox_widget->getGlobalTopRight() + sf::Vector2f(10.0f, 0.0f));
    CHECK_SELECTION(false, "", 0, -1, -1);
    T_CHECK(!textbox_widget->isFocused());
}

void WidgetTests::textboxWidgetMouseDragTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 80.0f, 20.0f);

    auto get_char_pos = [&](size_t index) {
        return textbox_widget->getGlobalCharPos(index);
    };
    auto move_to_char = [&](size_t index) {
        application.mouseMove(get_char_pos(index));
        application.advance();
    };
    auto drag_from_char = [&](size_t index) {
        application.mouseMove(get_char_pos(index));
        application.mouseLeftPress();
        application.advance();
    };
    auto mouse_release = [&]() {
        application.mouseLeftRelease();
        application.advance();
    };
    T_ASSERT(T_CHECK(!textbox_widget->isEditMode()));
    drag_from_char(1);
    T_ASSERT(T_CHECK(textbox_widget->isEditMode()));
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    move_to_char(2);
    CHECK_SELECTION(true, "e", 2, 1, 2);
    move_to_char(3);
    CHECK_SELECTION(true, "ex", 3, 1, 3);
    mouse_release();
    CHECK_SELECTION(true, "ex", 3, 1, 3);

    T_ASSERT_NO_ERRORS();
    drag_from_char(0);
    CHECK_SELECTION(false, "", 0, -1, -1);
    move_to_char(4);
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    mouse_release();
    CHECK_SELECTION(true, "Text", 4, 0, 4);

    T_ASSERT_NO_ERRORS();
    drag_from_char(4);
    CHECK_SELECTION(false, "", 4, -1, -1);
    move_to_char(0);
    CHECK_SELECTION(true, "Text", 0, 0, 4);
    mouse_release();
    CHECK_SELECTION(true, "Text", 0, 0, 4);
}

void WidgetTests::textboxWidgetCopyPasteTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 80.0f, 20.0f);
    CLICK_MOUSE(textbox_widget->getGlobalCenter());

    COPY();
    T_COMPARE(textbox_widget->getValue(), "Text");
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    T_COMPARE(textbox_widget->getValue(), "");
    PASTE();
    T_COMPARE(textbox_widget->getValue(), "Text");

    T_ASSERT_NO_ERRORS();
    SELECT_ALL();
    CUT();
    T_COMPARE(textbox_widget->getValue(), "");
    PASTE();
    T_COMPARE(textbox_widget->getValue(), "Text");

    T_ASSERT_NO_ERRORS();
    TAP_KEY(sf::Keyboard::Home);
    TAP_KEY(sf::Keyboard::Right);
    PRESS_KEY(sf::Keyboard::LShift);
    TAP_KEY(sf::Keyboard::Right);
    TAP_KEY(sf::Keyboard::Right);
    RELEASE_KEY(sf::Keyboard::LShift);
    COPY();
    T_COMPARE(textbox_widget->getValue(), "Text");
    TAP_KEY(sf::Keyboard::Home);
    PASTE();
    T_COMPARE(textbox_widget->getValue(), "exText");

    T_ASSERT_NO_ERRORS();
    PRESS_KEY(sf::Keyboard::LShift);
    TAP_KEY(sf::Keyboard::Right);
    TAP_KEY(sf::Keyboard::Right);
    RELEASE_KEY(sf::Keyboard::LShift);
    CUT();
    T_COMPARE(textbox_widget->getValue(), "exxt");
    TAP_KEY(sf::Keyboard::Left);
    PRESS_KEY(sf::Keyboard::LShift);
    TAP_KEY(sf::Keyboard::Right);
    TAP_KEY(sf::Keyboard::Right);
    RELEASE_KEY(sf::Keyboard::LShift);
    PASTE();
    T_COMPARE(textbox_widget->getValue(), "eTet");

    T_ASSERT_NO_ERRORS();
    COPY();
    PASTE();
    T_COMPARE(textbox_widget->getValue(), "eTeTet");
    CUT();
    PASTE();
    T_COMPARE(textbox_widget->getValue(), "eTeTeTet");

    T_ASSERT_NO_ERRORS();
    SELECT_ALL();
    CUT();
    COPY();
    PASTE();
    T_COMPARE(textbox_widget->getValue(), "eTeTeTet");
    CUT();
    PASTE();
    T_COMPARE(textbox_widget->getValue(), "eTeTeTeteTeTeTet");
}

void WidgetTests::textboxWidgetHistoryTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 80.0f, 20.0f);
    CLICK_MOUSE(textbox_widget->getGlobalCenter());

    auto undo = [&]() {
        PRESS_KEY(sf::Keyboard::LControl);
        ENTER_TEXT(sf::Keyboard::Z, 'z');
        RELEASE_KEY(sf::Keyboard::LControl);
    };
    auto redo = [&]() {
        PRESS_KEY(sf::Keyboard::LControl);
        PRESS_KEY(sf::Keyboard::LShift);
        ENTER_TEXT(sf::Keyboard::Z, 'z');
        RELEASE_KEY(sf::Keyboard::LShift);
        RELEASE_KEY(sf::Keyboard::LControl);
    };

    T_COMPARE(textbox_widget->getValue(), "Text");
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    T_COMPARE(textbox_widget->getValue(), "");
    undo();
    T_COMPARE(textbox_widget->getValue(), "Text");
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    redo();
    T_COMPARE(textbox_widget->getValue(), "");

    T_ASSERT_NO_ERRORS();
    ENTER_TEXT(sf::Keyboard::A, 'a');
    ENTER_TEXT(sf::Keyboard::B, 'b');
    ENTER_TEXT(sf::Keyboard::C, 'c');
    T_COMPARE(textbox_widget->getValue(), "abc");
    undo();
    T_COMPARE(textbox_widget->getValue(), "");
    undo();
    T_COMPARE(textbox_widget->getValue(), "Text");
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    undo();
    T_COMPARE(textbox_widget->getValue(), "Text");
    CHECK_SELECTION(true, "Text", 4, 0, 4);
    redo();
    T_COMPARE(textbox_widget->getValue(), "");
    redo();
    T_COMPARE(textbox_widget->getValue(), "abc");
    CHECK_SELECTION(false, "", 3, -1, -1);

    T_ASSERT_NO_ERRORS();
    undo();
    T_COMPARE(textbox_widget->getValue(), "");
    undo();
    T_COMPARE(textbox_widget->getValue(), "Text");
    TAP_KEY(sf::Keyboard::Left);
    TAP_KEY(sf::Keyboard::Right);
    TAP_KEY(sf::Keyboard::Right);
    TAP_KEY(sf::Keyboard::Right);
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    T_COMPARE(textbox_widget->getValue(), "Tt");
    undo();
    T_COMPARE(textbox_widget->getValue(), "Text");
    undo();
    T_COMPARE(textbox_widget->getValue(), "Text");
    CHECK_SELECTION(false, "", 3, -1, -1);

    T_ASSERT_NO_ERRORS();
    PRESS_KEY(sf::Keyboard::LShift);
    TAP_KEY(sf::Keyboard::Left);
    TAP_KEY(sf::Keyboard::Left);
    RELEASE_KEY(sf::Keyboard::LShift);
    CUT();
    T_COMPARE(textbox_widget->getValue(), "Tt");
    undo();
    T_COMPARE(textbox_widget->getValue(), "Text");
    undo();
    T_COMPARE(textbox_widget->getValue(), "Text");
    CHECK_SELECTION(true, "ex", 1, 1, 3);

    T_ASSERT_NO_ERRORS();
    T_COMPARE(textbox_widget->getValue(), "Text");
    TAP_KEY(sf::Keyboard::Right);
    PASTE();
    T_COMPARE(textbox_widget->getValue(), "Texext");
    PASTE();
    T_COMPARE(textbox_widget->getValue(), "Texexext");
    undo();
    T_COMPARE(textbox_widget->getValue(), "Texext");
    undo();
    T_COMPARE(textbox_widget->getValue(), "Text");
    undo();
    T_COMPARE(textbox_widget->getValue(), "Text");
    CHECK_SELECTION(false, "", 3, -1, -1);
}

void WidgetTests::textboxWidgetIntegerTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 80.0f, 20.0f);
    CLICK_MOUSE(textbox_widget->getGlobalCenter());

    T_CHECK(textbox_widget->isValidValue());
    textbox_widget->setType(fw::TextBoxWidget::TextBoxType::INTEGER);
    T_CHECK(!textbox_widget->isValidValue());
    SELECT_ALL();
    CUT();
    T_CHECK(!textbox_widget->isValidValue());
    // digits
    ENTER_TEXT(sf::Keyboard::Num1, '1');
    ENTER_TEXT(sf::Keyboard::Num2, '2');
    ENTER_TEXT(sf::Keyboard::Num3, '3');
    ENTER_TEXT(sf::Keyboard::Num4, '4');
    T_COMPARE(textbox_widget->getValue(), "1234");
    T_CHECK(textbox_widget->isValidValue());
    // letter e
    ENTER_TEXT(sf::Keyboard::E, 'e');
    T_COMPARE(textbox_widget->getValue(), "1234");
    T_CHECK(textbox_widget->isValidValue());
    // plus sign
    TAP_KEY(sf::Keyboard::Home);
    PRESS_KEY(sf::Keyboard::LShift);
    ENTER_TEXT(sf::Keyboard::Equal, '+');
    RELEASE_KEY(sf::Keyboard::LShift);
    T_COMPARE(textbox_widget->getValue(), "+1234");
    T_CHECK(textbox_widget->isValidValue());
    // two plus signs
    PRESS_KEY(sf::Keyboard::LShift);
    ENTER_TEXT(sf::Keyboard::Equal, '+');
    RELEASE_KEY(sf::Keyboard::LShift);
    T_COMPARE(textbox_widget->getValue(), "++1234");
    T_CHECK(!textbox_widget->isValidValue());
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    TAP_KEY(sf::Keyboard::End);
    ENTER_TEXT(sf::Keyboard::Period, '.');
    T_COMPARE(textbox_widget->getValue(), "+1234");
    // comma
    ENTER_TEXT(sf::Keyboard::Comma, ',');
    T_COMPARE(textbox_widget->getValue(), "+1234");
    T_CHECK(textbox_widget->isValidValue());
    // minus sign at the end
    ENTER_TEXT(sf::Keyboard::Dash, '-');
    T_COMPARE(textbox_widget->getValue(), "+1234-");
    T_CHECK(!textbox_widget->isValidValue());
    // all digits
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    ENTER_TEXT(sf::Keyboard::Num5, '5');
    ENTER_TEXT(sf::Keyboard::Num6, '6');
    ENTER_TEXT(sf::Keyboard::Num7, '7');
    ENTER_TEXT(sf::Keyboard::Num8, '8');
    ENTER_TEXT(sf::Keyboard::Num9, '9');
    T_COMPARE(textbox_widget->getValue(), "+123456789");
    T_CHECK(textbox_widget->isValidValue());
    // minus sign before plus sign
    TAP_KEY(sf::Keyboard::Home);
    ENTER_TEXT(sf::Keyboard::Dash, '-');
    T_COMPARE(textbox_widget->getValue(), "-+123456789");
    T_CHECK(!textbox_widget->isValidValue());
    // minus sign
    TAP_KEY(sf::Keyboard::Right);
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    T_COMPARE(textbox_widget->getValue(), "-123456789");
    T_CHECK(textbox_widget->isValidValue());
    // zero at the beginning
    ENTER_TEXT(sf::Keyboard::Num0, '0');
    T_COMPARE(textbox_widget->getValue(), "-0123456789");
    T_CHECK(textbox_widget->isValidValue());
}

void WidgetTests::textboxWidgetFloatTest(test::Test& test) {
    fw::Application application(window);
    fw::TextBoxWidget* textbox_widget = initTextBox(application, test.name, 80.0f, 20.0f);
    CLICK_MOUSE(textbox_widget->getGlobalCenter());

    T_CHECK(textbox_widget->isValidValue());
    textbox_widget->setType(fw::TextBoxWidget::TextBoxType::FLOAT);
    T_CHECK(!textbox_widget->isValidValue());
    SELECT_ALL();
    CUT();
    T_CHECK(!textbox_widget->isValidValue());
    // digits
    ENTER_TEXT(sf::Keyboard::Num1, '1');
    ENTER_TEXT(sf::Keyboard::Num2, '2');
    ENTER_TEXT(sf::Keyboard::Num3, '3');
    ENTER_TEXT(sf::Keyboard::Num4, '4');
    T_COMPARE(textbox_widget->getValue(), "1234");
    T_CHECK(textbox_widget->isValidValue());
    // plus sign
    TAP_KEY(sf::Keyboard::Home);
    PRESS_KEY(sf::Keyboard::LShift);
    ENTER_TEXT(sf::Keyboard::Equal, '+');
    RELEASE_KEY(sf::Keyboard::LShift);
    T_COMPARE(textbox_widget->getValue(), "+1234");
    T_CHECK(textbox_widget->isValidValue());
    // two plus signs
    PRESS_KEY(sf::Keyboard::LShift);
    ENTER_TEXT(sf::Keyboard::Equal, '+');
    RELEASE_KEY(sf::Keyboard::LShift);
    T_COMPARE(textbox_widget->getValue(), "++1234");
    T_CHECK(!textbox_widget->isValidValue());
    // minus sign at the end
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    TAP_KEY(sf::Keyboard::End);
    ENTER_TEXT(sf::Keyboard::Dash, '-');
    T_COMPARE(textbox_widget->getValue(), "+1234-");
    T_CHECK(!textbox_widget->isValidValue());
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    // letter a
    ENTER_TEXT(sf::Keyboard::A, 'a');
    T_COMPARE(textbox_widget->getValue(), "+1234");
    T_CHECK(textbox_widget->isValidValue());
    // letter e
    ENTER_TEXT(sf::Keyboard::E, 'e');
    T_COMPARE(textbox_widget->getValue(), "+1234e");
    T_CHECK(!textbox_widget->isValidValue());
    // exponent
    ENTER_TEXT(sf::Keyboard::Num5, '9');
    T_COMPARE(textbox_widget->getValue(), "+1234e9");
    T_CHECK(textbox_widget->isValidValue());
    // big exponent
    ENTER_TEXT(sf::Keyboard::Num5, '9');
    T_COMPARE(textbox_widget->getValue(), "+1234e99");
    T_CHECK(!textbox_widget->isValidValue());
    // letter e after the end
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    ENTER_TEXT(sf::Keyboard::Comma, ',');
    T_COMPARE(textbox_widget->getValue(), "+1234e9");
    ENTER_TEXT(sf::Keyboard::E, 'e');
    T_COMPARE(textbox_widget->getValue(), "+1234e9e");
    T_CHECK(!textbox_widget->isValidValue());
    // dot after the end
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    ENTER_TEXT(sf::Keyboard::Period, '.');
    T_COMPARE(textbox_widget->getValue(), "+1234e9.");
    T_CHECK(!textbox_widget->isValidValue());
    // dot in exponent
    ENTER_TEXT(sf::Keyboard::Num6, '6');
    T_COMPARE(textbox_widget->getValue(), "+1234e9.6");
    T_CHECK(!textbox_widget->isValidValue());
    // dot in mantissa
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    T_COMPARE(textbox_widget->getValue(), "+1234e9");
    TAP_KEY(sf::Keyboard::Left);
    TAP_KEY(sf::Keyboard::Left);
    TAP_KEY(sf::Keyboard::Left);
    ENTER_TEXT(sf::Keyboard::Period, '.');
    T_COMPARE(textbox_widget->getValue(), "+123.4e9");
    T_CHECK(textbox_widget->isValidValue());
    // two dots in mantissa
    ENTER_TEXT(sf::Keyboard::Period, '.');
    T_COMPARE(textbox_widget->getValue(), "+123..4e9");
    T_CHECK(!textbox_widget->isValidValue());
    // more digist in mantissa
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    T_COMPARE(textbox_widget->getValue(), "+123.4e9");
    TAP_KEY(sf::Keyboard::Right);
    ENTER_TEXT(sf::Keyboard::Num5, '5');
    ENTER_TEXT(sf::Keyboard::Num6, '6');
    ENTER_TEXT(sf::Keyboard::Num7, '7');
    ENTER_TEXT(sf::Keyboard::Num8, '8');
    T_COMPARE(textbox_widget->getValue(), "+123.45678e9");
    T_CHECK(textbox_widget->isValidValue());
    // minus sign before plus sign
    TAP_KEY(sf::Keyboard::Home);
    ENTER_TEXT(sf::Keyboard::Dash, '-');
    T_COMPARE(textbox_widget->getValue(), "-+123.45678e9");
    T_CHECK(!textbox_widget->isValidValue());
    // zero in the beginning of mantissa
    TAP_KEY(sf::Keyboard::Right);
    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
    ENTER_TEXT(sf::Keyboard::Num0, '0');
    T_COMPARE(textbox_widget->getValue(), "-0123.45678e9");
    T_CHECK(textbox_widget->isValidValue());
    // two minus signs at the beginning
    TAP_KEY(sf::Keyboard::Home);
    ENTER_TEXT(sf::Keyboard::Dash, '-');
    T_COMPARE(textbox_widget->getValue(), "--0123.45678e9");
    T_CHECK(!textbox_widget->isValidValue());

    ENTER_TEXT(sf::Keyboard::Backspace, '\b');
}

void WidgetTests::canvasWidgetBasicTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f size(100.0f, 100.0f);
    fw::CanvasWidget* canvas_widget =
        application.getWidgets().createCanvasWidget(size, sf::Vector2u(100, 100));
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(canvas_widget));
    sf::Vector2f position(100.0f, 100.0f);
    canvas_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = canvas_widget;
    gwt.total_widgets = 2;
    gwt.type = fw::Widget::WidgetType::Canvas;
    gwt.name = "canvas";
    gwt.fullname = "root|canvas";
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
    gwt.is_click_through = true;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
    gwt.is_focused = false;
    gwt.clip_children = false;
    gwt.force_custom_cursor = false;
    gwt.parent = root_widget;
    gwt.local_bounds = sf::FloatRect(sf::Vector2f(), size);
    gwt.global_bounds = sf::FloatRect(position, size);
    gwt.parent_local_bounds = gwt.global_bounds;
    gwt.visual_local_bounds = gwt.local_bounds;
    gwt.visual_global_bounds = gwt.global_bounds;
    gwt.visual_parent_local_bounds = gwt.global_bounds;
    T_WRAP_CONTAINER(genericWidgetTest(gwt));

    T_COMPARE(canvas_widget->getChildren().size(), 0);
}

void WidgetTests::canvasWidgetDrawTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::CanvasWidget* canvas_widget = application.getWidgets().createCanvasWidget(100.0f, 100.0f, 100, 100);
    auto color_to_str = &WidgetTests::colorToStr;
    {
        canvas_widget->clear();
        sf::Image image = canvas_widget->getRenderTexture().getTexture().copyToImage();
        T_ASSERT(T_COMPARE(image.getPixel(0, 0), sf::Color::Black, color_to_str));
    }
    {
        canvas_widget->clear(sf::Color(128, 128, 128));
        sf::Image image = canvas_widget->getRenderTexture().getTexture().copyToImage();
        T_ASSERT(T_COMPARE(image.getPixel(0, 0), sf::Color(128, 128, 128), color_to_str));
    }
    {
        canvas_widget->clear(sf::Color::Red);
        sf::RectangleShape rect(sf::Vector2f(30.0f, 30.0f));
        rect.setPosition(10.0f, 10.0f);
        rect.setFillColor(sf::Color::Green);
        canvas_widget->draw(rect);
        canvas_widget->display();
        sf::Image image = canvas_widget->getRenderTexture().getTexture().copyToImage();
        T_COMPARE(image.getPixel(0, 0), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(5, 5), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(10, 10), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(15, 15), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(20, 20), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(25, 25), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(30, 30), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(35, 35), sf::Color::Green, color_to_str);
        T_COMPARE(image.getPixel(40, 40), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(45, 45), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(50, 50), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(55, 55), sf::Color::Red, color_to_str);
        T_COMPARE(image.getPixel(60, 60), sf::Color::Red, color_to_str);
    }
}

void WidgetTests::canvasWidgetAlphaTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    fw::CanvasWidget* canvas_widget = application.getWidgets().createCanvasWidget(100.0f, 100.0f, 100, 100);
    auto color_to_str = &WidgetTests::colorToStr;
    {
        sf::RectangleShape rect(sf::Vector2f(100.0f, 100.0f));
        canvas_widget->clear();
        rect.setFillColor(sf::Color(255, 0, 0, 128));
        canvas_widget->draw(rect);
        rect.setFillColor(sf::Color(0, 255, 0, 128));
        canvas_widget->draw(rect);
        rect.setFillColor(sf::Color(0, 0, 255, 128));
        canvas_widget->draw(rect);
        sf::Image image = canvas_widget->getRenderTexture().getTexture().copyToImage();
        T_ASSERT(T_COMPARE(image.getPixel(0, 0), sf::Color(32, 64, 128), color_to_str));
    }
}

void WidgetTests::windowWidgetBasicTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(100.0f, 100.0f);
    fw::WindowWidget* window_widget = application.getWidgets().createWindowWidget(size);
    sf::Vector2f total_size = size + sf::Vector2f(0.0f, fw::WINDOW_HEADER_HEIGHT);
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(window_widget));
    window_widget->setPosition(position);
    window_widget->setHeaderFont(textbox_font);
    application.advance();
    sf::Vector2f header_size = sf::Vector2f(size.x, fw::WINDOW_HEADER_HEIGHT);

    {
        GenericWidgetTest gwt(application, test);
        gwt.widget = window_widget;
        gwt.total_widgets = 7;
        gwt.type = fw::Widget::WidgetType::Window;
        gwt.name = "window";
        gwt.fullname = "root|window";
        gwt.is_visual_position_quantized = false;
        fw::WidgetVisibility visibility;
        visibility.addedToRoot = true;
        visibility.allParentsVisible = true;
        visibility.hasUnclippedRegion = true;
        visibility.nonZeroSize = true;
        visibility.onScreen = true;
        visibility.opaque = false;
        visibility.renderableSetting = false;
        visibility.visibleSetting = true;
        gwt.visibility = visibility;
        gwt.is_click_through = true;
        gwt.is_mouse_over = false;
        gwt.focusable_type = fw::Widget::FocusableType::NONE;
        gwt.is_focused = false;
        gwt.clip_children = false;
        gwt.force_custom_cursor = false;
        gwt.parent = root_widget;
        gwt.local_bounds = sf::FloatRect(sf::Vector2f(), total_size);
        gwt.global_bounds = sf::FloatRect(position, total_size);
        gwt.parent_local_bounds = gwt.global_bounds;
        gwt.visual_local_bounds = gwt.local_bounds;
        gwt.visual_global_bounds = gwt.global_bounds;
        gwt.visual_parent_local_bounds = gwt.global_bounds;
        T_WRAP_CONTAINER(genericWidgetTest(gwt));
    }

    fw::RectangleWidget* header_widget = nullptr;
    fw::RectangleWidget* main_widget = nullptr;
    fw::EmptyWidget* resize_widget = nullptr;
    fw::RectangleWidget* outline_widget = nullptr;
    T_ASSERT(T_COMPARE(window_widget->getChildren().size(), 4));
    const CompVector<fw::Widget*>& children = window_widget->getChildren();
    header_widget = dynamic_cast<fw::RectangleWidget*>(window_widget->getChild(0));
    main_widget = dynamic_cast<fw::RectangleWidget*>(window_widget->getChild(1));
    resize_widget = dynamic_cast<fw::EmptyWidget*>(window_widget->getChild(2));
    outline_widget = dynamic_cast<fw::RectangleWidget*>(window_widget->getChild(3));
    T_CHECK(main_widget);
    T_CHECK(resize_widget);
    T_CHECK(outline_widget);

    T_COMPARE(window_widget->getFillColor(), sf::Color::Transparent, &WidgetTests::colorToStr);

    T_ASSERT_NO_ERRORS();
    {
        GenericWidgetTest gwt(application, test);
        gwt.widget = header_widget;
        gwt.total_widgets = 7;
        gwt.type = fw::Widget::WidgetType::Rectangle;
        gwt.name = "header";
        gwt.fullname = "root|window|header";
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
        gwt.parent = window_widget;
        gwt.local_bounds = sf::FloatRect(sf::Vector2f(), header_size);
        gwt.global_bounds = sf::FloatRect(position, header_size);
        gwt.parent_local_bounds = gwt.local_bounds;
        gwt.visual_local_bounds = gwt.local_bounds;
        gwt.visual_global_bounds = gwt.global_bounds;
        gwt.visual_parent_local_bounds = gwt.local_bounds;
        T_WRAP_CONTAINER(genericWidgetTest(gwt));
    }

    if (T_COMPARE(header_widget->getChildren().size(), 1)) {
        fw::TextWidget* text_widget = dynamic_cast<fw::TextWidget*>(header_widget->getChild(0));
        T_CHECK(text_widget);
    }

    T_COMPARE(header_widget->getFillColor(), fw::WINDOW_HEADER_DEFAULT_COLOR, &WidgetTests::colorToStr);

    {
        GenericWidgetTest gwt(application, test);
        gwt.widget = main_widget;
        gwt.total_widgets = 7;
        gwt.type = fw::Widget::WidgetType::Rectangle;
        gwt.name = "main";
        gwt.fullname = "root|window|main";
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
        gwt.force_custom_cursor = true;
        gwt.parent = window_widget;
        gwt.local_bounds = sf::FloatRect(sf::Vector2f(), size);
        gwt.global_bounds = sf::FloatRect(position + sf::Vector2f(0.0f, header_widget->getHeight()), size);
        gwt.parent_local_bounds = sf::FloatRect(sf::Vector2f(0.0f, header_widget->getHeight()), size);
        gwt.visual_local_bounds = gwt.local_bounds;
        gwt.visual_global_bounds = gwt.global_bounds;
        gwt.visual_parent_local_bounds = gwt.parent_local_bounds;
        T_WRAP_CONTAINER(genericWidgetTest(gwt));
    }

    T_ASSERT(T_COMPARE(main_widget->getChildren().size(), 0));

    T_COMPARE(main_widget->getFillColor(), fw::WINDOW_DEFAULT_COLOR, &WidgetTests::colorToStr);
}

void WidgetTests::windowWidgetDragTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(100.0f, 100.0f);
    fw::WindowWidget* window_widget = application.getWidgets().createWindowWidget(size);
    window_widget->setPosition(position);
    window_widget->setHeaderFont(textbox_font);
    sf::Vector2f header_center = window_widget->getHeaderWidget()->getGlobalCenter();
    sf::Vector2f offset = sf::Vector2f(50.0f, 30.0f);
    sf::Vector2f new_position = position + offset;
    dragWindow(application, window_widget, offset);
    T_VEC2_APPROX_COMPARE(window_widget->getGlobalPosition(), new_position);
}

void WidgetTests::windowWidgetChildrenTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(100.0f, 100.0f);
    fw::WindowWidget* window_widget = application.getWidgets().createWindowWidget(size);
    window_widget->setPosition(position);
    window_widget->setHeaderFont(textbox_font);
    fw::RectangleWidget* rect_widget = application.getWidgets().createRectangleWidget(30.0f, 30.0f);
    rect_widget->setFillColor(sf::Color::Green);
    rect_widget->setPosition(10.0f, 10.0f);
    rect_widget->setParent(window_widget);
    const CompVector<fw::Widget*>& window_children = window_widget->getWindowChildren();
    if (T_COMPARE(window_children.size(), 1)) {
        T_CHECK(window_children[0] == rect_widget);
    }
}

void WidgetTests::windowWidgetResizeTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    sf::Vector2f position(400.0f, 300.0f);
    sf::Vector2f size(300.0f, 200.0f);
    fw::WindowWidget* window_widget = application.getWidgets().createWindowWidget(size);
    window_widget->setPosition(position);
    window_widget->setOrigin(fw::Widget::Anchor::CENTER);
    window_widget->setHeaderFont(textbox_font);
    application.advance();
    T_WRAP_CONTAINER(resizeWindowTest(application, test, window_widget));
}

void WidgetTests::windowWidgetChainTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();

    fw::WindowWidget* parent_window = application.getWidgets().createWindowWidget(300.0f, 250.0f);
    parent_window->setName("parent window");
    parent_window->setPosition(150.0f, 100.0f);
    parent_window->setHeaderFont(textbox_font);
    parent_window->setHeaderText("Parent window");
    parent_window->setHeaderTextCharacterSize(15);
    fw::RectangleWidget* red_rect = application.getWidgets().createRectangleWidget(30.0f, 30.0f);
    red_rect->setFillColor(sf::Color::Red);
    red_rect->setName("red rect");
    red_rect->setParentAnchor(fw::Widget::Anchor::TOP_LEFT);
    red_rect->setAnchorOffset(10.0f, 10.0f);
    red_rect->setParent(parent_window);

    fw::WindowWidget* child_window = application.getWidgets().createWindowWidget(200.0f, 170.0f);
    child_window->setName("child window");
    child_window->setPosition(20.0f, 20.0f);
    child_window->setHeaderFont(textbox_font);
    child_window->setHeaderText("Child window");
    child_window->setHeaderTextCharacterSize(15);
    child_window->setParent(parent_window);
    fw::RectangleWidget* green_rect = application.getWidgets().createRectangleWidget(20.0f, 20.0f);
    green_rect->setFillColor(sf::Color::Green);
    green_rect->setName("green rect");
    green_rect->setParentAnchor(fw::Widget::Anchor::TOP_LEFT);
    green_rect->setAnchorOffset(10.0f, 10.0f);
    green_rect->setParent(child_window);

    fw::WindowWidget* another_child_window = application.getWidgets().createWindowWidget(80.0f, 60.0f);
    another_child_window->setName("another child window");
    another_child_window->setPosition(20.0f, 20.0f);
    another_child_window->setHeaderFont(textbox_font);
    another_child_window->setHeaderText("Another child window");
    another_child_window->setHeaderTextCharacterSize(15);
    another_child_window->setParent(child_window);
    fw::RectangleWidget* blue_rect = application.getWidgets().createRectangleWidget(20.0f, 20.0f);
    blue_rect->setFillColor(sf::Color::Blue);
    blue_rect->setName("blue rect");
    blue_rect->setParentAnchor(fw::Widget::Anchor::TOP_LEFT);
    blue_rect->setAnchorOffset(10.0f, 10.0f);
    blue_rect->setParent(another_child_window);

    application.advance();

    T_WRAP_CONTAINER(resizeWindowTest(application, test, another_child_window));
}

void WidgetTests::windowWidgetDragLimitsTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::WindowWidget* parent_window = application.getWidgets().createWindowWidget(300.0f, 250.0f);
    parent_window->setName("parent window");
    parent_window->setPosition(150.0f, 100.0f);
    parent_window->setHeaderFont(textbox_font);
    parent_window->setHeaderText("Parent window");
    parent_window->setHeaderTextCharacterSize(15);
    fw::WindowWidget* child_window = application.getWidgets().createWindowWidget(100.0f, 100.0f);
    child_window->setName("child window");
    child_window->setPosition(20.0f, 20.0f);
    child_window->setHeaderFont(textbox_font);
    child_window->setHeaderText("Child window");
    child_window->setHeaderTextCharacterSize(15);
    child_window->setParent(parent_window);
    application.advance();
    fw::RectangleWidget* parent_main_widget = parent_window->getMainWidget();
    float child_width = child_window->getWidth();
    float child_height = child_window->getHeight();
    float margin = fw::WINDOW_ONSCREEN_MARGIN;
    float parent_width = parent_main_widget->getWidth();
    float parent_height = parent_main_widget->getHeight();
    float parent_width_n = -child_width - 10.0f;
    float parent_width_p = parent_width + child_width + 10.0f;
    float parent_height_n = -child_height - 10.0f;
    float parent_height_p = parent_height + child_height + 10.0f;
    float parent_center_x = parent_width / 2.0f;
    float parent_center_y = parent_height / 2.0f;
    float child_header_height = child_window->getHeaderWidget()->getHeight();
    sf::Vector2f child_header_center = child_window->getHeaderWidget()->getGlobalCenter();
    PRESS_MOUSE_LEFT(child_header_center);
    MOVE_MOUSE(parent_main_widget->toGlobal(sf::Vector2f(parent_width_n, parent_height_n))); // top left
    T_VEC2_APPROX_COMPARE(child_window->getPosition(), sf::Vector2f(margin - child_width, 0.0f));
    MOVE_MOUSE(parent_main_widget->toGlobal(sf::Vector2f(parent_center_x, parent_height_n))); // top
    T_VEC2_APPROX_COMPARE(child_window->getPosition(), sf::Vector2f(parent_center_x - child_width / 2.0f, 0.0f));
    MOVE_MOUSE(parent_main_widget->toGlobal(sf::Vector2f(parent_width_p, parent_height_n))); // top right
    T_VEC2_APPROX_COMPARE(child_window->getPosition(), sf::Vector2f(parent_width - margin, 0.0f));
    MOVE_MOUSE(parent_main_widget->toGlobal(sf::Vector2f(parent_width_n, parent_center_y))); // left
    T_VEC2_APPROX_COMPARE(child_window->getPosition(), sf::Vector2f(margin - child_width, parent_center_y - child_header_height / 2.0f));
    MOVE_MOUSE(parent_main_widget->toGlobal(sf::Vector2f(parent_width_p, parent_center_y))); // right
    T_VEC2_APPROX_COMPARE(child_window->getPosition(), sf::Vector2f(parent_width - margin, parent_center_y - child_header_height / 2.0f));
    MOVE_MOUSE(parent_main_widget->toGlobal(sf::Vector2f(parent_width_n, parent_height_p))); // bottom left
    T_VEC2_APPROX_COMPARE(child_window->getPosition(), sf::Vector2f(margin - child_width, parent_height - margin));
    MOVE_MOUSE(parent_main_widget->toGlobal(sf::Vector2f(parent_center_x, parent_height_p))); // bottom
    T_VEC2_APPROX_COMPARE(child_window->getPosition(), sf::Vector2f(parent_center_x - child_width / 2.0f, parent_height - margin));
    MOVE_MOUSE(parent_main_widget->toGlobal(sf::Vector2f(parent_width_p, parent_height_p))); // bottom right
    T_VEC2_APPROX_COMPARE(child_window->getPosition(), sf::Vector2f(parent_width - margin, parent_height - margin));
}

void WidgetTests::windowWidgetResizeLimitsTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::WindowWidget* parent_window = application.getWidgets().createWindowWidget(300.0f, 250.0f);
    parent_window->setName("parent window");
    parent_window->setPosition(150.0f, 100.0f);
    parent_window->setHeaderFont(textbox_font);
    parent_window->setHeaderText("Parent window");
    parent_window->setHeaderTextCharacterSize(15);
    fw::WindowWidget* child_window = application.getWidgets().createWindowWidget(100.0f, 100.0f);
    child_window->setName("child window");
    child_window->setPosition(20.0f, 20.0f);
    child_window->setHeaderFont(textbox_font);
    child_window->setHeaderText("Child window");
    child_window->setHeaderTextCharacterSize(15);
    child_window->setParent(parent_window);
    application.advance();
    fw::RectangleWidget* parent_main_widget = parent_window->getMainWidget();
    sf::Vector2f child_pos = child_window->getPosition();
    float child_width = child_window->getWidth();
    float child_height = child_window->getHeight();
    sf::Vector2f min_size = fw::WINDOW_MIN_SIZE;
    float parent_width = parent_main_widget->getWidth();
    float parent_height = parent_main_widget->getHeight();
    float parent_width_n = -child_width - 10.0f;
    float parent_width_p = parent_width + child_width + 10.0f;
    float parent_height_n = -child_height - 10.0f;
    float parent_height_p = parent_height + child_height + 10.0f;
    float parent_center_x = parent_width / 2.0f;
    float parent_center_y = parent_height / 2.0f;
    float child_header_height = child_window->getHeaderWidget()->getHeight();
    sf::Vector2f child_header_center = child_window->getHeaderWidget()->getGlobalCenter();
    float cursor_offset = fw::WINDOW_RESIZE_MARGIN / 2.0f;
    auto reset_child_window = [&]() {
        child_window->setPosition(child_pos);
        child_window->setSize(child_width, child_height);
    };

    // making window smaller than minimum size
    {
        // top left
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(min_size.x, min_size.y + child_header_height);
        sf::FloatRect new_bounds = sf::FloatRect(child_window->getBottomRight() + sf::Vector2f(-new_size.x, -new_size.y), new_size);
        resizeWindow(child_window, ResizePoint::TOP_LEFT, sf::Vector2f(child_width, child_height));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // top
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(child_width, min_size.y + child_header_height);
        sf::FloatRect new_bounds = sf::FloatRect(child_window->getBottomLeft() + sf::Vector2f(0.0f, -new_size.y), new_size);
        resizeWindow(child_window, ResizePoint::TOP, sf::Vector2f(0.0f, child_height));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // top right
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(min_size.x, min_size.y + child_header_height);
        sf::FloatRect new_bounds = sf::FloatRect(child_window->getBottomLeft() + sf::Vector2f(0.0f, -new_size.y), new_size);
        resizeWindow(child_window, ResizePoint::TOP_RIGHT, sf::Vector2f(-child_width, child_height));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // left
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(min_size.x, child_height);
        sf::FloatRect new_bounds = sf::FloatRect(child_window->getTopRight() + sf::Vector2f(-new_size.x, 0.0f), new_size);
        resizeWindow(child_window, ResizePoint::LEFT, sf::Vector2f(child_width, 0.0f));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // right
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(min_size.x, child_height);
        sf::FloatRect new_bounds = sf::FloatRect(child_window->getTopLeft(), new_size);
        resizeWindow(child_window, ResizePoint::RIGHT, sf::Vector2f(-child_width, 0.0f));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // bottom left
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(min_size.x, min_size.y + child_header_height);
        sf::FloatRect new_bounds = sf::FloatRect(child_window->getTopRight() + sf::Vector2f(-new_size.x, 0.0f), new_size);
        resizeWindow(child_window, ResizePoint::BOTTOM_LEFT, sf::Vector2f(child_width, -child_height));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // bottom
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(child_width, min_size.y + child_header_height);
        sf::FloatRect new_bounds = sf::FloatRect(child_window->getTopLeft(), new_size);
        resizeWindow(child_window, ResizePoint::BOTTOM, sf::Vector2f(0.0f, -child_height));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // bottom right
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(min_size.x, min_size.y + child_header_height);
        sf::FloatRect new_bounds = sf::FloatRect(child_window->getTopLeft(), new_size);
        resizeWindow(child_window, ResizePoint::BOTTOM_RIGHT, sf::Vector2f(-child_width, -child_height));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }

    // resizing past parent window borders
    {
        // top left
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(child_width + parent_width, child_window->getBottom().y);
        sf::FloatRect new_bounds = sf::FloatRect(sf::Vector2f(child_pos.x - parent_width, 0.0f), new_size);
        resizeWindow(child_window, ResizePoint::TOP_LEFT, sf::Vector2f(-parent_width, -parent_height));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // top right
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(child_width + parent_width, child_window->getBottom().y);
        sf::FloatRect new_bounds = sf::FloatRect(sf::Vector2f(child_pos.x, 0.0f), new_size);
        resizeWindow(child_window, ResizePoint::TOP_RIGHT, sf::Vector2f(parent_width, -parent_height));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // bottom left
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(child_width + parent_width, child_height + parent_height);
        sf::FloatRect new_bounds = sf::FloatRect(sf::Vector2f(child_pos.x - parent_width, child_pos.y), new_size);
        resizeWindow(child_window, ResizePoint::BOTTOM_LEFT, sf::Vector2f(-parent_width, parent_height));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // bottom right
        reset_child_window();
        sf::Vector2f new_size = sf::Vector2f(child_width + parent_width, child_height + parent_height);
        sf::FloatRect new_bounds = sf::FloatRect(sf::Vector2f(child_pos.x, child_pos.y), new_size);
        resizeWindow(child_window, ResizePoint::BOTTOM_RIGHT, sf::Vector2f(parent_width, parent_height));
        T_COMPARE(child_window->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
}

void WidgetTests::windowWidgetMoveToTopDragTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    float window_width = 100.0f;
    sf::Vector2f position1(100.0f, 100.0f);
    sf::Vector2f position2(position1.x + window_width, 100.0f);
    sf::Vector2f size1(window_width, 100.0f);
    sf::Vector2f size2(window_width, 100.0f);
    fw::WindowWidget* window_1_widget = application.getWidgets().createWindowWidget(size1);
    fw::WindowWidget* window_2_widget = application.getWidgets().createWindowWidget(size2);
    window_1_widget->setPosition(position1);
    window_2_widget->setPosition(position2);
    dragWindow(application, window_1_widget, sf::Vector2f(window_width, 0.0f));
    T_VEC2_APPROX_COMPARE(window_1_widget->getPosition(), window_2_widget->getPosition());
    dragWindow(application, window_1_widget, sf::Vector2f(window_width, 0.0f));
    T_VEC2_APPROX_COMPARE(window_1_widget->getPosition(), window_2_widget->getPosition() + sf::Vector2f(window_width, 0.0f));
    dragWindow(application, window_2_widget, sf::Vector2f(window_width, 0.0f));
    T_VEC2_APPROX_COMPARE(window_2_widget->getPosition(), window_1_widget->getPosition());
    dragWindow(application, window_2_widget, sf::Vector2f(window_width, 0.0f));
    T_VEC2_APPROX_COMPARE(window_2_widget->getPosition(), window_1_widget->getPosition() + sf::Vector2f(window_width, 0.0f));
}

void WidgetTests::windowWidgetMoveToTopResizeTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.setDefaultFont(textbox_font);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    float window_width = 100.0f;
    sf::Vector2f position1(100.0f, 100.0f);
    sf::Vector2f pos_offset(window_width, 0.0f);
    sf::Vector2f position2(position1.x + window_width, 100.0f);
    sf::Vector2f square_position = position1;
    sf::Vector2f window_size(window_width, 100.0f);
    sf::Vector2f square_size(window_width, window_size.y / 2.0f);
    sf::Vector2f resize_offset(0.0f, 10.0f);
    fw::WindowWidget* window_widget = application.getWidgets().createWindowWidget(window_size);
    fw::RectangleWidget* square_widget = application.getWidgets().createRectangleWidget(square_size);
    window_widget->setPosition(position1);
    square_widget->setPosition(position1);
    square_widget->setClickThrough(false);

    dragWindow(application, window_widget, sf::Vector2f(window_width, 0.0f));
    T_VEC2_APPROX_COMPARE(window_widget->getPosition(), position1);

    resizeWindow(window_widget, ResizePoint::BOTTOM, resize_offset);
    dragWindow(application, window_widget, sf::Vector2f(window_width, 0.0f));
    T_VEC2_APPROX_COMPARE(window_widget->getPosition(), position2);
}

void WidgetTests::dropdownWidgetBasicTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    fw::DropdownWidget* dropdown_widget = application.getWidgets().createDropdownWidget();
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(dropdown_widget));
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(40.0f, 20.0f);
    dropdown_widget->setPosition(position);
    dropdown_widget->setSize(size);

    GenericWidgetTest gwt(application, test);
    gwt.widget = dropdown_widget;
    gwt.total_widgets = 7;
    gwt.type = fw::Widget::WidgetType::Dropdown;
    gwt.name = "dropdown";
    gwt.fullname = "root|dropdown";
    gwt.is_visual_position_quantized = false;
    fw::WidgetVisibility visibility;
    visibility.addedToRoot = true;
    visibility.allParentsVisible = true;
    visibility.hasUnclippedRegion = true;
    visibility.nonZeroSize = true;
    visibility.onScreen = true;
    visibility.opaque = false;
    visibility.renderableSetting = false;
    visibility.visibleSetting = true;
    gwt.visibility = visibility;
    gwt.is_click_through = false;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::MODAL;
    gwt.is_focused = false;
    gwt.clip_children = false;
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

void WidgetTests::dropdownWidgetOptions1Test(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    fw::DropdownWidget* dropdown_widget = application.getWidgets().createDropdownWidget();
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(40.0f, 20.0f);
    dropdown_widget->setPosition(position);
    dropdown_widget->setSize(size);
    dropdown_widget->setFont(textbox_font);
    dropdown_widget->setCharacterSize(15);
    sf::Vector2f dropdown_center = dropdown_widget->getGlobalCenter();
    fw::RectangleWidget* panel_widget = dropdown_widget->getPanelWidget();
    application.advance();

    dropdown_widget->addOption("option1");
    dropdown_widget->addOption("option2");
    dropdown_widget->addOption("option3");
    T_COMPARE(dropdown_widget->getValue(), -1);
    T_CHECK(!panel_widget->isVisible());

    auto click_option = [&](size_t index, ptrdiff_t prev) {
        T_CHECK(!dropdown_widget->isFocused());
        CLICK_MOUSE(dropdown_center);
        T_CHECK(dropdown_widget->isFocused());
        T_COMPARE(dropdown_widget->getValue(), prev);
        T_CHECK(panel_widget->isVisible());
        CLICK_MOUSE(dropdown_widget->getOptionWidget(index)->getGlobalCenter());
        T_CHECK(!dropdown_widget->isFocused());
        T_COMPARE(dropdown_widget->getValue(), index);
        T_CHECK(!panel_widget->isVisible());
    };
    T_WRAP_CONTAINER(click_option(0, -1));
    T_WRAP_CONTAINER(click_option(1, 0));
    T_WRAP_CONTAINER(click_option(2, 1));

    T_ASSERT_NO_ERRORS();
    CLICK_MOUSE(dropdown_center);
    ptrdiff_t value = dropdown_widget->getValue();
    CLICK_MOUSE(dropdown_center);
    T_CHECK(!dropdown_widget->isFocused());
    T_COMPARE(dropdown_widget->getValue(), value);
    T_CHECK(!panel_widget->isVisible());
}

void WidgetTests::dropdownWidgetOptions2Test(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    fw::DropdownWidget* dropdown_widget = application.getWidgets().createDropdownWidget();
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(40.0f, 20.0f);
    dropdown_widget->setPosition(position);
    dropdown_widget->setSize(size);
    dropdown_widget->setFont(textbox_font);
    dropdown_widget->setCharacterSize(15);
    sf::Vector2f dropdown_center = dropdown_widget->getGlobalCenter();
    fw::RectangleWidget* panel_widget = dropdown_widget->getPanelWidget();
    application.advance();

    auto click_option = [&](size_t index) {
        CLICK_MOUSE(dropdown_center);
        CLICK_MOUSE(dropdown_widget->getOptionWidget(index)->getGlobalCenter());
    };
    dropdown_widget->addOption("option1");
    dropdown_widget->addOption("option2");
    dropdown_widget->addOption("option1.5", 1);
    T_COMPARE(dropdown_widget->getOptionText(0), "option1");
    T_COMPARE(dropdown_widget->getOptionText(1), "option1.5");
    T_COMPARE(dropdown_widget->getOptionText(2), "option2");
    dropdown_widget->removeOption("option1.5");
    T_COMPARE(dropdown_widget->getOptionText(0), "option1");
    T_COMPARE(dropdown_widget->getOptionText(1), "option2");
    dropdown_widget->removeOption("option1");
    T_COMPARE(dropdown_widget->getOptionText(0), "option2");
    dropdown_widget->addOption("option1", 0);
    T_COMPARE(dropdown_widget->getOptionText(0), "option1");
    T_COMPARE(dropdown_widget->getOptionText(1), "option2");
    dropdown_widget->addOption("option1.5", 1);
    T_COMPARE(dropdown_widget->getOptionText(0), "option1");
    T_COMPARE(dropdown_widget->getOptionText(1), "option1.5");
    T_COMPARE(dropdown_widget->getOptionText(2), "option2");
    dropdown_widget->setOptionText(1, "option3");
    T_COMPARE(dropdown_widget->getOptionText(0), "option1");
    T_COMPARE(dropdown_widget->getOptionText(1), "option3");
    T_COMPARE(dropdown_widget->getOptionText(2), "option2");
}

void WidgetTests::scrollAreaWidgetBasicTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.advance();
    sf::Vector2f size(300.0f, 200.0f);
    fw::ScrollAreaWidget* scroll_area_widget = application.getWidgets().createScrollAreaWidget(size);
    fw::Widget* root_widget = application.getWidgets().getRootWidget();
    T_ASSERT(T_CHECK(scroll_area_widget));
    sf::Vector2f position(100.0f, 100.0f);
    scroll_area_widget->setPosition(position);

    GenericWidgetTest gwt(application, test);
    gwt.widget = scroll_area_widget;
    gwt.total_widgets = 8;
    gwt.type = fw::Widget::WidgetType::ScrollArea;
    gwt.name = "scroll area";
    gwt.fullname = "root|scroll area";
    gwt.is_visual_position_quantized = false;
    fw::WidgetVisibility visibility;
    visibility.addedToRoot = true;
    visibility.allParentsVisible = true;
    visibility.hasUnclippedRegion = true;
    visibility.nonZeroSize = true;
    visibility.onScreen = true;
    visibility.opaque = false;
    visibility.renderableSetting = false;
    visibility.visibleSetting = true;
    gwt.visibility = visibility;
    gwt.is_click_through = true;
    gwt.is_mouse_over = false;
    gwt.focusable_type = fw::Widget::FocusableType::NONE;
    gwt.is_focused = false;
    gwt.clip_children = false;
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

void WidgetTests::scrollAreaWidgetScrollTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.setDefaultFont(textbox_font);
    sf::Vector2f scroll_area_size(300.0f, 200.0f);
    sf::Vector2f container_size(100.0f, 100.0f);
    sf::Vector2f child_size(350.0f, 100.0f);
    fw::ScrollAreaWidget* scroll_area_widget = application.getWidgets().createScrollAreaWidget(scroll_area_size);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::RectangleWidget* red_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* green_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* blue_widget = application.getWidgets().createRectangleWidget(child_size);
    scroll_area_widget->setScrolledWidget(container_widget);
    container_widget->setFillColor(sf::Color(100, 100, 100));
    container_widget->setHorizontal(false);
    red_widget->setFillColor(sf::Color::Red);
    red_widget->setParent(container_widget);
    green_widget->setFillColor(sf::Color::Green);
    green_widget->setParent(container_widget);
    blue_widget->setFillColor(sf::Color::Blue);
    blue_widget->setParent(container_widget);

    auto check_slider_x = [&](float x, bool scrollbar_y_visible) {
        T_APPROX_COMPARE(scroll_area_widget->getSliderXWidget()->getPosition().x, x);
        float scrollbar_x_width = scroll_area_widget->getScrollbarXWidget()->getWidth();
        float scroll_area_width = scroll_area_widget->getWidth();
        if (scrollbar_y_visible) {
            float scrollbar_y_width = scroll_area_widget->getScrollbarYWidget()->getWidth();
            scroll_area_width -= scrollbar_y_width;
        }
        float x_factor = scroll_area_width / container_widget->getWidth();
        float slider_x_width = scrollbar_x_width * x_factor;
        T_APPROX_COMPARE(scroll_area_widget->getSliderXWidget()->getWidth(), slider_x_width);
    };
    auto check_slider_y = [&](float y, bool scrollbar_x_visible) {
        T_APPROX_COMPARE(scroll_area_widget->getSliderYWidget()->getPosition().y, y);
        float scrollbar_y_height = scroll_area_widget->getScrollbarYWidget()->getHeight();
        float scroll_area_height = scroll_area_widget->getHeight();
        if (scrollbar_x_visible) {
            float scrollbar_x_height = scroll_area_widget->getScrollbarXWidget()->getHeight();
            scroll_area_height -= scrollbar_x_height;
        }
        float y_factor = scroll_area_height / container_widget->getHeight();
        float slider_y_height = scrollbar_y_height * y_factor;
        T_APPROX_COMPARE(scroll_area_widget->getSliderYWidget()->getHeight(), slider_y_height);
    };

    application.advance();
    T_CHECK(scroll_area_widget->getScrollbarXWidget()->isVisible());
    T_CHECK(scroll_area_widget->getScrollbarYWidget()->isVisible());
    T_WRAP_CONTAINER(check_slider_x(0.0f, true));
    T_WRAP_CONTAINER(check_slider_y(0.0f, true));

    T_ASSERT_NO_ERRORS();
    float slider_x_offset = 10.0f;
    float slider_y_offset = 10.0f;
    float slider_x_big_offset = 100.0f;
    float slider_y_big_offset = 100.0f;
    {
        // scrolling x slider
        mouseDragGesture(application, scroll_area_widget->getSliderXWidget()->getGlobalCenter(), sf::Vector2f(slider_x_offset, 0.0f));
        T_WRAP_CONTAINER(check_slider_x(slider_x_offset, true));
        T_WRAP_CONTAINER(check_slider_y(0.0f, true));
        float slider_x_factor = container_widget->getWidth() / scroll_area_widget->getScrollbarXWidget()->getWidth();
        float container_pos_x = -slider_x_offset * slider_x_factor;
        T_APPROX_COMPARE(container_widget->getPosition().x, container_pos_x);
    }
    {
        // scrolling y slider
        mouseDragGesture(application, scroll_area_widget->getSliderYWidget()->getGlobalCenter(), sf::Vector2f(0.0f, slider_y_offset));
        T_WRAP_CONTAINER(check_slider_y(slider_x_offset, true));
        T_WRAP_CONTAINER(check_slider_y(slider_y_offset, true));
        float slider_y_factor = container_widget->getHeight() / scroll_area_widget->getScrollbarYWidget()->getHeight();
        float container_pos_y = -slider_y_offset * slider_y_factor;
        T_APPROX_COMPARE(container_widget->getPosition().y, container_pos_y);
    }
    {
        // scrolling x slider to the beginning
        mouseDragGesture(application, scroll_area_widget->getSliderXWidget()->getGlobalCenter(), sf::Vector2f(-slider_x_big_offset, 0.0f));
        T_WRAP_CONTAINER(check_slider_x(0.0f, true));
        T_WRAP_CONTAINER(check_slider_y(slider_y_offset, true));
        T_APPROX_COMPARE(container_widget->getPosition().x, 0.0f);
    }
    {
        // scrolling y slider to the beginning
        mouseDragGesture(application, scroll_area_widget->getSliderYWidget()->getGlobalCenter(), sf::Vector2f(0.0f, -slider_y_big_offset));
        T_WRAP_CONTAINER(check_slider_x(0.0f, true));
        T_WRAP_CONTAINER(check_slider_y(0.0f, true));
        T_APPROX_COMPARE(container_widget->getPosition().y, 0.0f);
    }
    {
        // scrolling x slider to the end
        mouseDragGesture(application, scroll_area_widget->getSliderXWidget()->getGlobalCenter(), sf::Vector2f(slider_x_big_offset, 0.0f));
        float x_range = scroll_area_widget->getScrollbarXWidget()->getWidth() - scroll_area_widget->getSliderXWidget()->getWidth();
        T_WRAP_CONTAINER(check_slider_x(x_range, true));
        T_WRAP_CONTAINER(check_slider_y(0.0f, true));
        float slider_x_factor = container_widget->getWidth() / scroll_area_widget->getScrollbarXWidget()->getWidth();
        float container_pos_x = -x_range * slider_x_factor;
        T_APPROX_COMPARE(container_widget->getPosition().x, container_pos_x);
    }
    {
        // scrolling y slider to the end
        mouseDragGesture(application, scroll_area_widget->getSliderYWidget()->getGlobalCenter(), sf::Vector2f(0.0f, slider_y_big_offset));
        float x_range = scroll_area_widget->getScrollbarXWidget()->getWidth() - scroll_area_widget->getSliderXWidget()->getWidth();
        float y_range = scroll_area_widget->getScrollbarYWidget()->getHeight() - scroll_area_widget->getSliderYWidget()->getHeight();
        T_WRAP_CONTAINER(check_slider_x(x_range, true));
        T_WRAP_CONTAINER(check_slider_y(y_range, true));
        float slider_y_factor = container_widget->getHeight() / scroll_area_widget->getScrollbarYWidget()->getHeight();
        float container_pos_y = -y_range * slider_y_factor;
        T_APPROX_COMPARE(container_widget->getPosition().y, container_pos_y);
    }
}

void WidgetTests::scrollAreaWidgetScrollbarVisibilityTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.setDefaultFont(textbox_font);
    sf::Vector2f scroll_area_size(300.0f, 200.0f);
    sf::Vector2f container_size(100.0f, 100.0f);
    sf::Vector2f child_size(350.0f, 100.0f);
    fw::WindowWidget* window_widget = application.getWidgets().createWindowWidget(scroll_area_size);
    fw::ScrollAreaWidget* scroll_area_widget = application.getWidgets().createScrollAreaWidget(scroll_area_size);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::RectangleWidget* red_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* green_widget = application.getWidgets().createRectangleWidget(child_size);
    fw::RectangleWidget* blue_widget = application.getWidgets().createRectangleWidget(child_size);
    scroll_area_widget->setScrolledWidget(container_widget);
    scroll_area_widget->setSizePolicy(fw::Widget::SizePolicy::PARENT);
    scroll_area_widget->setParent(window_widget);
    container_widget->setFillColor(sf::Color(100, 100, 100));
    container_widget->setHorizontal(false);
    red_widget->setFillColor(sf::Color::Red);
    red_widget->setParent(container_widget);
    green_widget->setFillColor(sf::Color::Green);
    green_widget->setParent(container_widget);
    blue_widget->setFillColor(sf::Color::Blue);
    blue_widget->setParent(container_widget);

    application.advance();
    T_CHECK(scroll_area_widget->getScrollbarXWidget()->isVisible());
    T_CHECK(scroll_area_widget->getScrollbarYWidget()->isVisible());

    {
        float margin = 100.0f;
        float offset = container_widget->getWidth() - scroll_area_widget->getWidth() + margin;
        resizeWindow(window_widget, ResizePoint::RIGHT, sf::Vector2f(offset, 0.0f));
        T_CHECK(!scroll_area_widget->getScrollbarXWidget()->isVisible());
        T_CHECK(scroll_area_widget->getScrollbarYWidget()->isVisible());
    }
    {
        float margin = 100.0f;
        float offset = container_widget->getHeight() - scroll_area_widget->getHeight() + margin;
        resizeWindow(window_widget, ResizePoint::BOTTOM, sf::Vector2f(0.0f, offset));
        T_CHECK(!scroll_area_widget->getScrollbarXWidget()->isVisible());
        T_CHECK(!scroll_area_widget->getScrollbarYWidget()->isVisible());
    }
    {
        // testing behaviour near the corner
        float margin = fw::SCROLL_AREA_SCROLLBAR_DEFAULT_WIDTH / 2.0f;
        float offset_x = container_widget->getWidth() - scroll_area_widget->getWidth() + margin;
        float offset_y = container_widget->getHeight() - scroll_area_widget->getHeight() + margin;
        resizeWindow(window_widget, ResizePoint::BOTTOM_RIGHT, sf::Vector2f(offset_x, offset_y));
        T_CHECK(!scroll_area_widget->getScrollbarXWidget()->isVisible());
        T_CHECK(!scroll_area_widget->getScrollbarYWidget()->isVisible());
        resizeWindow(window_widget, ResizePoint::RIGHT, sf::Vector2f(-margin * 2.0f, 0.0f));
        T_CHECK(scroll_area_widget->getScrollbarXWidget()->isVisible());
        T_CHECK(scroll_area_widget->getScrollbarYWidget()->isVisible());
    }
    {
        // testing a case where scrollbar becomes invisible in one position,
        // and becomes visible again in another position
        float margin_1 = -50.0f;
        float margin_2 = 100.0f;
        float offset_x = container_widget->getWidth() - scroll_area_widget->getWidth() + margin_1;
        float offset_y = container_widget->getHeight() - scroll_area_widget->getHeight() + margin_1;
        resizeWindow(window_widget, ResizePoint::BOTTOM_RIGHT, sf::Vector2f(offset_x, offset_y));
        T_CHECK(scroll_area_widget->getScrollbarXWidget()->isVisible());
        T_CHECK(scroll_area_widget->getScrollbarYWidget()->isVisible());
        resizeWindow(window_widget, ResizePoint::BOTTOM, sf::Vector2f(0.0f, margin_2));
        resizeWindow(window_widget, ResizePoint::RIGHT, sf::Vector2f(margin_2, 0.0f));
        // not using resizeWindow because it calls advance one more time
        sf::Vector2f grab_pos = getGrabPos(window_widget, ResizePoint::BOTTOM);
        application.mouseMove(grab_pos);
        application.advance();
        application.mouseLeftPress();
        application.advance();
        application.mouseMove(sf::Vector2f(grab_pos.x, grab_pos.y - margin_2));
        application.advance();
        float scrollbar_pos = scroll_area_widget->getWidth();
        if (T_CHECK(scroll_area_widget->getScrollbarYWidget()->isVisible())) {
            T_COMPARE(scroll_area_widget->getScrollbarYWidget()->getPosition().x, scrollbar_pos);
        }
        application.mouseLeftRelease();
        application.advance();
    }
}

void WidgetTests::scrollAreaWidgetScrollbarContainerTest(test::Test& test) {
    fw::Application application(window);
    application.init(test.name, 800, 600, 0, false);
    application.start(true);
    application.setDefaultFont(textbox_font);
    sf::Vector2f scroll_area_size(300.0f, 200.0f);
    sf::Vector2f container_size(100.0f, 100.0f);
    sf::Vector2f child_size(100.0f, 200.0f);
    fw::ScrollAreaWidget* scroll_area_widget = application.getWidgets().createScrollAreaWidget(scroll_area_size);
    fw::ContainerWidget* container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::ContainerWidget* inner_container_widget = application.getWidgets().createContainerWidget(container_size);
    fw::RectangleWidget* red_widget = application.getWidgets().createRectangleWidget(child_size);
    scroll_area_widget->setScrolledWidget(container_widget);
    container_widget->setFillColor(sf::Color(100, 100, 100));
    container_widget->setHorizontal(false);
    container_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    inner_container_widget->setName("inner container");
    inner_container_widget->setFillColor(sf::Color(70, 70, 70));
    inner_container_widget->setHorizontal(false);
    inner_container_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    inner_container_widget->setParent(container_widget);
    red_widget->setName("red");
    red_widget->setFillColor(sf::Color::Red);
    red_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    red_widget->setParent(inner_container_widget);
    application.advance();

    red_widget->setHeight(red_widget->getHeight() + 50.0f);
    application.advance();
    T_CHECK(!scroll_area_widget->getScrollbarXWidget()->isVisible());
    T_CHECK(scroll_area_widget->getScrollbarYWidget()->isVisible());
    application.advance();
    T_CHECK(!scroll_area_widget->getScrollbarXWidget()->isVisible());
    T_CHECK(scroll_area_widget->getScrollbarYWidget()->isVisible());
    red_widget->setHeight(child_size.y);
    application.advance();
    T_CHECK(!scroll_area_widget->getScrollbarXWidget()->isVisible());
    T_CHECK(!scroll_area_widget->getScrollbarYWidget()->isVisible());
    T_COMPARE(container_widget->getWidth(), scroll_area_size.x);
    application.advance();
    T_CHECK(!scroll_area_widget->getScrollbarXWidget()->isVisible());
    T_CHECK(!scroll_area_widget->getScrollbarYWidget()->isVisible());
    T_COMPARE(container_widget->getWidth(), scroll_area_size.x);
}

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
    gwt.total_widgets = 2;
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

    fw::TreeViewWidget::Entry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewWidget::Entry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewWidget::Entry* entry_2_child_1 = tree_view_widget->addEntry("Entry 2 Child 1");
    fw::TreeViewWidget::Entry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewWidget::Entry* entry_3_child_1 = tree_view_widget->addEntry("Entry 3 Child 1");
    fw::TreeViewWidget::Entry* entry_3_child_2 = tree_view_widget->addEntry("Entry 3 Child 2");
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

    fw::TreeViewWidget::Entry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewWidget::Entry* entry_1_child_1 = tree_view_widget->addEntry("Entry 1 Child 1");
    fw::TreeViewWidget::Entry* entry_1_child_1_child_1 = tree_view_widget->addEntry("Entry 1 Child 1 Child 1");
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

    auto click_entry = [&](fw::TreeViewWidget::Entry* entry, bool shift = false, bool ctrl = false) {
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
    auto click_arrow = [&](fw::TreeViewWidget::Entry* entry) {
        sf::Vector2f arrow_pos = entry->getArrowAreaWidget()->getGlobalCenter();
        application.mouseMove(arrow_pos);
        application.mouseLeftPress();
        application.advance();
        application.mouseLeftRelease();
        application.advance();
    };
    CompVector<fw::TreeViewWidget::Entry*> clicked_entries;
    CompVector<fw::TreeViewWidget::Entry*> selected_entries;
    CompVector<fw::TreeViewWidget::Entry*> deselected_entries;
    tree_view_widget->OnEntryClicked += [&](fw::TreeViewWidget::Entry* entry) {
        clicked_entries.add(entry);
    };
    tree_view_widget->OnEntrySelected += [&](fw::TreeViewWidget::Entry* entry) {
        selected_entries.add(entry);
    };
    tree_view_widget->OnEntryDeselected += [&](fw::TreeViewWidget::Entry* entry) {
        deselected_entries.add(entry);
    };
    fw::TreeViewWidget::Entry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewWidget::Entry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewWidget::Entry* entry_2_child_1 = tree_view_widget->addEntry("Entry 2 Child 1");
    fw::TreeViewWidget::Entry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewWidget::Entry* entry_3_child_1 = tree_view_widget->addEntry("Entry 3 Child 1");
    fw::TreeViewWidget::Entry* entry_3_child_2 = tree_view_widget->addEntry("Entry 3 Child 2");
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

    fw::TreeViewWidget::Entry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewWidget::Entry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewWidget::Entry* entry_2_child_1 = tree_view_widget->addEntry("Entry 2 Child 1");
    fw::TreeViewWidget::Entry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewWidget::Entry* entry_3_child_1 = tree_view_widget->addEntry("Entry 3 Child 1");
    fw::TreeViewWidget::Entry* entry_3_child_2 = tree_view_widget->addEntry("Entry 3 Child 2");
    fw::TreeViewWidget::Entry* entry_4 = tree_view_widget->addEntry("Entry 4");
    fw::TreeViewWidget::Entry* entry_4_child_1 = tree_view_widget->addEntry("Entry 4 Child 1");
    fw::TreeViewWidget::Entry* entry_4_child_1_child_1 = tree_view_widget->addEntry("Entry 4 Child 1 Child 1");
    fw::TreeViewWidget::Entry* entry_4_child_1_child_2 = tree_view_widget->addEntry("Entry 4 Child 1 Child 2");
    entry_2_child_1->setParent(entry_2);
    entry_3_child_1->setParent(entry_3);
    entry_3_child_2->setParent(entry_3);
    entry_4_child_1->setParent(entry_4);
    entry_4_child_1_child_1->setParent(entry_4_child_1);
    entry_4_child_1_child_2->setParent(entry_4_child_1);
    tree_view_widget->expandAll();
    application.advance();

    const CompVector<fw::TreeViewWidget::Entry*>& top_entries = tree_view_widget->getTopEntries();
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
        const CompVector<fw::TreeViewWidget::Entry*>& children = entry_3->getChildren();
        T_ASSERT(T_COMPARE(children.size(), 2));
        T_CHECK(children[0] == entry_3_child_2);
        T_CHECK(children[1] == entry_3_child_1);
        T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));
    }
    {
        entry_4_child_1_child_1->moveToIndex(2);
        application.advance();
        const CompVector<fw::TreeViewWidget::Entry*>& children = entry_4_child_1->getChildren();
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

    fw::TreeViewWidget::Entry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewWidget::Entry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewWidget::Entry* entry_2_child_1 = tree_view_widget->addEntry("Entry 2 Child 1");
    fw::TreeViewWidget::Entry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewWidget::Entry* entry_3_child_1 = tree_view_widget->addEntry("Entry 3 Child 1");
    fw::TreeViewWidget::Entry* entry_3_child_2 = tree_view_widget->addEntry("Entry 3 Child 2");
    fw::TreeViewWidget::Entry* entry_4 = tree_view_widget->addEntry("Entry 4");
    fw::TreeViewWidget::Entry* entry_4_child_1 = tree_view_widget->addEntry("Entry 4 Child 1");
    fw::TreeViewWidget::Entry* entry_4_child_2 = tree_view_widget->addEntry("Entry 4 Child 2");
    fw::TreeViewWidget::Entry* entry_4_child_3 = tree_view_widget->addEntry("Entry 4 Child 3");
    fw::TreeViewWidget::Entry* entry_5 = tree_view_widget->addEntry("Entry 5");
    fw::TreeViewWidget::Entry* entry_5_child_1 = tree_view_widget->addEntry("Entry 5 Child 1");
    fw::TreeViewWidget::Entry* entry_5_child_1_child_1 = tree_view_widget->addEntry("Entry 5 Child 1 Child 1");
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

    const CompVector<fw::TreeViewWidget::Entry*>& top_entries = tree_view_widget->getTopEntries();
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
    fw::TreeViewWidget::Entry* entry_5_child_2 = tree_view_widget->addEntry("Entry 5 Child 2");
    fw::TreeViewWidget::Entry* entry_5_child_3 = tree_view_widget->addEntry("Entry 5 Child 3");
    fw::TreeViewWidget::Entry* entry_5_child_3_child_1 = tree_view_widget->addEntry("Entry 5 Child 3 Child 1");
    fw::TreeViewWidget::Entry* entry_5_child_3_child_2 = tree_view_widget->addEntry("Entry 5 Child 3 Child 2");
    fw::TreeViewWidget::Entry* entry_5_child_3_child_3 = tree_view_widget->addEntry("Entry 5 Child 3 Child 3");
    fw::TreeViewWidget::Entry* entry_5_child_4 = tree_view_widget->addEntry("Entry 5 Child 4");
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

    fw::TreeViewWidget::Entry* entry_1 = tree_view_widget->addEntry("Entry 1");
    fw::TreeViewWidget::Entry* entry_2 = tree_view_widget->addEntry("Entry 2");
    fw::TreeViewWidget::Entry* entry_2_child_1 = tree_view_widget->addEntry("Entry 2 Child 1");
    fw::TreeViewWidget::Entry* entry_3 = tree_view_widget->addEntry("Entry 3");
    fw::TreeViewWidget::Entry* entry_3_child_1 = tree_view_widget->addEntry("Entry 3 Child 1");
    fw::TreeViewWidget::Entry* entry_3_child_2 = tree_view_widget->addEntry("Entry 3 Child 2");
    fw::TreeViewWidget::Entry* entry_4 = tree_view_widget->addEntry("Entry 4");
    fw::TreeViewWidget::Entry* entry_4_child_1 = tree_view_widget->addEntry("Entry 4 Child 1");
    fw::TreeViewWidget::Entry* entry_4_child_2 = tree_view_widget->addEntry("Entry 4 Child 2");
    fw::TreeViewWidget::Entry* entry_4_child_3 = tree_view_widget->addEntry("Entry 4 Child 3");
    fw::TreeViewWidget::Entry* entry_5 = tree_view_widget->addEntry("Entry 5");
    fw::TreeViewWidget::Entry* entry_5_child_1 = tree_view_widget->addEntry("Entry 5 Child 1");
    fw::TreeViewWidget::Entry* entry_5_child_1_child_1 = tree_view_widget->addEntry("Entry 5 Child 1 Child 1");
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

    fw::TreeViewWidget::Entry* entry_6 = tree_view_widget->addEntry("Entry 6");
    application.advance();
    T_COMPARE(tree_view_widget->getTopEntryCount(), 1);
    T_COMPARE(tree_view_widget->getAllEntryCount(), 1);
    T_COMPARE(tree_view_widget->getHeight(), calcTreeViewHeight(tree_view_widget));
}

std::string WidgetTests::sfVec2fToStr(const sf::Vector2f& vec) {
    return "(" + utils::vec_to_str(vec) + ")";
}

std::string WidgetTests::sfVec2iToStr(const sf::Vector2i& vec) {
    return "(" + utils::vec_to_str(vec) + ")";
}

std::string WidgetTests::sfVec2uToStr(const sf::Vector2u& vec) {
    return "(" + utils::vec_to_str(vec) + ")";
}

std::string WidgetTests::cursorTypeToStr(sf::Cursor::Type type) {
    switch (type) {
        case sf::Cursor::Arrow:                  return "Arrow";
        case sf::Cursor::ArrowWait:              return "ArrowWait";
        case sf::Cursor::Wait:                   return "Wait";
        case sf::Cursor::Text:                   return "Text";
        case sf::Cursor::Hand:                   return "Hand";
        case sf::Cursor::SizeHorizontal:         return "SizeHorizontal";
        case sf::Cursor::SizeVertical:           return "SizeVertical";
        case sf::Cursor::SizeTopLeftBottomRight: return "SizeTopLeftBottomRight";
        case sf::Cursor::SizeBottomLeftTopRight: return "SizeBottomLeftTopRight";
        case sf::Cursor::SizeLeft:               return "SizeLeft";
        case sf::Cursor::SizeRight:              return "SizeRight";
        case sf::Cursor::SizeTop:                return "SizeTop";
        case sf::Cursor::SizeBottom:             return "SizeBottom";
        case sf::Cursor::SizeTopLeft:            return "SizeTopLeft";
        case sf::Cursor::SizeBottomRight:        return "SizeBottomRight";
        case sf::Cursor::SizeBottomLeft:         return "SizeBottomLeft";
        case sf::Cursor::SizeTopRight:           return "SizeTopRight";
        case sf::Cursor::SizeAll:                return "SizeAll";
        case sf::Cursor::Cross:                  return "Cross";
        case sf::Cursor::Help:                   return "Help";
        case sf::Cursor::NotAllowed:             return "NotAllowed";
        default:                                 mAssert(false, "Unknown cursor type"); return "Unknown";
    }
}

std::string WidgetTests::floatRectToStr(const sf::FloatRect& rect) {
    return "pos: " + sfVec2fToStr(rect.getPosition()) + " size: " + sfVec2fToStr(rect.getSize());
}

std::string WidgetTests::colorToStr(const sf::Color& color) {
    return "(" + utils::color_to_str(color) + ")";
}

std::string WidgetTests::anchorToStr(fw::Widget::Anchor anchor) {
    switch (anchor) {
        case fw::Widget::Anchor::CUSTOM:        return "CUSTOM";
        case fw::Widget::Anchor::TOP_LEFT:      return "TOP_LEFT";
        case fw::Widget::Anchor::TOP_CENTER:    return "TOP_CENTER";
        case fw::Widget::Anchor::TOP_RIGHT:     return "TOP_RIGHT";
        case fw::Widget::Anchor::CENTER_LEFT:   return "CENTER_LEFT";
        case fw::Widget::Anchor::CENTER:        return "CENTER";
        case fw::Widget::Anchor::CENTER_RIGHT:  return "CENTER_RIGHT";
        case fw::Widget::Anchor::BOTTOM_LEFT:   return "BOTTOM_LEFT";
        case fw::Widget::Anchor::BOTTOM_CENTER: return "BOTTOM_CENTER";
        case fw::Widget::Anchor::BOTTOM_RIGHT:  return "BOTTOM_RIGHT";
        default:                                mAssert("Unknown anchor type"); return "Unknown";
    }
}

bool WidgetTests::rectApproxCmp(const sf::FloatRect& left, const sf::FloatRect& right) {
    float epsilon = 0.0001f;
    if (abs(left.left - right.left) >= epsilon) {
        return false;
    }
    if (abs(left.top - right.top) >= epsilon) {
        return false;
    }
    if (abs(left.width - right.width) >= epsilon) {
        return false;
    }
    if (abs(left.height - right.height) >= epsilon) {
        return false;
    }
    return true;
}

fw::TextBoxWidget* WidgetTests::initTextBox(fw::Application& application, const std::string& test_name, float width, float height) const {
    application.init(test_name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::TextBoxWidget* textbox_widget = application.getWidgets().createTextBoxWidget();
    textbox_widget->setCharacterSize(20);
    textbox_widget->setFont(textbox_font);
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(width, height);
    std::string value = "Text";
    textbox_widget->setPosition(position);
    textbox_widget->setSize(size);
    textbox_widget->setValue(value);
    application.advance();
    return textbox_widget;
}

void WidgetTests::mouseDragGesture(
    fw::Application& application,
    const sf::Vector2f& begin_pos,
    const sf::Vector2f& offset
) {
    application.mouseMove(begin_pos);
    application.advance();
    application.mouseLeftPress();
    application.advance();
    application.mouseMove(begin_pos + offset);
    application.advance();
    application.mouseLeftRelease();
    application.advance();
}

sf::Vector2f WidgetTests::getGrabPos(fw::WindowWidget* window, ResizePoint resize_point) {
    float cursor_offset = fw::WINDOW_RESIZE_MARGIN / 2.0f;
    sf::Vector2f grab_pos;
    if (resize_point == ResizePoint::TOP_LEFT) {
        grab_pos = window->getGlobalTopLeft() + sf::Vector2f(-cursor_offset, -cursor_offset);
    } else if (resize_point == ResizePoint::TOP) {
        grab_pos = window->getGlobalTop() + sf::Vector2f(0.0f, -cursor_offset);
    } else if (resize_point == ResizePoint::TOP_RIGHT) {
        grab_pos = window->getGlobalTopRight() + sf::Vector2f(cursor_offset, -cursor_offset);
    } else if (resize_point == ResizePoint::LEFT) {
        grab_pos = window->getGlobalLeft() + sf::Vector2f(-cursor_offset, 0.0f);
    } else if (resize_point == ResizePoint::RIGHT) {
        grab_pos = window->getGlobalRight() + sf::Vector2f(cursor_offset, 0.0f);
    } else if (resize_point == ResizePoint::BOTTOM_LEFT) {
        grab_pos = window->getGlobalBottomLeft() + sf::Vector2f(-cursor_offset, cursor_offset);
    } else if (resize_point == ResizePoint::BOTTOM) {
        grab_pos = window->getGlobalBottom() + sf::Vector2f(0.0f, cursor_offset);
    } else if (resize_point == ResizePoint::BOTTOM_RIGHT) {
        grab_pos = window->getGlobalBottomRight() + sf::Vector2f(cursor_offset, cursor_offset);
    } else {
        mAssert(false, "Unknown resize point");
    }
    return grab_pos;
}

void WidgetTests::resizeWindow(fw::WindowWidget* window, ResizePoint resize_point, const sf::Vector2f offset) {
    sf::Vector2f grab_pos = getGrabPos(window, resize_point);
    mouseDragGesture(window->getWidgetList().getApplication(), grab_pos, offset);
}

void WidgetTests::resizeWindowTest(fw::Application& application, test::Test& test, fw::WindowWidget* widget) {
    sf::Vector2f resize_offset(15.0f, 10.0f);

    {
        // top left
        sf::FloatRect new_bounds = sf::FloatRect(
            widget->getTopLeft() + resize_offset,
            widget->getSize() - resize_offset
        );
        resizeWindow(widget, ResizePoint::TOP_LEFT, resize_offset);
        T_COMPARE(widget->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // top
        sf::FloatRect new_bounds = sf::FloatRect(
            widget->getTopLeft() + sf::Vector2f(0.0f, resize_offset.y),
            widget->getSize() + sf::Vector2f(0.0f, -resize_offset.y)
        );
        resizeWindow(widget, ResizePoint::TOP, resize_offset);
        T_COMPARE(widget->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // top right
        sf::FloatRect new_bounds = sf::FloatRect(
            widget->getTopLeft() + sf::Vector2f(0.0f, resize_offset.y),
            widget->getSize() + sf::Vector2f(resize_offset.x, -resize_offset.y)
        );
        resizeWindow(widget, ResizePoint::TOP_RIGHT, resize_offset);
        T_COMPARE(widget->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // left
        sf::FloatRect new_bounds = sf::FloatRect(
            widget->getTopLeft() + sf::Vector2f(resize_offset.x, 0.0f),
            widget->getSize() + sf::Vector2f(-resize_offset.x, 0.0f)
        );
        resizeWindow(widget, ResizePoint::LEFT, resize_offset);
        T_COMPARE(widget->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // right
        sf::FloatRect new_bounds = sf::FloatRect(
            widget->getTopLeft(),
            widget->getSize() + sf::Vector2f(resize_offset.x, 0.0f)
        );
        resizeWindow(widget, ResizePoint::RIGHT, resize_offset);
        T_COMPARE(widget->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // bottom left
        sf::FloatRect new_bounds = sf::FloatRect(
            widget->getTopLeft() + sf::Vector2f(resize_offset.x, 0.0f),
            widget->getSize() + sf::Vector2f(-resize_offset.x, resize_offset.y)
        );
        resizeWindow(widget, ResizePoint::BOTTOM_LEFT, resize_offset);
        T_COMPARE(widget->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // bottom
        sf::FloatRect new_bounds = sf::FloatRect(
            widget->getTopLeft(),
            widget->getSize() + sf::Vector2f(0.0f, resize_offset.y)
        );
        resizeWindow(widget, ResizePoint::BOTTOM, resize_offset);
        T_COMPARE(widget->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
    {
        // bottom right
        sf::FloatRect new_bounds = sf::FloatRect(
            widget->getTopLeft(),
            widget->getSize() + resize_offset
        );
        resizeWindow(widget, ResizePoint::BOTTOM_RIGHT, resize_offset);
        T_COMPARE(widget->getParentLocalBounds(), new_bounds, &WidgetTests::floatRectToStr);
    }
}

sf::Vector2f WidgetTests::getHeaderCenter(fw::WindowWidget* window) {
    return window->getHeaderWidget()->getGlobalCenter();
}

void WidgetTests::dragWindow(fw::Application& application, fw::WindowWidget* window, const sf::Vector2f& offset) {
    sf::Vector2f header_center = getHeaderCenter(window);
    application.mouseMove(header_center);
    application.advance();
    application.mouseLeftPress();
    application.advance();
    application.mouseMove(header_center + offset);
    application.advance();
    application.mouseLeftRelease();
    application.advance();
}

float WidgetTests::calcTreeViewEntryHeight(fw::TreeViewWidget::Entry* entry) {
    float result = 0.0f;
    result += fw::TREEVIEW_ENTRY_HEIGHT;
    if (entry->isExpanded()) {
        for (size_t i = 0; i < entry->getChildrenCount(); i++) {
            result += fw::TREEVIEW_CONTAINER_PADDING;
            fw::TreeViewWidget::Entry* child = entry->getChild(i);
            float child_height = calcTreeViewEntryHeight(child);
            result += child_height;
        }
    }
    return result;
}

float WidgetTests::calcTreeViewHeight(fw::TreeViewWidget* treeview) {
    float result = 0.0f;
    if (treeview->getChildrenCount() == 0) {
        return fw::TREEVIEW_CONTAINER_PADDING;
    }
    result += fw::TREEVIEW_CONTAINER_PADDING;
    for (size_t i = 0; i < treeview->getTopEntryCount(); i++) {
        fw::TreeViewWidget::Entry* entry = treeview->getFromTop(i);
        float entry_height = calcTreeViewEntryHeight(entry);
        result += entry_height;
        if (i < treeview->getTopEntryCount() - 1) {
            result += fw::TREEVIEW_CONTAINER_PADDING;
        }
    }
    result += fw::TREEVIEW_CONTAINER_PADDING;
    return result;
}

void WidgetTests::genericWidgetTest(const GenericWidgetTest& gwt) {
    fw::Application& application = gwt.application;
    test::Test& test = gwt.test;
    fw::Widget* widget = gwt.widget;
    T_COMPARE(application.getWidgets().getSize(), gwt.total_widgets);
    T_CHECK(widget->getType() == gwt.type);
    T_COMPARE(widget->getName(), gwt.name);
    T_COMPARE(widget->getFullName(), gwt.fullname);
    T_COMPARE(widget->isVisualPositionQuantized(), gwt.is_visual_position_quantized);
    fw::WidgetVisibility wv = widget->checkVisibility();
    T_COMPARE(wv.addedToRoot, gwt.visibility.addedToRoot);
    T_COMPARE(wv.allParentsVisible, gwt.visibility.allParentsVisible);
    T_COMPARE(wv.hasUnclippedRegion, gwt.visibility.hasUnclippedRegion);
    T_COMPARE(wv.nonZeroSize, gwt.visibility.nonZeroSize);
    T_COMPARE(wv.onScreen, gwt.visibility.onScreen);
    T_COMPARE(wv.opaque, gwt.visibility.opaque);
    T_COMPARE(wv.renderableSetting, gwt.visibility.renderableSetting);
    T_COMPARE(wv.visibleSetting, gwt.visibility.visibleSetting);
    T_COMPARE(widget->isClickThrough(), gwt.is_click_through);
    T_COMPARE(widget->isMouseOver(), gwt.is_mouse_over);
    T_CHECK(widget->getFocusableType() == gwt.focusable_type);
    T_COMPARE(widget->isFocused(), gwt.is_focused);
    T_COMPARE(widget->getClipChildren(), gwt.clip_children);
    T_COMPARE(widget->getForceCustomCursor(), gwt.force_custom_cursor);
    T_CHECK(widget->getParent() == gwt.parent);
    auto rect_to_str = &WidgetTests::floatRectToStr;
    auto rect_approx_cmp = &WidgetTests::rectApproxCmp;
    T_COMPARE(widget->getLocalBounds(), gwt.local_bounds, rect_to_str, rect_approx_cmp);
    T_COMPARE(widget->getParentLocalBounds(), gwt.parent_local_bounds, rect_to_str, rect_approx_cmp);
    T_COMPARE(widget->getGlobalBounds(), gwt.global_bounds, rect_to_str, rect_approx_cmp);
    T_COMPARE(widget->getVisualLocalBounds(), gwt.visual_local_bounds, rect_to_str, rect_approx_cmp);
    T_COMPARE(widget->getVisualParentLocalBounds(), gwt.visual_parent_local_bounds, rect_to_str, rect_approx_cmp);
    T_COMPARE(widget->getVisualGlobalBounds(), gwt.visual_global_bounds, rect_to_str, rect_approx_cmp);
    T_COMPARE(widget->getUnclippedRegion(), gwt.visual_global_bounds, rect_to_str, rect_approx_cmp);
    sf::FloatRect quantized = gwt.visual_global_bounds;
    quantized.left = floor(quantized.left);
    quantized.top = floor(quantized.top);
    quantized.width = floor(quantized.width);
    quantized.height = floor(quantized.height);
    T_COMPARE(widget->getQuantizedUnclippedRegion(), quantized, rect_to_str, rect_approx_cmp);
    T_COMPARE(widget->getWidth(), gwt.parent_local_bounds.width);
    T_APPROX_COMPARE(widget->getHeight(), gwt.parent_local_bounds.height);
    T_APPROX_COMPARE(widget->getGlobalWidth(), gwt.parent_local_bounds.width);
    T_APPROX_COMPARE(widget->getGlobalHeight(), gwt.parent_local_bounds.height);
    T_VEC2_APPROX_COMPARE(widget->getSize(), gwt.local_bounds.getSize());
    auto get_corners = [&](const sf::FloatRect& bounds) {
        std::vector<sf::Vector2f> corners(4);
        corners[0] = bounds.getPosition();
        corners[1] = bounds.getPosition() + sf::Vector2f(gwt.local_bounds.width, 0.0f);
        corners[2] = bounds.getPosition() + sf::Vector2f(0.0f, bounds.height);
        corners[3] = bounds.getPosition() + bounds.getSize();
        return corners;
    };
    std::vector<sf::Vector2f> parent_local_corners = get_corners(gwt.parent_local_bounds);
    std::vector<sf::Vector2f> global_corners = get_corners(gwt.global_bounds);
    std::vector<sf::Vector2f> visual_global_corners = get_corners(gwt.visual_global_bounds);
    T_VEC2_APPROX_COMPARE(widget->getTopLeft(), parent_local_corners[0]);
    T_VEC2_APPROX_COMPARE(widget->getTopRight(), parent_local_corners[1]);
    T_VEC2_APPROX_COMPARE(widget->getBottomLeft(), parent_local_corners[2]);
    T_VEC2_APPROX_COMPARE(widget->getBottomRight(), parent_local_corners[3]);
    T_VEC2_APPROX_COMPARE(widget->getGlobalTopLeft(), global_corners[0]);
    T_VEC2_APPROX_COMPARE(widget->getGlobalTopRight(), global_corners[1]);
    T_VEC2_APPROX_COMPARE(widget->getGlobalBottomLeft(), global_corners[2]);
    T_VEC2_APPROX_COMPARE(widget->getGlobalBottomRight(), global_corners[3]);
    T_VEC2_APPROX_COMPARE(widget->getVisualGlobalTopLeft(), visual_global_corners[0]);
    T_VEC2_APPROX_COMPARE(widget->getVisualGlobalTopRight(), visual_global_corners[1]);
    T_VEC2_APPROX_COMPARE(widget->getVisualGlobalBottomLeft(), visual_global_corners[2]);
    T_VEC2_APPROX_COMPARE(widget->getVisualGlobalBottomRight(), visual_global_corners[3]);
}

bool WidgetTests::layer_contains(const std::vector<Node*>& layer, const std::string& name) {
    for (Node* node : layer) {
        if (node->str == name) {
            return true;
        }
    }
    return false;
}

TestApplication::TestApplication(sf::RenderWindow& window) : Application(window) { }

void TestApplication::onInit() {
    initialized = true;
}

void TestApplication::onStart() {
    started = true;
}

void TestApplication::onFrameBegin() {
    frame_begin = true;
}

void TestApplication::onFrameEnd() {
    frame_end = true;
}

void TestApplication::onProcessWidgets() {
    process_widgets = true;
}

void TestApplication::onProcessWindowEvent(const sf::Event& event) {
    process_window_event = true;
}

void TestApplication::onProcessKeyboardEvent(const sf::Event& event) {
    process_keyboard_event = true;
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            space_key_pressed = true;
        }
    } else if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::Space) {
            space_key_pressed = false;
        }
    }
}

void TestApplication::onProcessLeftPress() {
    process_left_press = true;
    left_click_pos = getMousePos();
}

void TestApplication::onProcessRightPress() {
    process_right_press = true;
    right_click_pos = getMousePos();
}

void TestApplication::onProcessLeftRelease() {
    process_left_release = true;
    left_release_pos = getMousePos();
}

void TestApplication::onProcessRightRelease() {
    process_right_release = true;
    right_release_pos = getMousePos();
}

void TestApplication::onProcessMouseMove() {
    process_mouse_move = true;
}

void TestApplication::onProcessMouseScrollX(float delta) {
    process_mouse_scroll_x = true;
    scroll_x_delta = delta;
}

void TestApplication::onProcessMouseScrollY(float delta) {
    process_mouse_scroll_y = true;
    scroll_y_delta = delta;
}

void TestApplication::onProcessKeyboard() {
    process_keyboard = true;
}

void TestApplication::onProcessMouse() {
    process_mouse = true;
}

void TestApplication::onAfterProcessInput() {
    after_process_input = true;
}

void TestApplication::onProcessWorld() {
    process_world = true;
}

void TestApplication::onRender() {
    rendered = true;
}

void TestApplication::onClose() {
    closed = true;
}

GenericWidgetTest::GenericWidgetTest(
    fw::Application& application,
    test::Test& test
) : application(application), test(test) { }

Node::Node(const std::string& str) {
    this->str = str;
}

void Node::addParent(Node* parent) {
    parents.push_back(parent);
}

const std::vector<Node*>& Node::getParents(Node* node) {
    return node->parents;
}

Node* NodeList::createNode(const std::string& name) {
    DataPointer<Node> uptr = make_data_pointer<Node>("Node " + name, name);
    Node* ptr = uptr.get();
    nodes.add(std::move(uptr));
    return ptr;
}

const std::vector<Node*>& NodeList::getNodes() const {
    return nodes.getVector();
}
