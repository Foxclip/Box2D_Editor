#include "widgets/application.h"

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

    void Application::start() {
        onStart();
        mainLoop();
    }

    void Application::onInit() { }

    void Application::onStart() { }

    void Application::onFrameBegin() { }

    void Application::onFrameEnd() { }

    void Application::onProcessWidgets() { }

    void Application::onProcessWindowEvent(const sf::Event& event) { }

    void Application::onProcessKeyboardEvent(const sf::Event& event) { }

    void Application::beforeProcessMouseEvent(const sf::Event& event) { }

    void Application::onProcessMouseEvent(const sf::Event& event) { }

    void Application::onProcessLeftClick() { }

    void Application::onProcessLeftRelease() { }

    void Application::onProcessMouseScroll(const sf::Event& event) { }

    void Application::onProcessKeyboard() { }

    void Application::onProcessMouse() { }

    void Application::afterProcessInput() { }

    void Application::onProcessWorld() { }

    void Application::onRender() { }

    void Application::mainLoop() {
        while (window.isOpen()) {
            onFrameBegin();
            processWidgets();
            processInput();
            processWorld();
            render();
            onFrameEnd();
        }
    }

    void Application::processWidgets() {
        widgets.unlock();
        widgets.reset(sf::Vector2f(window.getSize().x, window.getSize().y), mousePosf);
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
                    leftButtonPressGesture = true;
                    leftButtonProcessWidgetsOnRelease = true;
                    mousePressPosf = mousePosf;
                    processLeftClick();
                    leftButtonProcessWidgetsOnPress = true;
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
                    leftButtonPressGesture = false;
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

    void Application::processLeftClick() {
        if (leftButtonProcessWidgetsOnPress) {
            widgets.processClick(mousePosf);
            if (widgets.isClickBlocked()) {
                return;
            }
        }
        onProcessLeftClick();
    }

    void Application::processLeftRelease() {
        if (!leftButtonPressGesture) {
            return;
        }
        if (leftButtonProcessWidgetsOnRelease && widgets.getFocusedWidget()) {
            widgets.processRelease(mousePosf);
            if (widgets.isReleaseBlocked()) {
                return;
            }
        }
        onProcessLeftRelease();
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
        onProcessMouse();
        mousePrevPos = mousePos;
    }


    void Application::processWorld() {
        onProcessWorld();
    }

    void Application::render() {
        window.clear(sf::Color(0, 0, 0));
        window_view.setCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
        window_view.setSize(window.getSize().x, window.getSize().y);
        window.setView(window_view);
        onRender();
        widgets.render(window);
        window.display();
    }

}
