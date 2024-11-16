#include "tests/widget_tests.h"

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
