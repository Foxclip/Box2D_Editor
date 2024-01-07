#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include <stdio.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int ANTIALIASING = 0;
std::unique_ptr<sf::RenderWindow> window;
sf::CircleShape shape(10.0f);
sf::View view1(sf::FloatRect(0.0f, 0.0f, 800.0f, 600.0f));
float zoomFactor = 1.0f;
float viewCenterX = 0, viewCenterY = 0;

void resetView() {
    viewCenterX = 0.0f;
    viewCenterY = 0.0f;
    zoomFactor = 1.0f;
}

void init() {
    sf::ContextSettings cs;
    cs.antialiasingLevel = ANTIALIASING;
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML", sf::Style::Default, cs);
    window->setVerticalSyncEnabled(true);
    shape.setFillColor(sf::Color::Green);
    resetView();
}

void process_input() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
        }
    }
}

void process_world() {

}

void render() {
    window->clear();
    view1.setCenter(viewCenterX, viewCenterY);
    view1.setSize(window->getSize().x * zoomFactor, window->getSize().y * zoomFactor);
    window->setView(view1);

    window->draw(shape);

    window->display();
}

int main() {
    
    init();

    while (window->isOpen()) {
        process_input();
        process_world();
        render();
    }

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
