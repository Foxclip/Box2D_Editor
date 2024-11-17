#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_application.h"
#include "tests/widget_tests/widget_tests_basic.h"
#include "tests/widget_tests/widget_tests_text.h"
#include "tests/widget_tests/widget_tests_toposort.h"
#include "common/utils.h"

WidgetTests::WidgetTests(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : TestModule(name, parent, required_nodes) {
    test::TestModule* toposort_list = addModule<WidgetTestsToposort>("Toposort");
    test::TestModule* application_list = addModule<WidgetTestsApplication>("Application", { toposort_list });
    test::TestModule* widgets_basic_list = addModule<WidgetTestsBasic>("WidgetsBasic", { application_list });
    test::TestModule* text_widget_list = addModule<WidgetTestsText>("TextWidget", { application_list });

    test::TestModule* checkbox_widget_list = addModule("CheckboxWidget", { widgets_basic_list });
    test::Test* checkbox_widget_basic_test = checkbox_widget_list->addTest("basic", [&](test::Test& test) { checkboxWidgetBasicTest(test); });
    test::Test* checkbox_widget_toggle_test = checkbox_widget_list->addTest("toggle", { checkbox_widget_basic_test }, [&](test::Test& test) { checkboxWidgetToggleTest(test); });

    test::TestModule* container_widget_list = addModule("ContainerWidget", { widgets_basic_list });
    test::Test* container_widget_basic_test = container_widget_list->addTest("basic", [&](test::Test& test) { containerWidgetBasicTest(test); });
    test::Test* container_widget_children_test = container_widget_list->addTest("children", { container_widget_basic_test }, [&](test::Test& test) { containerWidgetChildrenTest(test); });
    test::Test* container_widget_padding_test = container_widget_list->addTest("padding", { container_widget_children_test }, [&](test::Test& test) { containerWidgetPaddingTest(test); });

    test::TestModule* size_policy_list = addModule("SizePolicy", { widgets_basic_list });
    test::Test* size_policy_basic_test = size_policy_list->addTest("basic", [&](test::Test& test) { sizePolicyTest(test); });
    test::Test* size_policy_position_test = size_policy_list->addTest("position", [&](test::Test& test) { sizePolicyPositionTest(test); });
    test::Test* size_policy_expand_test = size_policy_list->addTest("expand", [&](test::Test& test) { sizePolicyExpandTest(test); });
    test::Test* size_policy_limits_test = size_policy_list->addTest("limits", { size_policy_expand_test }, [&](test::Test& test) { sizePolicyLimitsTest(test); });
    std::vector<test::TestNode*> size_policy_tests = { size_policy_basic_test, size_policy_position_test, size_policy_expand_test, size_policy_limits_test };
    test::Test* size_policy_combined_test = size_policy_list->addTest("combined", size_policy_tests, [&](test::Test& test) { sizePolicyCombinedTest(test); });

    test::TestModule* widget_link_list = addModule("WidgetLink", { widgets_basic_list, size_policy_list });
    test::Test* widget_link_basic_test = widget_link_list->addTest("basic", [&](test::Test& test) { widgetLinkBasicTest(test); });
    test::Test* widget_link_targets_func_test = widget_link_list->addTest("target_func", [&](test::Test& test) { widgetLinkTargetsFuncTest(test); });
    test::Test* widget_link_container_test = widget_link_list->addTest("container", { widget_link_basic_test }, [&](test::Test& test) { widgetLinkContainerTest(test); });
    test::Test* widget_link_remove_test = widget_link_list->addTest("remove", { widget_link_basic_test }, [&](test::Test& test) { widgetLinkRemoveTest(test); });

    test::TestModule* textbox_widget_list = addModule("TextBoxWidget", { widgets_basic_list, text_widget_list });
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

    test::TestModule* canvas_widget_list = addModule("CanvasWidget", { widgets_basic_list });
    test::Test* canvas_widget_basic_test = canvas_widget_list->addTest("basic", [&](test::Test& test) { canvasWidgetBasicTest(test); });
    test::Test* canvas_widget_draw_test = canvas_widget_list->addTest("draw", { canvas_widget_basic_test }, [&](test::Test& test) { canvasWidgetDrawTest(test); });
    test::Test* canvas_widget_alpha_test = canvas_widget_list->addTest("alpha", { canvas_widget_basic_test }, [&](test::Test& test) { canvasWidgetAlphaTest(test); });

    test::TestModule* window_widget_list = addModule("WindowWidget", { widgets_basic_list, text_widget_list });
    test::Test* window_widget_basic_test = window_widget_list->addTest("basic", [&](test::Test& test) { windowWidgetBasicTest(test); });
    test::Test* window_widget_drag_test = window_widget_list->addTest("drag", { window_widget_basic_test }, [&](test::Test& test) { windowWidgetDragTest(test); });
    test::Test* window_widget_children_test = window_widget_list->addTest("children", { window_widget_basic_test }, [&](test::Test& test) { windowWidgetChildrenTest(test); });
    test::Test* window_widget_resize_test = window_widget_list->addTest("resize", { window_widget_basic_test }, [&](test::Test& test) { windowWidgetResizeTest(test); });
    test::Test* window_widget_chain_test = window_widget_list->addTest("chain", { window_widget_children_test, window_widget_resize_test }, [&](test::Test& test) { windowWidgetChainTest(test); });
    test::Test* window_widget_drag_limits_test = window_widget_list->addTest("drag_limits", { window_widget_drag_test, window_widget_chain_test }, [&](test::Test& test) { windowWidgetDragLimitsTest(test); });
    test::Test* window_widget_resize_limits_test = window_widget_list->addTest("resize_limits", { window_widget_chain_test }, [&](test::Test& test) { windowWidgetResizeLimitsTest(test); });
    test::Test* window_widget_move_to_top_drag_test = window_widget_list->addTest("move_to_top_drag", { window_widget_drag_test }, [&](test::Test& test) { windowWidgetMoveToTopDragTest(test); });
    test::Test* window_widget_move_to_top_resize_test = window_widget_list->addTest("move_to_top_resize", { window_widget_drag_test, window_widget_resize_test }, [&](test::Test& test) { windowWidgetMoveToTopResizeTest(test); });

    test::TestModule* dropdown_widget_list = addModule("DropdownWidget", { widgets_basic_list, text_widget_list });
    test::Test* dropdown_widget_basic_test = dropdown_widget_list->addTest("basic", [&](test::Test& test) { dropdownWidgetBasicTest(test); });
    test::Test* dropdown_widget_options_1_test = dropdown_widget_list->addTest("options_1", { dropdown_widget_basic_test }, [&](test::Test& test) { dropdownWidgetOptions1Test(test); });
    test::Test* dropdown_widget_options_2_test = dropdown_widget_list->addTest("options_2", { dropdown_widget_options_1_test }, [&](test::Test& test) { dropdownWidgetOptions2Test(test); });

    test::TestModule* scroll_area_widget_list = addModule("ScrollAreaWidget", { widgets_basic_list });
    test::Test* scroll_area_widget_basic_test = scroll_area_widget_list->addTest("basic", [&](test::Test& test) { scrollAreaWidgetBasicTest(test); });
    test::Test* scroll_area_widget_scroll_test = scroll_area_widget_list->addTest("scroll", { scroll_area_widget_basic_test }, [&](test::Test& test) { scrollAreaWidgetScrollTest(test); });
    test::Test* scroll_area_widget_scrollbar_visibility_test = scroll_area_widget_list->addTest("scrollbar_visibility", { scroll_area_widget_basic_test }, [&](test::Test& test) { scrollAreaWidgetScrollbarVisibilityTest(test); });
    test::Test* scroll_area_widget_scrollbar_container_test = scroll_area_widget_list->addTest("scrollbar_container", { scroll_area_widget_scrollbar_visibility_test }, [&](test::Test& test) { scrollAreaWidgetScrollbarContainerTest(test); });

    test::TestModule* tree_view_widget_list = addModule("TreeViewWidget", { widgets_basic_list });
    test::Test* tree_view_widget_basic_test = tree_view_widget_list->addTest("basic", [&](test::Test& test) { treeviewWidgetBasicTest(test); });
    test::Test* tree_view_widget_entries_test = tree_view_widget_list->addTest("entries", { tree_view_widget_basic_test }, [&](test::Test& test) { treeviewWidgetEntriesTest(test); });
    test::Test* tree_view_widget_parent1_test = tree_view_widget_list->addTest("parent_1", { tree_view_widget_entries_test }, [&](test::Test& test) { treeviewWidgetParent1Test(test); });
    test::Test* tree_view_widget_parent2_test = tree_view_widget_list->addTest("parent_2", { tree_view_widget_entries_test }, [&](test::Test& test) { treeviewWidgetParent2Test(test); });
    test::Test* tree_view_widget_select_test = tree_view_widget_list->addTest("select", { tree_view_widget_parent1_test }, [&](test::Test& test) { treeviewWidgetSelectTest(test); });
    test::Test* tree_view_widget_reorder_test = tree_view_widget_list->addTest("reorder", { tree_view_widget_parent2_test }, [&](test::Test& test) { treeviewWidgetReorderTest(test); });
    test::Test* tree_view_widget_remove_test = tree_view_widget_list->addTest("remove", { tree_view_widget_parent2_test }, [&](test::Test& test) { treeviewWidgetRemoveTest(test); });
    test::Test* tree_view_widget_clear_test = tree_view_widget_list->addTest("clear", { tree_view_widget_parent2_test }, [&](test::Test& test) { treeviewWidgetClearTest(test); });
    test::Test* tree_view_widget_drag_1_test = tree_view_widget_list->addTest("drag_1", { tree_view_widget_reorder_test }, [&](test::Test& test) { treeviewWidgetDrag1Test(test); });
    test::Test* tree_view_widget_drag_2_test = tree_view_widget_list->addTest("drag_2", { tree_view_widget_drag_1_test }, [&](test::Test& test) { treeviewWidgetDrag2Test(test); });
}

void WidgetTests::beforeRunModule() {
    sf::ContextSettings cs_window;
    window.create(sf::VideoMode(800, 600), "Widget tests", sf::Style::Default, cs_window);
    textbox_font = fw::Font("fonts/verdana.ttf");
}

void WidgetTests::afterRunModule() {
    window.close();
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

float WidgetTests::calcTreeViewEntryHeight(fw::TreeViewEntry* entry) {
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

float WidgetTests::calcTreeViewHeight(fw::TreeViewWidget* treeview) {
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

GenericWidgetTest::GenericWidgetTest(
    fw::Application& application,
    test::Test& test
) : application(application), test(test) { }
