#include "tests/widget_tests/widget_tests.h"

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
