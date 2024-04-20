#include "widgets/application.h"
#include <Windows.h>
#include <winuser.h>

Logger& operator<<(Logger& lg, const sf::Vector2f& value) {
    return lg << "(" << value.x << " " << value.y << ")";
}

namespace fw {

    void Application::init(
        const sf::String& window_title,
        unsigned int window_width,
        unsigned int window_height,
        unsigned int antialiasing
    ) {
        sf::ContextSettings cs_window;
        cs_window.antialiasingLevel = antialiasing;
        window.create(sf::VideoMode(window_width, window_height), window_title, sf::Style::Default, cs_window);
        window.setVerticalSyncEnabled(true);
        onInit();
    }

    void Application::start(bool loop) {
        onStart();
        if (loop) {
            mainLoop();
        }
    }

    void Application::advance() {
        onFrameBegin();
        processWidgets();
        processInput();
        processWorld();
        render();
        onFrameEnd();
    }

    void Application::maximizeWindow() const {
        sf::WindowHandle windowHandle = window.getSystemHandle();
        ShowWindow(windowHandle, SW_MAXIMIZE);
    }

    sf::Vector2u Application::getWindowSize() const {
        return window.getSize();
    }

    void Application::close() {
        window.close();
        onClose();
    }

    void Application::onInit() { }

    void Application::onStart() { }

    void Application::onFrameBegin() { }

    void Application::onFrameEnd() { }

    void Application::onProcessWidgets() { }

    void Application::onProcessWindowEvent(const sf::Event& event) { }

    void Application::onProcessKeyboardEvent(const sf::Event& event) { }

    void Application::beforeProcessMouseEvent(const sf::Event& event) { }

    void Application::onProcessLeftClick() { }

    void Application::onProcessLeftRelease() { }

    void Application::onProcessMouseScroll(const sf::Event& event) { }

    void Application::onProcessKeyboard() { }

    void Application::onProcessMouse() { }

    void Application::afterProcessInput() { }

    void Application::onProcessWorld() { }

    void Application::onRender() { }

    void Application::onClose() { }

    void Application::startMoveGesture() {
        startMoveGesture(MouseGesture::SCREEN);
    }

    void Application::mainLoop() {
        while (window.isOpen()) {
            advance();
        }
    }

    void Application::processWidgets() {
        widgets.unlock();
        widgets.reset(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y), mousePosf);
        onProcessWidgets();
    }

    void Application::processInput() {
        mousePos = sf::Mouse::getPosition(window);
        mousePosf = to2f(mousePos);
        sf::Event event;
        while (window.pollEvent(event)) {
            processWindowEvent(event);
            processKeyboardEvent(event);
            processMouseEvent(event);
        }
        processKeyboard();
        processMouse();
        afterProcessInput();
        widgets.updateWidgets();
        widgets.updateRenderQueue();
        widgets.lock();
    }

    void Application::processWindowEvent(const sf::Event& event) {
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
        beforeProcessMouseEvent(event);
        if (event.type == sf::Event::MouseButtonPressed) {
            switch (event.mouseButton.button) {
                case sf::Mouse::Left:
                    leftButtonPressed = true;
                    mousePressPosf = mousePosf;
                    processLeftClick();
                    break;
                case sf::Mouse::Right:
                    rightButtonPressed = true;
                    mousePrevPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                    break;
            }
        }
        if (event.type == sf::Event::MouseButtonReleased) {
            switch (event.mouseButton.button) {
                case sf::Mouse::Left:
                    leftButtonPressed = false;
                    processLeftRelease();
                    break;
                case sf::Mouse::Right:
                    rightButtonPressed = false;
                    break;
            }
        }
        if (event.type == sf::Event::MouseWheelScrolled) {
            onProcessMouseScroll(event);
        }
    }

    void Application::startNormalGesture(
        MouseGesture::MouseGestureSource source,
        sf::Mouse::Button button
    ) {
        LoggerTag tag_mouse_gesture("mouseGesture");
        mouseGesture = MouseGesture(source, MouseGesture::NORMAL, mousePosf, button);
        std::string source_str = source == MouseGesture::WIDGETS ? "widgets" : "screen";
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
        logger << mousePosf << "\n";
        logger << button_str << "\n";
    }

    void Application::startMoveGesture(
        MouseGesture::MouseGestureSource source
    ) {
        LoggerTag tag_mouse_gesture("mouseGesture");
        mouseGesture = MouseGesture(source, MouseGesture::MOVE, mousePosf, sf::Mouse::Left);
        std::string source_str = source == MouseGesture::WIDGETS ? "widgets" : "screen";
        std::string type_str = "move";
        logger << "Start gesture: \n";
        LoggerIndent indent;
        logger << source_str << "\n";
        logger << type_str << "\n";
        logger << mousePosf << "\n";
    }

    void Application::endGesture() {
        LoggerTag tag_mouse_gesture("mouseGesture");
        mouseGesture.active = false;
        logger << "End gesture\n";
    }

    void Application::processLeftClick() {
        if (mouseGesture.active) {
            if (mouseGesture.type == MouseGesture::MOVE) {
                if (mouseGesture.source == MouseGesture::WIDGETS) {
                    // skipping for now
                } else if (mouseGesture.source == MouseGesture::SCREEN) {
                    onProcessLeftClick();
                }
                endGesture();
            }
        } else {
            widgets.processClick(mousePosf);
            if (widgets.isClickBlocked()) {
                startNormalGesture(MouseGesture::WIDGETS, sf::Mouse::Left);
            } else {
                startNormalGesture(MouseGesture::SCREEN, sf::Mouse::Left);
                onProcessLeftClick();
            }
        }
    }

    void Application::processLeftRelease() {
        if (!mouseGesture.active) {
            return;
        }
        if (mouseGesture.source == MouseGesture::WIDGETS) {
            widgets.processRelease(mousePosf);
        } else if (mouseGesture.source == MouseGesture::SCREEN) {
            onProcessLeftRelease();
        }
        endGesture();
    }

    void Application::processKeyboard() {
        onProcessKeyboard();
    }

    void Application::processMouse() {
        widgets.processMouse(mousePosf);
        sf::Cursor::Type cursor_type = sf::Cursor::Arrow;
        widgets.getCurrentCursorType(cursor_type);
        switch (cursor_type) {
            case sf::Cursor::Arrow: window.setMouseCursor(arrow_cursor); break;
            case sf::Cursor::Text: window.setMouseCursor(text_cursor); break;
            default: window.setMouseCursor(arrow_cursor); break;
        }
        bool non_screen_gesture = mouseGesture.active && mouseGesture.source != MouseGesture::SCREEN;
        if (!non_screen_gesture) {
            onProcessMouse();
        }
        mousePrevPos = mousePos;
    }


    void Application::processWorld() {
        onProcessWorld();
    }

    void Application::render() {
        window.clear(sf::Color(0, 0, 0));
        window_view.setCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
        window_view.setSize((float)window.getSize().x, (float)window.getSize().y);
        window.setView(window_view);
        onRender();
        widgets.render(window);
        window.display();
    }

    MouseGesture::MouseGesture() { }

    MouseGesture::MouseGesture(
        MouseGestureSource source,
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
