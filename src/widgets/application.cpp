#include "widgets/application.h"
#include <Windows.h>
#include <winuser.h>

namespace fw {

    Application::Application() : window(internal_window) { }

    Application::Application(sf::RenderWindow& window) : window(window) {
        external_window = true;
    }

    void Application::init(
        const sf::String& window_title,
        unsigned int window_width,
        unsigned int window_height,
        unsigned int antialiasing,
        bool vsync
    ) {
        if (!premultiply.loadFromFile("shaders/premultiply.frag", sf::Shader::Fragment)) {
            throw std::runtime_error("Shader loading error");
        }
        sf::ContextSettings cs_window;
        cs_window.antialiasingLevel = antialiasing;
        if (external_window) {
            window.setSize(sf::Vector2u(window_width, window_height));
            window.setTitle(window_title);
        } else {
            window.create(sf::VideoMode(window_width, window_height), window_title, sf::Style::Default, cs_window);
        }
        window.setVerticalSyncEnabled(vsync);
        onInit();
    }

    void Application::start(bool external_control) {
        this->external_control = external_control;
        running = true;
        onStart();
        if (!external_control) {
            mainLoop();
        }
    }

    void Application::advance() {
        stage = Stage::NONE;
        onFrameBegin();
        stage = Stage::PROCESS;
        processWidgets();
        stage = Stage::BEFORE_INPUT;
        processBeforeInput();
        stage = Stage::INPUT;
        processInput();
        stage = Stage::AFTER_INPUT;
        processAfterInput();
        stage = Stage::WORLD;
        processWorld();
        stage = Stage::UPDATE;
        widgets.updateWidgets();
        stage = Stage::RENDER;
        render();
        stage = Stage::NONE;
        onFrameEnd();
    }

    void Application::maximizeWindow() const {
        sf::WindowHandle windowHandle = window.getSystemHandle();
        ShowWindow(windowHandle, SW_MAXIMIZE);
    }

    sf::Vector2u Application::getWindowSize() const {
        return window.getSize();
    }

    const fw::Font& Application::getDefaultFont() const {
        return widgets.getDefaultFont();
    }

    Stage Application::getStage() const {
        return stage;
    }

    Widget* Application::getLeftGestureSource() const {
        if (mouse_gesture_left.active) {
            return mouse_gesture_left.source;
        } else {
            return nullptr;
        }
    }

    Widget* Application::getRightGestureSource() const {
        if (mouse_gesture_right.active) {
            return mouse_gesture_right.source;
        } else {
            return nullptr;
        }
    }

    void Application::setWindowSize(unsigned int width, unsigned int height) {
        window.setSize(sf::Vector2u(width, height));
        if (external_control) {
            sf::Event event;
            event.type = sf::Event::Resized;
            event.size.width = width;
            event.size.height = height;
            addExternalEvent(event);
        }
    }

    void Application::setWindowSize(const sf::Vector2u& size) {
        window.setSize(size);
    }

    void Application::addExternalEvent(const sf::Event& event) {
        wAssert(external_control);
        external_event_queue.push(event);
    }

    void Application::mouseMove(int x, int y) {
        wAssert(external_control);
        external_mouse_pos = sf::Vector2i(x, y);
        sf::Event event;
        event.type = sf::Event::MouseMoved;
        event.mouseMove.x = x;
        event.mouseMove.y = y;
        addExternalEvent(event);
    }

    void Application::mouseMove(float x, float y) {
        mouseMove((int)x, (int)y);
    }

    void Application::mouseMove(const sf::Vector2i& pos) {
        mouseMove(pos.x, pos.y);
    }

    void Application::mouseMove(const sf::Vector2f& pos) {
        mouseMove(pos.x, pos.y);
    }

    void Application::mouseLeftPress() {
        wAssert(external_control);
        sf::Event event;
        event.type = sf::Event::MouseButtonPressed;
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = external_mouse_pos.x;
        event.mouseButton.y = external_mouse_pos.y;
        addExternalEvent(event);
    }

    void Application::mouseRightPress() {
        wAssert(external_control);
        sf::Event event;
        event.type = sf::Event::MouseButtonPressed;
        event.mouseButton.button = sf::Mouse::Right;
        event.mouseButton.x = external_mouse_pos.x;
        event.mouseButton.y = external_mouse_pos.y;
        addExternalEvent(event);
    }

    void Application::mouseLeftRelease() {
        wAssert(external_control);
        sf::Event event;
        event.type = sf::Event::MouseButtonReleased;
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = external_mouse_pos.x;
        event.mouseButton.y = external_mouse_pos.y;
        addExternalEvent(event);
    }

