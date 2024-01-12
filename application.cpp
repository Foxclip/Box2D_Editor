#include "application.h"
#include "utils.h"
#include <numbers>
#include <iostream>

void Application::init() {
    sf::ContextSettings cs;
    cs.antialiasingLevel = ANTIALIASING;
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML", sf::Style::Default, cs);
    window->setVerticalSyncEnabled(true);
    init_objects();
    world_view = sf::View(sf::FloatRect(0.0f, 0.0f, 800.0f, 600.0f));
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
    GameObject* box1 = create_box(b2Vec2(0.0f, 5.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color::Green);
    GameObject* box2 = create_box(b2Vec2(0.0f, 10.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color::Green);
    std::vector<GameObject*> boxes = { box0, box1, box2 };
    //std::vector<GameObject*> boxes = { box1, box2 };
    //std::vector<GameObject*> boxes = { box0 };
    for (int i = 0; i < boxes.size(); i++) {
        GameObject* box = boxes[i];
        box->SetType(b2_dynamicBody);
        box->SetDensity(1.0f);
        box->SetFriction(0.3f);
        box->SetRestitution(0.5f);
    }
    //b2DistanceJointDef distance_joint_def;
    //distance_joint_def.Initialize(box1->rigid_body, box2->rigid_body, box1->rigid_body->GetWorldCenter(), box2->rigid_body->GetWorldCenter() + b2Vec2(0.0f, 0.0f));
    //b2DistanceJoint* distance_joint = (b2DistanceJoint*)world->CreateJoint(&distance_joint_def);

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
        process_keyboard_event(event);
        process_mouse_event(event);
    }
    process_keyboard();
    process_mouse();
}

void Application::process_keyboard_event(sf::Event event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::R: resetView(); break;
        }
    }
}

void Application::process_mouse_event(sf::Event event) {
    mousePos = sf::Mouse::getPosition(*window);
    mouseWorldPos = sf_screen_to_world(mousePos);
    b2Vec2 mouse_pos_world = b2Vec2(mouseWorldPos.x, mouseWorldPos.y);
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            b2Fixture* grabbed_fixture = get_fixture_at(mousePos);
            if (grabbed_fixture) {
                grabbed_body = grabbed_fixture->GetBody();
                grabbed_body_point_local = grabbed_body->GetLocalPoint(mouse_pos_world);
            }
        } else if (event.mouseButton.button == sf::Mouse::Right) {
            mousePrevPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        }
    }
    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            grabbed_body = nullptr;
        }
    }
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            zoomFactor *= pow(MOUSE_SCROLL_ZOOM, -event.mouseWheelScroll.delta);
        }
    }
}

void Application::process_keyboard() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window->close();
    }
}

void Application::process_mouse() {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (grabbed_body) {
            b2Vec2 mouse_pos_world = b2_screen_to_world(mousePos);
            b2Vec2 grabbed_body_point_world = grabbed_body->GetWorldPoint(grabbed_body_point_local);
            b2Vec2 vec = mouse_pos_world - grabbed_body_point_world;
            b2Vec2 force = MOUSE_FORCE_SCALE * vec;
            grabbed_body->ApplyForce(force, grabbed_body_point_world, true);
        }
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        sf::Vector2i mouseDelta = mousePrevPos - mousePos;
        viewCenterX += mouseDelta.x * zoomFactor;
        viewCenterY += -mouseDelta.y * zoomFactor;
    }
    mousePrevPos = mousePos;
}

void Application::process_world() {
    world->Step(timeStep, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
}

void Application::render() {
    window->clear();
    world_view.setCenter(viewCenterX, viewCenterY);
    world_view.setSize(window->getSize().x * zoomFactor, -1.0f * window->getSize().y * zoomFactor);
    window->setView(world_view);

    for (int i = 0; i < game_objects.size(); i++) {
        GameObject* object = game_objects[i].get();
        object->UpdateVisual();
        window->draw(*object->drawable.get());
    }

    if (grabbed_body) {
        b2Vec2 grabbed_body_point_world = grabbed_body->GetWorldPoint(grabbed_body_point_local);
        sf::Vector2f sf_grabbed_point = sf::Vector2f(grabbed_body_point_world.x, grabbed_body_point_world.y);
        line_primitive[0].position = sf_grabbed_point;
        line_primitive[0].color = sf::Color::Yellow;
        line_primitive[1].position = mouseWorldPos;
        line_primitive[1].color = sf::Color::Yellow;
        window->draw(line_primitive);
    }

    window->display();
}

b2Vec2 Application::b2_screen_to_world(sf::Vector2i screen_pos) {
    sf::Vector2f sfml_pos = window->mapPixelToCoords(screen_pos);
    b2Vec2 b2_pos = b2Vec2(sfml_pos.x, sfml_pos.y);
    return b2_pos;
}

sf::Vector2f Application::sf_screen_to_world(sf::Vector2i screen_pos) {
    sf::Vector2f sfml_pos = window->mapPixelToCoords(screen_pos);
    return sfml_pos;
}

sf::Vector2i Application::world_to_screen(b2Vec2 world_pos) {
    sf::Vector2f sfml_pos = sf::Vector2f(world_pos.x, world_pos.y);
    sf::Vector2i screen_pos = window->mapCoordsToPixel(sfml_pos);
    return screen_pos;
}

b2Fixture* Application::get_fixture_at(sf::Vector2i screen_pos) {
    b2Vec2 world_pos = b2_screen_to_world(screen_pos);
    b2Vec2 world_pos_next = b2_screen_to_world(screen_pos + sf::Vector2i(1, 1));
    b2Vec2 midpoint = 0.5f * (world_pos + world_pos_next);
    QueryCallback callback;
    b2AABB aabb;
    aabb.upperBound = world_pos;
    aabb.lowerBound = world_pos_next;
    world->QueryAABB(&callback, aabb);
    for (int i = 0; i < callback.fixtures.size(); i++) {
        b2Fixture* fixture = callback.fixtures[i];
        if (fixture->TestPoint(midpoint)) {
            return fixture;
        }
    }
    return nullptr;
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
        shape->setPoint(i, sf::Vector2f(vertices[1].x, vertices[1].y));
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
        drawable_vertices[i].position = sf::Vector2f(vertices[i].x, vertices[i].y);
    }
    std::unique_ptr<LineStripShape> line_strip = std::make_unique<LineStripShape>(drawable_vertices);
    line_strip->setLineColor(color);
    std::unique_ptr<GameObject> uptr = std::make_unique<GameObject>(std::move(line_strip), body);
    GameObject* ptr = uptr.get();
    game_objects.push_back(std::move(uptr));
    return ptr;
}

bool QueryCallback::ReportFixture(b2Fixture* fixture) {
    fixtures.push_back(fixture);
    return true;
}
