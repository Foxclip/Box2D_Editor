#pragma once

#include "gameobject.h"
#include <Windows.h>
#include <winuser.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int ANTIALIASING = 0;
const float MOUSE_SCROLL_ZOOM = 1.2f;
const int FPS = 60;
const int VERTEX_SIZE = 9;
const int VERTEX_EDITOR_DISTANCE = 10;
const int TOOL_RECT_SIZE = 40;
const int TOOLBOX_PADDING = 10;

class QueryCallback : public b2QueryCallback {
public:
	std::vector<b2Fixture*> fixtures;
	bool ReportFixture(b2Fixture* fixture);
};

class Tool {
public:
	Tool(std::string name);
	std::string name;
	sf::RectangleShape shape;
private:
};

extern std::vector<std::unique_ptr<Tool>> tools;

class Application {

public:
	void init();
	void start();

private:
	std::unique_ptr<sf::RenderWindow> window;
	sf::View world_view;
	sf::View ui_view;
	sf::Vector2i mousePos;
	sf::Vector2f sfMousePosWorld;
	sf::Vector2i mousePrevPos;
	float zoomFactor = 0.03f;
	float viewCenterX = 0, viewCenterY = 5.0f;
	sf::VertexArray line_primitive = sf::VertexArray(sf::Lines, 2);
	sf::RectangleShape vertex_rect;
	sf::RectangleShape vertex_editor_rect;
	sf::RectangleShape paused_rect;
	sf::RectangleShape toolbox_rect;
	sf::Font ui_font;
	sf::Text paused_text;
	sf::Text tool_text;
	bool vertex_editor_mode = false;
	int grabbed_vertex = -1;
	Tool* selected_tool = nullptr;

	const int32 VELOCITY_ITERATIONS = 6;
	const int32 POSITION_ITERATIONS = 2;
	const float MOUSE_FORCE_SCALE = 50.0f;
	std::unique_ptr<b2World> world;
	float timeStep = 1.0f / FPS;
	b2Body* mouse_body = nullptr;
	b2MouseJoint* mouse_joint = nullptr;
	bool paused = true;
	GroundObject* ground = nullptr;
	b2Vec2 b2MousePosWorld;

	std::vector<std::unique_ptr<GameObject>> game_objects;

	void init_ui();
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
	void render_world();
	void render_ui();
	void maximize_window();
	Tool* create_tool(std::string name);
	b2Vec2 b2_screen_to_world(sf::Vector2i screen_pos);
	sf::Vector2f sf_screen_to_world(sf::Vector2i screen_pos);
	sf::Vector2i world_to_screen(b2Vec2 world_pos);
	sf::Vector2f world_to_screenf(b2Vec2 world_pos);
	b2Fixture* get_fixture_at(sf::Vector2i screen_pos);
	bool mouse_get_ground_vertex(int& index, sf::Vector2i& position);

	GameObject* create_box(b2Vec2 pos, float angle, b2Vec2 size, sf::Color color);
	GameObject* create_ball(b2Vec2 pos, float radius, sf::Color color, sf::Color notch_color = sf::Color::Transparent);
	CarObject* create_car(b2Vec2 pos, std::vector<float> lengths, std::vector<float> wheels, sf::Color color);
	GroundObject* create_ground(b2Vec2 pos, std::vector<b2Vec2> vertices, sf::Color color);

};