    void Application::mouseRightRelease() {
        wAssert(external_control);
        sf::Event event;
        event.type = sf::Event::MouseButtonReleased;
        event.mouseButton.button = sf::Mouse::Right;
        event.mouseButton.x = external_mouse_pos.x;
        event.mouseButton.y = external_mouse_pos.y;
        addExternalEvent(event);
    }

    void Application::mouseLeftClick() {
        mouseLeftPress();
        advance();
        mouseLeftRelease();
        advance();
    }

    void Application::mouseRightClick() {
        mouseRightPress();
        advance();
        mouseRightRelease();
        advance();
    }

    void Application::mouseLeftClick(const sf::Vector2f& pos) {
        mouseMove(pos);
        mouseLeftClick();
    }

    void Application::mouseRightClick(const sf::Vector2f& pos) {
        mouseMove(pos);
        mouseRightClick();
    }

    void Application::mouseScrollX(float delta) {
        wAssert(external_control);
        sf::Event event;
        event.type = sf::Event::MouseWheelScrolled;
        event.mouseWheelScroll.wheel = sf::Mouse::HorizontalWheel;
        event.mouseWheelScroll.delta = delta;
        event.mouseWheelScroll.x = external_mouse_pos.x;
        event.mouseWheelScroll.y = external_mouse_pos.y;
        addExternalEvent(event);
    }

    void Application::mouseScrollY(float delta) {
        wAssert(external_control);
        sf::Event event;
        event.type = sf::Event::MouseWheelScrolled;
        event.mouseWheelScroll.wheel = sf::Mouse::VerticalWheel;
        event.mouseWheelScroll.delta = delta;
        event.mouseWheelScroll.x = external_mouse_pos.x;
        event.mouseWheelScroll.y = external_mouse_pos.y;
        addExternalEvent(event);
    }

    void Application::keyPress(sf::Keyboard::Key key) {
        wAssert(external_control);
        if (key == sf::Keyboard::LControl) {
            external_lctrl_pressed = true;
        } else if (key == sf::Keyboard::LAlt) {
            external_lalt_pressed = true;
        } else if (key == sf::Keyboard::LShift) {
            external_lshift_pressed = true;
        }
        sf::Event event;
        event.type = sf::Event::KeyPressed;
        event.key.code = key;
        addExternalEvent(event);
    }

    void Application::keyRelease(sf::Keyboard::Key key) {
        wAssert(external_control);
        if (key == sf::Keyboard::LControl) {
            external_lctrl_pressed = false;
        } else if (key == sf::Keyboard::LAlt) {
            external_lalt_pressed = false;
        } else if (key == sf::Keyboard::LShift) {
            external_lshift_pressed = false;
        }
        sf::Event event;
        event.type = sf::Event::KeyReleased;
        event.key.code = key;
        addExternalEvent(event);
    }

    void Application::textEntered(sf::Uint32 code) {
        wAssert(external_control);
        sf::Event event;
        event.type = sf::Event::TextEntered;
        event.text.unicode = code;
        addExternalEvent(event);
    }

