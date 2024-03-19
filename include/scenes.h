#pragma once

#include "application.h"

void scene1(Application& app) {
    std::vector<b2Vec2> ground_vertices = {
        b2Vec2(25.0f, 8.0f),
        b2Vec2(15.0f, 2.0f),
        b2Vec2(5.0f, 0.0f),
        b2Vec2(-5.0f, 0.0f),
        b2Vec2(-15.0f, 2.0f),
        b2Vec2(-25.0f, 8.0f),
    };
    GameObject* ground = app.create_chain("ground", b2Vec2(0.0f, 0.0f), 0.0f, ground_vertices, sf::Color(255, 255, 255));

    GameObject* box0 = app.create_box("box0", b2Vec2(0.0f, 1.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color(0, 255, 0));
    GameObject* box1 = app.create_box("box1", b2Vec2(0.1f, 2.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color(0, 255, 0));
    GameObject* box2 = app.create_box("box2", b2Vec2(0.2f, 3.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color(0, 255, 0));
    GameObject* ball = app.create_ball("ball0", b2Vec2(0.0f, 5.0f), 0.5f, sf::Color(0, 255, 0), sf::Color(0, 64, 0));
    CompoundVector<GameObject*> dynamic_objects = { box0, box1, box2, ball };
    for (size_t i = 0; i < dynamic_objects.size(); i++) {
        GameObject* box = dynamic_objects[i];
        box->setType(b2_dynamicBody, false);
        box->setDensity(1.0f, false);
        box->setFriction(0.3f, false);
        box->setRestitution(0.5f, false);
    }

    std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
    std::vector<float> wheels = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    GameObject* car = app.create_car("car0", b2Vec2(0.0f, 10.0f), lengths, wheels, sf::Color(255, 0, 0));
    car->setType(b2_dynamicBody, false);
    car->setDensity(1.0f, false);
    car->setFriction(0.3f, false);
    car->setRestitution(0.5f, false);

    app.setCameraPos(0.0f, 5.0f);
    app.setCameraZoom(30.0f);
}

void single_box(Application& app) {
    GameObject* box0 = app.create_box(
        "box0",
        b2Vec2(0.0f, 0.0f),
        utils::to_radians(0.0f),
        b2Vec2(1.0f, 1.0f),
        sf::Color(0, 255, 0)
    );
    app.setCameraPos(0.0f, 0.0f);
    app.setCameraZoom(200.0f);
}

void single_ball(Application& app) {
    GameObject* ball = app.create_ball(
        "ball0",
        b2Vec2(0.0f, 0.0f),
        0.5f,
        sf::Color(0, 255, 0),
        sf::Color(0, 64, 0)
    );
    app.setCameraPos(0.0f, 0.0f);
    app.setCameraZoom(200.0f);
}

void ground_transform(Application& app) {
    std::vector<b2Vec2> ground_vertices = {
        b2Vec2(25.0f, 8.0f),
        b2Vec2(15.0f, 2.0f),
        b2Vec2(5.0f, 0.0f),
        b2Vec2(-5.0f, 0.0f),
        b2Vec2(-15.0f, 2.0f),
        b2Vec2(-25.0f, 8.0f),
    };
    GameObject* ground = app.create_chain(
        "ground",
        b2Vec2(0.0f, 5.0f),
        utils::to_radians(45.0f),
        ground_vertices,
        sf::Color(255, 255, 255)
    );
    app.setCameraPos(0.0f, 5.0f);
    app.setCameraZoom(30.0f);
}

void single_car(Application& app) {
    std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
    std::vector<float> wheels = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    GameObject* car = app.create_car("car0", b2Vec2(0.0f, 0.0f), lengths, wheels, sf::Color(255, 0, 0));
    car->setType(b2_dynamicBody, false);
    car->setDensity(1.0f, false);
    car->setFriction(0.3f, false);
    car->setRestitution(0.5f, false);
    app.setCameraPos(0.0f, 0.0f);
    app.setCameraZoom(50.0f);
}

void multiple_chains(Application& app) {
    std::vector<b2Vec2> ground_vertices = {
        b2Vec2(25.0f, 8.0f),
        b2Vec2(15.0f, 2.0f),
        b2Vec2(5.0f, 0.0f),
        b2Vec2(-5.0f, 0.0f),
        b2Vec2(-15.0f, 2.0f),
        b2Vec2(-25.0f, 8.0f),
    };
    GameObject* chain1 = app.create_chain(
        "chain0",
        b2Vec2(0.0f, 0.0f),
        utils::to_radians(0.0f),
        ground_vertices,
        sf::Color(255, 255, 255)
    );
    GameObject* chain2 = app.create_chain(
        "chain1",
        b2Vec2(0.0f, 5.0f),
        utils::to_radians(0.0f),
        ground_vertices,
        sf::Color(255, 255, 255)
    );
    app.setCameraPos(0.0f, 5.0f);
    app.setCameraZoom(30.0f);
}

void polygon(Application& app) {
    std::vector<float> lengths = { 5.0f, 5.0f, 5.0f, 5.0f };
    std::vector<float> wheels = { 0.0f, 0.0f, 0.0f, 0.0f };
    GameObject* car = app.create_car("polygon0", b2Vec2(0.0f, 0.0f), lengths, wheels, sf::Color(255, 0, 0));
    car->setType(b2_dynamicBody, false);
    car->setDensity(1.0f, false);
    car->setFriction(0.3f, false);
    car->setRestitution(0.5f, false);
    app.setCameraPos(0.0f, 0.0f);
    app.setCameraZoom(50.0f);
}

void box_parent(Application& app) {
    GameObject* parent = app.create_box(
        "parent",
        b2Vec2(0.0f, 0.0f),
        utils::to_radians(0.0f),
        b2Vec2(1.0f, 1.0f),
        sf::Color(0, 255, 0)
    );
    GameObject* child = app.create_box(
        "child",
        b2Vec2(2.0f, 0.0f),
        utils::to_radians(0.0f),
        b2Vec2(1.0f, 1.0f),
        sf::Color(0, 255, 0)
    );
    GameObject* another_child = app.create_box(
        "another child",
        b2Vec2(4.0f, 0.0f),
        utils::to_radians(0.0f),
        b2Vec2(1.0f, 1.0f),
        sf::Color(0, 255, 0)
    );
    GameObjectList& objects = app.getObjectList();
    child->setParent(parent);
    another_child->setParent(child);
    parent->setGlobalPosition(b2Vec2(1.0f, 0.0f));
    app.setCameraPos(0.0f, 0.0f);
    app.setCameraZoom(200.0f);
}

void convex_polygon(Application& app) {
    std::vector<b2Vec2> vertices;
    const size_t VERTEX_COUNT = 50;
    for (size_t i = 0; i < VERTEX_COUNT; i++) {
        b2Vec2 pos = utils::get_circle_vertex<b2Vec2>(i, VERTEX_COUNT, 1.0f);
        vertices.push_back(pos);
    }
    PolygonObject* polygon = app.create_polygon("polygon", b2Vec2(0.0f, 0.0f), 0.0f, vertices, sf::Color(255, 0, 0));
    polygon->setType(b2_dynamicBody, false);
    polygon->setDensity(1.0f, false);
    polygon->setFriction(0.3f, false);
    polygon->setRestitution(0.5f, false);
    polygon->draw_indices = true;
    app.setCameraPos(0.0f, 0.0f);
    app.setCameraZoom(50.0f);
}

void duplication(Application& app) {
    app.load("level.txt");
    GameObjectList& objects = app.getObjectList();
    GameObject* car = objects.getByName("car0");
    logger << "Car pos: " << car->getPosition() << "\n";
    GameObject* car_copy = objects.duplicate(car, true);
    car_copy->setGlobalPosition(car->getGlobalPosition() + b2Vec2(0.0f, 2.0f));
    logger << "Car copy pos: " << car_copy->getPosition() << "\n";
}

void parent_loop(Application& app) {
    GameObject* box0 = app.create_box(
        "box0",
        b2Vec2(0.0f, 0.0f),
        utils::to_radians(0.0f),
        b2Vec2(1.0f, 1.0f),
        sf::Color(0, 255, 0)
    );
    GameObject* box1 = app.create_box(
        "box1",
        b2Vec2(2.0f, 0.0f),
        utils::to_radians(0.0f),
        b2Vec2(1.0f, 1.0f),
        sf::Color(0, 255, 0)
    );
    GameObjectList& game_objects = app.getObjectList();
    box0->setParent(box1);
    box1->setParent(box0);
}