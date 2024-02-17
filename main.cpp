#include <cstdio>
#include <iostream>
#include "application.h"
#include "logger.h"

void create_objects(Application& app) {
    //std::vector<b2Vec2> ground_vertices = {
    //    b2Vec2(25.0f, 8.0f),
    //    b2Vec2(15.0f, 2.0f),
    //    b2Vec2(5.0f, 0.0f),
    //    b2Vec2(-5.0f, 0.0f),
    //    b2Vec2(-15.0f, 2.0f),
    //    b2Vec2(-25.0f, 8.0f),
    //};
    //GameObject* ground = app.create_ground(b2Vec2(0.0f, 0.0f), ground_vertices, sf::Color(255, 255, 255));

    //GameObject* box0 = app.create_box(b2Vec2(0.0f, 1.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color(0, 255, 0));
    //GameObject* box1 = app.create_box(b2Vec2(0.1f, 2.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color(0, 255, 0));
    //GameObject* box2 = app.create_box(b2Vec2(0.2f, 3.0f), utils::to_radians(0.0f), b2Vec2(1.0f, 1.0f), sf::Color(0, 255, 0));
    //GameObject* ball = app.create_ball(b2Vec2(0.0f, 5.0f), 0.5f, sf::Color(0, 255, 0), sf::Color(0, 64, 0));
    //std::vector<GameObject*> dynamic_objects = { box0, box1, box2, ball };
    //for (size_t i = 0; i < dynamic_objects.size(); i++) {
    //    GameObject* box = dynamic_objects[i];
    //    box->setType(b2_dynamicBody, false);
    //    box->setDensity(1.0f, false);
    //    box->setFriction(0.3f, false);
    //    box->setRestitution(0.5f, false);
    //}

    //std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
    //std::vector<float> wheels = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    //GameObject* car = app.create_car(b2Vec2(0.0f, 10.0f), lengths, wheels, sf::Color(255, 0, 0));
    //car->setType(b2_dynamicBody, false);
    //car->setDensity(1.0f, false);
    //car->setFriction(0.3f, false);
    //car->setRestitution(0.5f, false);

    GameObject* box0 = app.create_box(b2Vec2(0.0f, 0.0f), utils::to_radians(45.0f), b2Vec2(1.0f, 1.0f), sf::Color(0, 255, 0));
    app.setCameraPos(0.0f, 0.0f);
    app.setCameraZoom(200.0f);
}

void execute_app() {
    Application app;
    app.init("level.txt");
    //app.init();
    //create_objects(app);
    app.start();
}

int main() {
    
    try {
        execute_app();
    } catch (std::string msg) {
        logger << "ERROR: " << msg << "\n";
    } catch (std::exception exc) {
        logger << "ERROR: " << exc.what() << "\n";
    }

    // TODO: selecting ground shape with rectangle select
    // TODO: delete all fixtures in Polygon::recut method
    // TODO: rename tokenizer to serializer
    // TODO: editing shapes
    // TODO: cut polygons with b2maxvertices
    // TODO: select tool rectangle select
    // TODO: set whether object is static or dynamic in edit mode
    // TODO: evolving cars
    // TODO: rename project to EvolvingCars
    // TODO: edit and simulate modes

    return 0;
}
