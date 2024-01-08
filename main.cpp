#include "box2d/box2d.h"
#include <cstdio>
#include <iostream>
#include "application.h"

int main() {
    
    try {
        Application app;
        app.init();
        app.start();
    } catch (std::string msg) {
        std::cout << "ERROR: " << msg << "\n";
    } catch (std::exception exc) {
        std::cout << "ERROR: " << exc.what() << "\n";
    }

    // draggable and zoomable view

    return 0;
}

//int main() {
//	b2Vec2 gravity(0.0f, -10.0f);
//	b2World world(gravity);
//
//	b2BodyDef groundBodyDef;
//	groundBodyDef.position.Set(0.0f, -10.0f);
//	b2Body* groundBody = world.CreateBody(&groundBodyDef);
//	b2PolygonShape groundBox;
//	groundBox.SetAsBox(50.0f, 10.0f);
//	groundBody->CreateFixture(&groundBox, 0.0f);
//
//	b2BodyDef bodyDef;
//	bodyDef.type = b2_dynamicBody;
//	bodyDef.position.Set(0.0f, 8.0f);
//	b2Body* body = world.CreateBody(&bodyDef);
//
//	b2PolygonShape dynamicBox;
//	dynamicBox.SetAsBox(1.0f, 1.0f);
//	b2FixtureDef fixtureDef;
//	fixtureDef.shape = &dynamicBox;
//	fixtureDef.density = 1.0f;
//	fixtureDef.friction = 0.3f;
//	body->CreateFixture(&fixtureDef);
//
//	float timeStep = 1.0f / 60.0f;
//	int32 velocityIterations = 6;
//	int32 positionIterations = 2;
//	b2Vec2 position = body->GetPosition();
//	float angle = body->GetAngle();
//
//	for (int32 i = 0; i < 200; ++i) {
//		world.Step(timeStep, velocityIterations, positionIterations);
//		position = body->GetPosition();
//		angle = body->GetAngle();
//		printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
//	}
//
//}
