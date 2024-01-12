#pragma once

#include "gameobject.h"

class QueryCallback : public b2QueryCallback {
public:
	std::vector<b2Fixture*> fixtures;
	bool ReportFixture(b2Fixture* fixture);
};

class Application {

public:
	void init();
	void start();

private:

	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const int ANTIALIASING = 0;
	const float MOUSE_SCROLL_ZOOM = 1.2f;
	const int FPS = 60;
	std::unique_ptr<sf::RenderWindow> window;
	sf::View world_view;
	sf::Vector2i mousePos;
	sf::Vector2i mousePrevPos;
	float zoomFactor = 0.03f;
	float viewCenterX = 0, viewCenterY = 5.0f;

	const int32 VELOCITY_ITERATIONS = 6;
	const int32 POSITION_ITERATIONS = 2;
	const float MOUSE_FORCE_SCALE = 50.0f;
	std::unique_ptr<b2World> world;
	float timeStep = 1.0f / FPS;
	b2Body* grabbed_body = nullptr;
	b2Vec2 grabbed_body_point_local;

	std::vector<std::unique_ptr<GameObject>> game_objects;

	void init_objects();
	void main_loop();
	void resetView();
	void process_input();
	void process_keyboard_event(sf::Event event);
	void process_mouse_event(sf::Event event);
	void process_keyboard();
	void process_mouse();
	void process_world();
	void render();
	b2Vec2 screen_to_world(sf::Vector2i screen_pos);
	b2Fixture* get_fixture_at(sf::Vector2i screen_pos);

	GameObject* create_box(b2Vec2 pos, float angle, b2Vec2 size, sf::Color color);
	GameObject* create_car(b2Vec2 pos, std::vector<float> lengths, sf::Color color);
	GameObject* create_ground(b2Vec2 pos, std::vector<b2Vec2> vertices, sf::Color color);

};