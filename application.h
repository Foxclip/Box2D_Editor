#pragma once

#include <SFML/Graphics.hpp>

class Application {

public:
	void init();
	void start();

private:

	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const int ANTIALIASING = 0;
	std::unique_ptr<sf::RenderWindow> window;
	sf::CircleShape shape;
	sf::View view1;
	float zoomFactor = 1.0f;
	float viewCenterX = 0, viewCenterY = 0;

void main_loop();
void resetView();
void process_input();
void process_world();
void render();

};