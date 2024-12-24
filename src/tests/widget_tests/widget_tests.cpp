#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_application.h"
#include "tests/widget_tests/widget_tests_basic.h"
#include "tests/widget_tests/widget_tests_canvas.h"
#include "tests/widget_tests/widget_tests_checkbox.h"
#include "tests/widget_tests/widget_tests_container.h"
#include "tests/widget_tests/widget_tests_dropdown.h"
#include "tests/widget_tests/widget_tests_scroll_area.h"
#include "tests/widget_tests/widget_tests_size_policy.h"
#include "tests/widget_tests/widget_tests_text.h"
#include "tests/widget_tests/widget_tests_textbox.h"
#include "tests/widget_tests/widget_tests_tree_view.h"
#include "tests/widget_tests/widget_tests_toposort.h"
#include "tests/widget_tests/widget_tests_widget_link.h"
#include "tests/widget_tests/widget_tests_window.h"
#include "common/utils.h"

WidgetTests::WidgetTests(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : TestModule(name, parent, required_nodes) {
    WidgetTestsToposort* toposort_list = addModule<WidgetTestsToposort>("Toposort");
    WidgetTestsApplication* application_list = addModule<WidgetTestsApplication>("Application", { toposort_list });
    WidgetTestsBasic* widgets_basic_list = addModule<WidgetTestsBasic>("WidgetsBasic", { application_list });
    WidgetTestsText* text_widget_list = addModule<WidgetTestsText>("TextWidget", { application_list });
    WidgetTestsCheckbox* checkbox_widget_list = addModule<WidgetTestsCheckbox>("CheckboxWidget", { widgets_basic_list });
    WidgetTestsContainer* container_widget_list = addModule<WidgetTestsContainer>("ContainerWidget", { widgets_basic_list });
    WidgetTestsSizePolicy* size_policy_list = addModule<WidgetTestsSizePolicy>("SizePolicy", { widgets_basic_list });
    WidgetTestsWidgetLink* widget_link_list = addModule<WidgetTestsWidgetLink>("WidgetLink", { widgets_basic_list, size_policy_list });
    WidgetTestsTextbox* textbox_widget_list = addModule<WidgetTestsTextbox>("TextBoxWidget", { widgets_basic_list, text_widget_list });
    WidgetTestsCanvas* canvas_widget_list = addModule<WidgetTestsCanvas>("CanvasWidget", { widgets_basic_list });
    WidgetTestsWindow* window_widget_list = addModule<WidgetTestsWindow>("WindowWidget", { widgets_basic_list, text_widget_list });
    WidgetTestsDropdown* dropdown_widget_list = addModule<WidgetTestsDropdown>("DropdownWidget", { widgets_basic_list, text_widget_list });
    WidgetTestsScrollArea* scroll_area_widget_list = addModule<WidgetTestsScrollArea>("ScrollAreaWidget", { widgets_basic_list });
    WidgetTestsTreeView* tree_view_widget_list = addModule<WidgetTestsTreeView>("TreeViewWidget", { widgets_basic_list });
}

void WidgetTests::beforeRunModule() {
    sf::ContextSettings cs_window;
    window.create(sf::VideoMode(800, 600), "Widget tests", sf::Style::Default, cs_window);
    textbox_font = fw::Font("fonts/verdana.ttf");
    fw::WidgetList::debug_mouse = true;
}

void WidgetTests::afterRunModule() {
    window.close();
    fw::WidgetList::debug_mouse = false;
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

void WidgetTests::dragWindow(fw::Application& application, fw::WindowWidget* window, const sf::Vector2f& offset) {
    sf::Vector2f header_center = window->getHeaderWidget()->getGlobalCenter();
    application.mouseMove(header_center);
    application.advance();
    application.mouseLeftPress();
    application.advance();
    application.mouseMove(header_center + offset);
    application.advance();
    application.mouseLeftRelease();
    application.advance();
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
