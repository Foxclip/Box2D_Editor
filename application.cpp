#include "application.h"

void Application::init() {
    sf::ContextSettings cs;
    cs.antialiasingLevel = ANTIALIASING;
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML", sf::Style::Default, cs);
    window->setVerticalSyncEnabled(true);
    shape = sf::CircleShape(10.0f);
    shape.setFillColor(sf::Color::Green);
    view1 = sf::View(sf::FloatRect(0.0f, 0.0f, 800.0f, 600.0f));
    resetView();
}

void Application::start() {
    main_loop();
}

void Application::main_loop() {
    while (window->isOpen()) {
        process_input();
        process_world();
        render();
    }
}

void Application::resetView() {
    viewCenterX = 0.0f;
    viewCenterY = 0.0f;
    zoomFactor = 1.0f;
}

void Application::process_input() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
        }
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::R: resetView(); break;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window->close();
        }
    }
}

void Application::process_world() {
    shape.move(sf::Vector2f(1.0f, 0.0f));
}

void Application::render() {
    window->clear();
    view1.setCenter(viewCenterX, viewCenterY);
    view1.setSize(window->getSize().x * zoomFactor, window->getSize().y * zoomFactor);
    window->setView(view1);

    window->draw(shape);

    window->display();
}
