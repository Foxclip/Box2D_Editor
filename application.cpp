#include "application.h"

void Application::init() {
    sf::ContextSettings cs;
    cs.antialiasingLevel = ANTIALIASING;
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML", sf::Style::Default, cs);
    window->setVerticalSyncEnabled(true);
    shape = sf::CircleShape(10.0f);
    shape.setFillColor(sf::Color::Green);

    init_physics();

    view1 = sf::View(sf::FloatRect(0.0f, 0.0f, 800.0f, 600.0f));
    resetView();
}

void Application::start() {
    main_loop();
}

void Application::init_physics() {
    b2Vec2 gravity(0.0f, -10.0f);
    world = std::make_unique<b2World>(gravity);

    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, -10.0f);
    b2Body* groundBody = world->CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, 10.0f);
    groundBody->CreateFixture(&groundBox, 0.0f);

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.0f, 8.0f);
    body = world->CreateBody(&bodyDef);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1.0f, 1.0f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
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
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                mousePrevPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            }
        }
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                zoomFactor *= pow(MOUSE_SCROLL_ZOOM, -event.mouseWheelScroll.delta);
            }
        }
        sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mouseDelta = mousePrevPos - mousePos;
            viewCenterX += mouseDelta.x * zoomFactor;
            viewCenterY += mouseDelta.y * zoomFactor;
        }
        mousePrevPos = mousePos;
    }
}

void Application::process_world() {
    world->Step(timeStep, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
    b2Vec2 position = body->GetPosition();
    float angle = body->GetAngle();
    printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
}

void Application::render() {
    window->clear();
    view1.setCenter(viewCenterX, viewCenterY);
    view1.setSize(window->getSize().x * zoomFactor, window->getSize().y * zoomFactor);
    window->setView(view1);

    window->draw(shape);

    window->display();
}
