#include "application.h"
#include "utils.h"
#include <numbers>

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
    b2Vec2 gravity(0.0f, -9.8f);
    world = std::make_unique<b2World>(gravity);
    std::vector<b2Vec2> ground_vertices = {
        b2Vec2(10.0f, 1.0f),
        b2Vec2(5.0f, 0.0f),
        b2Vec2(-5.0f, 0.0f),
        b2Vec2(-10.0f, 1.0f),
    };
    GameObject* ground = create_ground(b2Vec2(0.0f, 0.0f), ground_vertices, sf::Color::White);
    GameObject* box0 = create_box(b2Vec2(0.0f, 1.0f), utils::to_radians(10.0f), b2Vec2(1.0f, 1.0f), sf::Color::Green);
    GameObject* box1 = create_box(b2Vec2(0.0f, 2.5f), utils::to_radians(20.0f), b2Vec2(1.0f, 1.0f), sf::Color::Green);
    GameObject* box2 = create_box(b2Vec2(0.0f, 4.0f), utils::to_radians(30.0f), b2Vec2(1.0f, 1.0f), sf::Color::Green);
    std::vector<GameObject*> boxes = { box0, box1, box2 };
    for (int i = 0; i < boxes.size(); i++) {
        GameObject* box = boxes[i];
        box->SetType(b2_dynamicBody);
        box->SetDensity(1.0f);
        box->SetFriction(0.3f);
        box->SetRestitution(0.5f);
    }
    std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
    GameObject* car = create_car(b2Vec2(0.0f, 8.0f), lengths, sf::Color::Red);
    car->SetType(b2_dynamicBody);
    car->SetDensity(1.0f);
    car->SetFriction(0.3f);
    car->SetRestitution(0.5f);
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
        window->draw(*object->drawable.get());
    }

    window->display();
}

GameObject* Application::create_box(b2Vec2 pos, float angle, b2Vec2 size, sf::Color color) {
    b2BodyDef bodyDef;
    bodyDef.position = pos;
    bodyDef.angle = angle;
    b2Body* body = world->CreateBody(&bodyDef);
    b2PolygonShape box_shape;
    box_shape.SetAsBox(size.x / 2.0f, size.y / 2.0f);
    b2Fixture* fixture = body->CreateFixture(&box_shape, 1.0f);
    std::unique_ptr<sf::Shape> shape = std::make_unique<sf::RectangleShape>(sf::Vector2f(size.x, size.y));
    shape->setOrigin(size.x / 2.0f, size.y / 2.0f);
    shape->setFillColor(color);
    std::unique_ptr<GameObject> uptr = std::make_unique<GameObject>(std::move(shape), body);
    GameObject* ptr = uptr.get();
    game_objects.push_back(std::move(uptr));
    return ptr;
}

GameObject* Application::create_car(b2Vec2 pos, std::vector<float> lengths, sf::Color color) {
    std::unique_ptr<sf::ConvexShape> shape = std::make_unique<sf::ConvexShape>(lengths.size());
    float pi = std::numbers::pi;
    auto get_pos = [&](int i) {
        float angle = (float)i / lengths.size() * 2 * pi;
        b2Vec2 vector = b2Vec2(std::cos(angle), std::sin(angle));
        int index = i < lengths.size() ? i : i % lengths.size();
        b2Vec2 pos = lengths[index] * vector;
        return pos;
    };
    b2BodyDef bodyDef;
    bodyDef.position = pos;
    b2Body* body = world->CreateBody(&bodyDef);
    for (int i = 0; i < lengths.size(); i++) {
        b2Vec2 vertices[3];
        vertices[0] = b2Vec2(0.0f, 0.0f);
        vertices[1] = get_pos(i);
        vertices[2] = get_pos(i + 1);
        b2PolygonShape triangle;
        triangle.Set(vertices, 3);
        b2Fixture* fixture = body->CreateFixture(&triangle, 1.0f);
        shape->setPoint(i, sf::Vector2f(vertices[1].x, -vertices[1].y));
    }

    shape->setFillColor(color);
    std::unique_ptr<GameObject> uptr = std::make_unique<GameObject>(std::move(shape), body);
    uptr->position = pos;
    GameObject* ptr = uptr.get();
    game_objects.push_back(std::move(uptr));
    return ptr;
}

GameObject* Application::create_ground(b2Vec2 pos, std::vector<b2Vec2> vertices, sf::Color color) {
    b2BodyDef bodyDef;
    bodyDef.position = pos;
    b2Body* body = world->CreateBody(&bodyDef);
    b2ChainShape chain;
    chain.CreateChain(vertices.data(), vertices.size(), vertices.front(), vertices.back());
    b2Fixture* fixture = body->CreateFixture(&chain, 1.0f);

    sf::VertexArray drawable_vertices(sf::LinesStrip, vertices.size());
    for (int i = 0; i < vertices.size(); i++) {
        drawable_vertices[i].position = sf::Vector2f(vertices[i].x, -vertices[i].y);
    }
    std::unique_ptr<LineStripShape> line_strip = std::make_unique<LineStripShape>(drawable_vertices);
    line_strip->setLineColor(color);
    std::unique_ptr<GameObject> uptr = std::make_unique<GameObject>(std::move(line_strip), body);
    GameObject* ptr = uptr.get();
    game_objects.push_back(std::move(uptr));
    return ptr;
}
