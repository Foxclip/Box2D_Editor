#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_textbox.h"

WidgetTestsTextbox::WidgetTestsTextbox(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : TestModule(name, parent, required_nodes) {
    test::Test* textbox_widget_basic_test = addTest("basic", [&](test::Test& test) { textboxWidgetBasicTest(test); });
    test::Test* textbox_widget_input_test = addTest("input", { textbox_widget_basic_test }, [&](test::Test& test) { textboxWidgetInputTest(test); });
    test::Test* textbox_widget_events_test = addTest("events", { textbox_widget_input_test }, [&](test::Test& test) { textboxWidgetEventsTest(test); });
    test::Test* textbox_widget_cursor_test = addTest("cursor", { textbox_widget_basic_test }, [&](test::Test& test) { textboxWidgetCursorTest(test); });
    test::Test* textbox_widget_scroll_test = addTest("scroll", { textbox_widget_cursor_test }, [&](test::Test& test) { textboxWidgetScrollTest(test); });
    test::Test* textbox_widget_resizing_test = addTest("resizing", { textbox_widget_scroll_test }, [&](test::Test& test) { textboxWidgetResizingTest(test); });
    test::Test* textbox_widget_selection_test = addTest("selection", { textbox_widget_cursor_test }, [&](test::Test& test) { textboxWidgetSelectionTest(test); });
    test::Test* textbox_widget_mouse_click_test = addTest("mouse_click", { textbox_widget_cursor_test }, [&](test::Test& test) { textboxWidgetMouseClickTest(test); });
    test::Test* textbox_widget_mouse_drag_test = addTest("mouse_drag", { textbox_widget_mouse_click_test }, [&](test::Test& test) { textboxWidgetMouseDragTest(test); });
    test::Test* textbox_widget_copypaste_test = addTest("copypaste", { textbox_widget_selection_test }, [&](test::Test& test) { textboxWidgetCopyPasteTest(test); });
    test::Test* textbox_widget_history_test = addTest("history", { textbox_widget_copypaste_test }, [&](test::Test& test) { textboxWidgetHistoryTest(test); });
    test::Test* textbox_widget_integer_test = addTest("integer", { textbox_widget_copypaste_test }, [&](test::Test& test) { textboxWidgetIntegerTest(test); });
    test::Test* textbox_widget_float_test = addTest("float", { textbox_widget_copypaste_test }, [&](test::Test& test) { textboxWidgetFloatTest(test); });
}

void WidgetTestsTextbox::textboxWidgetBasicTest(test::Test& test) {
    fw::Application application(getWindow());
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
    T_WRAP_CONTAINER(WidgetTests::genericWidgetTest(gwt));

    T_COMPARE(textbox_widget->getFillColor(), sf::Color(50, 50, 50), &WidgetTests::colorToStr);
}

void WidgetTestsTextbox::textboxWidgetInputTest(test::Test& test) {
    fw::Application application(getWindow());
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

void WidgetTestsTextbox::textboxWidgetEventsTest(test::Test& test) {
    fw::Application application(getWindow());
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

void WidgetTestsTextbox::textboxWidgetCursorTest(test::Test& test) {
    fw::Application application(getWindow());
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

void WidgetTestsTextbox::textboxWidgetScrollTest(test::Test& test) {
    fw::Application application(getWindow());
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

void WidgetTestsTextbox::textboxWidgetResizingTest(test::Test& test) {
    fw::Application application(getWindow());
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

void WidgetTestsTextbox::textboxWidgetSelectionTest(test::Test& test) {
    fw::Application application(getWindow());
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

void WidgetTestsTextbox::textboxWidgetMouseClickTest(test::Test& test) {
    fw::Application application(getWindow());
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

void WidgetTestsTextbox::textboxWidgetMouseDragTest(test::Test& test) {
    fw::Application application(getWindow());
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

void WidgetTestsTextbox::textboxWidgetCopyPasteTest(test::Test& test) {
    fw::Application application(getWindow());
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

void WidgetTestsTextbox::textboxWidgetHistoryTest(test::Test& test) {
    fw::Application application(getWindow());
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

void WidgetTestsTextbox::textboxWidgetIntegerTest(test::Test& test) {
    fw::Application application(getWindow());
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

void WidgetTestsTextbox::textboxWidgetFloatTest(test::Test& test) {
    fw::Application application(getWindow());
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

sf::RenderWindow& WidgetTestsTextbox::getWindow() {
    return dynamic_cast<WidgetTests*>(parent)->window;
}

fw::Font& WidgetTestsTextbox::getFont() {
    return dynamic_cast<WidgetTests*>(parent)->textbox_font;
}

fw::TextBoxWidget* WidgetTestsTextbox::initTextBox(fw::Application& application, const std::string& test_name, float width, float height) {
    application.init(test_name, 800, 600, 0, false);
    application.start(true);
    application.mouseMove(400, 300);
    application.advance();
    fw::TextBoxWidget* textbox_widget = application.getWidgets().createTextBoxWidget();
    textbox_widget->setCharacterSize(20);
    textbox_widget->setFont(getFont());
    sf::Vector2f position(100.0f, 100.0f);
    sf::Vector2f size(width, height);
    std::string value = "Text";
    textbox_widget->setPosition(position);
    textbox_widget->setSize(size);
    textbox_widget->setValue(value);
    application.advance();
    return textbox_widget;
}