    bool Application::isLCtrlPressed() const {
        if (external_control) {
            return external_lctrl_pressed;
        } else {
            return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);
        }
    }

    bool Application::isLAltPressed() const {
        if (external_control) {
            return external_lalt_pressed;
        } else {
            return sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt);
        }
    }

    bool Application::isLShiftPressed() const {
        if (external_control) {
            return external_lshift_pressed;
        } else {
            return sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
        }
    }

    sf::Vector2i Application::getMousePos() const {
        if (external_control) {
            return external_mouse_pos;
        } else {
            return internal_mouse_pos;
        }
    }

    sf::Vector2f Application::getMousePosf() const {
        return to2f(getMousePos());
    }

    const sf::Vector2f& Application::getMousePressPosf() const {
        return mousePressPosf;
    }

    WidgetList& Application::getWidgets() {
        return widgets;
    }

    void Application::setBackgroundColor(const sf::Color& color) {
        background_color = color;
    }

    void Application::setDefaultFont(const fw::Font& font) {
        widgets.setDefaultFont(font);
    }

    void Application::setVerticalSyncEnabled(bool value) {
        window.setVerticalSyncEnabled(value);
    }

    void Application::close() {
        running = false;
        onClose();
    }

    void Application::onInit() { }

    void Application::onStart() { }

    void Application::onFrameBegin() { }

    void Application::onFrameEnd() { }

    void Application::onProcessWidgets() { }

    void Application::onProcessWindowEvent(const sf::Event& event) { }

    void Application::onProcessKeyboardEvent(const sf::Event& event) { }

    void Application::onProcessLeftPress() { }

    void Application::onProcessRightPress() { }

    void Application::onProcessLeftRelease() { }

    void Application::onProcessRightRelease() { }

    void Application::onProcessMouseMove() { }

    void Application::onProcessMouseScrollX(float delta) { }

    void Application::onProcessMouseScrollY(float delta) { }

    void Application::onProcessKeyboard() { }

    void Application::onProcessMouse() { }

    void Application::onBeforeProcessInput() { }

    void Application::onAfterProcessInput() { }

    void Application::onProcessWorld() { }

    void Application::onRender() { }

    void Application::onClose() { }

    void Application::startMoveGesture(Widget* source) {
        LoggerTag tag_mouse_gesture("mouseGesture");
        mouse_gesture_left = MouseGesture(source, MouseGesture::MOVE, getMousePosf(), sf::Mouse::Left);
        std::string source_str = source->getFullName();
        std::string type_str = "move";
        logger << "Start gesture: \n";
        LoggerIndent indent;
        logger << source_str << "\n";
        logger << type_str << "\n";
        logger << getMousePosf() << "\n";
    }

    void Application::endGestureLeft() {
        LoggerTag tag_mouse_gesture("mouseGesture");
        mouse_gesture_left.active = false;
        logger << "End gesture\n";
    }

    void Application::endGestureRight() {
        LoggerTag tag_mouse_gesture("mouseGesture");
        mouse_gesture_right.active = false;
        logger << "End gesture\n";
    }

    void Application::mainLoop() {
        wAssert(!external_window);
        while (window.isOpen() && running) {
            advance();
        }
        window.close();
    }

    void Application::processWidgets() {
        widgets.reset(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y), getMousePosf());
        onProcessWidgets();
    }

    void Application::processBeforeInput() {
        internal_mouse_pos = sf::Mouse::getPosition(window);
    }

    void Application::processInput() {
        if (external_control) {
            while (!external_event_queue.empty()) {
                sf::Event event = external_event_queue.front();
                external_event_queue.pop();
                processEvent(event);
            }
        } else {
            sf::Event event;
            while (window.pollEvent(event)) {
                processEvent(event);
            }
        }
        processKeyboard();
        processMouse();
        onAfterProcessInput();
    }

    void Application::processAfterInput() {
        widgets.processAfterInput();
    }

    void Application::processEvent(const sf::Event& event) {
        processWindowEvent(event);
        processKeyboardEvent(event);
        processMouseEvent(event);
    }

    void Application::processWindowEvent(const sf::Event& event) {
        widgets.processWindowEvent(event);
        onProcessWindowEvent(event);
    }

    void Application::processKeyboardEvent(const sf::Event& event) {
        if (widgets.getFocusedWidget()) {
            widgets.processKeyboardEvent(event);
        } else {
            onProcessKeyboardEvent(event);
        }
    }

    void Application::processMouseEvent(const sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed) {
            switch (event.mouseButton.button) {
                case sf::Mouse::Left:
                    leftButtonPressed = true;
                    mousePressPosf = getMousePosf();
                    processLeftPress();
                    break;
                case sf::Mouse::Right:
                    rightButtonPressed = true;
                    mousePrevPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                    processRightPress();
                    break;
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            switch (event.mouseButton.button) {
                case sf::Mouse::Left:
                    leftButtonPressed = false;
                    processLeftRelease();
                    break;
                case sf::Mouse::Right:
                    rightButtonPressed = false;
                    processRightRelease();
                    break;
            }
        } else if (event.type == sf::Event::MouseMoved) {
            processMouseMove();
        } else if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel) {
                processScrollX(event.mouseWheelScroll.delta);
            } else if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                processScrollY(event.mouseWheelScroll.delta);
            }
        }
    }

    void Application::startNormalGesture(Widget* source, sf::Mouse::Button button) {
        LoggerTag tag_mouse_gesture("mouseGesture");
        if (button == sf::Mouse::Left) {
            mouse_gesture_left = MouseGesture(source, MouseGesture::NORMAL, getMousePosf(), button);
        } else if (button == sf::Mouse::Right) {
            mouse_gesture_right = MouseGesture(source, MouseGesture::NORMAL, getMousePosf(), button);
        }
        std::string source_str = source->getFullName();
        std::string type_str = "normal";
        std::string button_str;
        switch (button) {
            case sf::Mouse::Left: button_str = "left"; break;
            case sf::Mouse::Right: button_str = "right"; break;
            case sf::Mouse::Middle: button_str = "middle"; break;
            default: button_str = "other"; break;
        }
        logger << "Start gesture: \n";
        LoggerIndent indent;
        logger << source_str << "\n";
        logger << type_str << "\n";
        logger << getMousePosf() << "\n";
        logger << button_str << "\n";
    }

    void Application::processLeftPress() {
        if (mouse_gesture_left.active) {
            if (mouse_gesture_left.type == MouseGesture::MOVE) {
                mouse_gesture_left.source->processLeftPress(getMousePosf(), false);
                onProcessLeftPress();
                endGestureLeft();
            }
        } else {
            widgets.processLeftPress(getMousePosf());
            onProcessLeftPress();
            Widget* blocking_widget = widgets.getBlockingWidget();
            startNormalGesture(blocking_widget, sf::Mouse::Left);
        }
    }

    void Application::processRightPress() {
        if (mouse_gesture_right.active) {
            endGestureRight();
        } else {
            widgets.processRightPress(getMousePosf());
            onProcessRightPress();
            Widget* blocking_widget = widgets.getBlockingWidget();
            startNormalGesture(blocking_widget, sf::Mouse::Right);
        }
    }

    void Application::processLeftRelease() {
        if (!mouse_gesture_left.active) {
            return;
        }
        widgets.processLeftRelease(getMousePosf());
        onProcessLeftRelease();
        endGestureLeft();
    }

    void Application::processRightRelease() {
        if (!mouse_gesture_right.active) {
            return;
        }
        widgets.processRightRelease(getMousePosf());
        onProcessRightRelease();
        endGestureRight();
    }

    void Application::processMouseMove() {
        widgets.processMouseMove(getMousePosf());
        onProcessMouseMove();
    }

    void Application::processScrollX(float delta) {
        widgets.processScrollX(getMousePosf(), delta);
        onProcessMouseScrollX(delta);
    }

    void Application::processScrollY(float delta) {
        widgets.processScrollY(getMousePosf(), delta);
        onProcessMouseScrollY(delta);
    }

    void Application::processKeyboard() {
        onProcessKeyboard();
    }

    void Application::processMouse() {
        sf::Vector2f mousePosf = getMousePosf();
        widgets.processMouse(mousePosf);
        Widget* gesture_source_left = getLeftGestureSource();
        Widget* gesture_source_right = getRightGestureSource();
        onProcessMouse();
        if (mouse_gesture_left.active && mouse_gesture_left.type == MouseGesture::NORMAL && gesture_source_left) {
            gesture_source_left->OnProcessDragGesture(sf::Mouse::Left, mousePosf);
        }
        if (mouse_gesture_right.active && mouse_gesture_right.type == MouseGesture::NORMAL && gesture_source_right) {
            gesture_source_right->OnProcessDragGesture(sf::Mouse::Right, mousePosf);
        }
        sf::Cursor::Type cursor_type = sf::Cursor::Arrow;
        widgets.getCurrentCursorType(cursor_type);
        setCursorType(cursor_type);
        mousePrevPos = getMousePos();
        mousePrevPosf = getMousePosf();
    }


    void Application::processWorld() {
        onProcessWorld();
    }

    void Application::render() {
        widgets.updateRenderQueue();
        widgets.lock();
        window.clear(background_color);
        window_view.setCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
        window_view.setSize((float)window.getSize().x, (float)window.getSize().y);
        window.setView(window_view);
        onRender();
        widgets.render(window);
        window.display();
        widgets.unlock();
    }

    void Application::setCursorType(sf::Cursor::Type type) {
        if (type == current_cursor_type) {
            return;
        }
        current_cursor_type = type;
        switch (type) {
            case sf::Cursor::Arrow: window.setMouseCursor(arrow_cursor); break;
            case sf::Cursor::Text: window.setMouseCursor(text_cursor); break;
            case sf::Cursor::SizeTopLeft: window.setMouseCursor(size_top_left_cursor); break;
            case sf::Cursor::SizeTop: window.setMouseCursor(size_top_cursor); break;
            case sf::Cursor::SizeTopRight: window.setMouseCursor(size_top_right_cursor); break;
            case sf::Cursor::SizeLeft: window.setMouseCursor(size_left_cursor); break;
            case sf::Cursor::SizeRight: window.setMouseCursor(size_right_cursor); break;
            case sf::Cursor::SizeBottomLeft: window.setMouseCursor(size_bottom_left_cursor); break;
            case sf::Cursor::SizeBottom: window.setMouseCursor(size_bottom_cursor); break;
            case sf::Cursor::SizeBottomRight: window.setMouseCursor(size_bottom_right_cursor); break;
            default: window.setMouseCursor(arrow_cursor); break;
        }
    }

    MouseGesture::MouseGesture() { }

    MouseGesture::MouseGesture(
        Widget* source,
        MouseGestureType type,
        sf::Vector2f startPos,
        sf::Mouse::Button button
    ) {
        this->source = source;
        this->type = type;
        this->startPos = startPos;
        this->button = button;
        active = true;
    }

}
