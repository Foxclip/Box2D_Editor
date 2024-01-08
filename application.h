#pragma once

#include "gameobject.h"

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
	sf::View view1;
	sf::Vector2i mousePrevPos;
	float zoomFactor = 0.3f;
	float viewCenterX = 0, viewCenterY = -30.0f;

	const int32 VELOCITY_ITERATIONS = 6;
	const int32 POSITION_ITERATIONS = 2;
	std::unique_ptr<b2World> world;
	float timeStep = 1.0f / FPS;

	std::vector<std::unique_ptr<GameObject>> game_objects;

	void init_objects();
	void main_loop();
	void resetView();
	void process_input();
	void process_world();
	void render();

};