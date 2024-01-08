#include "application.h"

void Application::init() {
    sf::ContextSettings cs;
    cs.antialiasingLevel = ANTIALIASING;
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML", sf::Style::Default, cs);
    window->setVerticalSyncEnabled(true);
    init_objects();
    view1 = sf::View(sf::FloatRect(0.0f, 0.0f, 800.0f, 600.0f));
}

void Application::start() {
    main_loop();
}

void Application::init_objects() {
    b2Vec2 gravity(0.0f, -10.0f);
    world = std::make_unique<b2World>(gravity);
    GameObject* ground = create_static_box(0.0f, 0.0f, 100.0f, 10.0f, sf::Color::Blue);
    GameObject* box = create_dynamic_box(0.0f, 50.0f, 0.5f, 10.0f, 10.0f, sf::Color::Green);
    box->fixture->SetRestitution(0.0f);
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
}

void Application::render() {
    window->clear();
    view1.setCenter(viewCenterX, viewCenterY);
    view1.setSize(window->getSize().x * zoomFactor, window->getSize().y * zoomFactor);
    window->setView(view1);

    for (int i = 0; i < game_objects.size(); i++) {
        GameObject* object = game_objects[i].get();
        object->UpdateVisual();
        window->draw(*object->shape.get());
    }

    window->display();
}

GameObject* Application::create_static_box(float pos_x, float pos_y, float width,  float height, sf::Color color) {
    b2BodyDef bodyDef;
    bodyDef.position.Set(pos_x, pos_y);
    b2Body* body = world->CreateBody(&bodyDef);
    b2PolygonShape box_shape;
    box_shape.SetAsBox(width / 2.0f, height / 2.0f);
    b2Fixture* fixture = body->CreateFixture(&box_shape, 0.0f);
    std::unique_ptr<sf::Shape> shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(width, height));
    shape->setOrigin(width / 2.0f, height / 2.0f);
    shape->setFillColor(color);
    std::unique_ptr<GameObject> uptr = std::make_unique<GameObject>(std::move(shape), body, fixture);
    GameObject* ptr = uptr.get();
    game_objects.push_back(std::move(uptr));
    return ptr;
}

GameObject* Application::create_dynamic_box(float pos_x, float pos_y, float angle, float width, float height, sf::Color color) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(pos_x, pos_y);
    bodyDef.angle = angle;
    b2Body* body = world->CreateBody(&bodyDef);
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(width / 2.0f, height / 2.0f);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.5f;
    b2Fixture* fixture = body->CreateFixture(&fixtureDef);
    std::unique_ptr<sf::Shape> shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(width, height));
    shape->setOrigin(width / 2.0f, height / 2.0f);
    shape->setFillColor(color);
    std::unique_ptr<GameObject> uptr = std::make_unique<GameObject>(std::move(shape), body, fixture);
    GameObject* ptr = uptr.get();
    game_objects.push_back(std::move(uptr));
    return ptr;
}
