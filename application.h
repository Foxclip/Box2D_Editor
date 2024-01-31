#pragma once

#include "gameobject.h" // keep above windows imports
#include <Windows.h>
#include <winuser.h>
#include <functional>
#include <set>
#include "logger.h"
#include "tools.h"
#include "history.h"
#include "widget.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int ANTIALIASING = 0;
const float MOUSE_SCROLL_ZOOM = 1.2f;
const int FPS = 60;

Logger& operator<<(Logger& lg, const b2Vec2& value);
Logger& operator<<(Logger& lg, const sf::Vector2f& value);

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
	sf::View world_view;
	sf::View ui_view;
	sf::Vector2i mousePos;
	sf::Vector2f mousePosf;
	sf::Vector2f sfMousePosWorld;
	sf::Vector2i mousePrevPos;
	float zoomFactor = 30.0f;
	float viewCenterX = 0.0f, viewCenterY = 5.0f;
	sf::VertexArray line_primitive = sf::VertexArray(sf::Lines, 2);
	sf::Font ui_font;
	ContainerWidget* paused_rect_widget;
	ContainerWidget* toolbox_widget;
	ContainerWidget* create_panel_widget;
	ContainerWidget* edit_window_widget;
	Tool* selected_tool = nullptr;

	const int32 VELOCITY_ITERATIONS = 6;
	const int32 POSITION_ITERATIONS = 2;
	const float MOUSE_FORCE_SCALE = 50.0f;
	std::unique_ptr<b2World> world;
	float timeStep = 1.0f / FPS;
	bool paused = true;
	GroundObject* ground = nullptr;
	b2Vec2 b2MousePosWorld;

	std::vector<std::unique_ptr<GameObject>> game_objects;
	History history;
	bool commit_action = false;
	std::string quicksave_str;
	bool quickload_requested = false;

	void init_tools();
	void init_world();
	void init_ui();
	void init_objects();
	void main_loop();
	void resetView();
	void process_widgets();
	void process_input();
	void process_keyboard_event(sf::Event event);
	void process_mouse_event(sf::Event event);
	void process_keyboard();
	void process_mouse();
	void process_left_click();
	void process_world();
	void render();
	void render_world();
	void render_ui();
	void maximize_window();
	std::string serialize();
	void deserialize(std::string str, bool set_camera);
	void save(std::string filename);
	void load(std::string filename);
	void quicksave();
	void quickload();
	Tool* try_select_tool(int index);
	void select_create_type(int type);
	void toggle_pause();
	b2Vec2 b2_screen_to_world(sf::Vector2i screen_pos);
	sf::Vector2f sf_screen_to_world(sf::Vector2i screen_pos);
	sf::Vector2i world_to_screen(b2Vec2 world_pos);
	sf::Vector2f world_to_screenf(sf::Vector2f world_pos);
	sf::Vector2f world_to_screenf(b2Vec2 world_pos);
	sf::Vector2f world_dir_to_screenf(b2Vec2 world_dir);
	b2Fixture* get_fixture_at(sf::Vector2i screen_pos);
	int mouse_get_ground_vertex();
	int mouse_get_ground_edge();
	int mouse_get_edge_vertex();
	void select_vertices_in_rect();
	void get_screen_normal(const b2Vec2& v1, const b2Vec2& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2);
	void get_screen_normal(const sf::Vector2i& v1, const sf::Vector2i& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2);
	void draw_line(const sf::Vector2f& v1, const sf::Vector2f& v2, const sf::Color& color);

	BoxObject* create_box(b2Vec2 pos, float angle, b2Vec2 size, sf::Color color);
	BallObject* create_ball(b2Vec2 pos, float radius, sf::Color color, sf::Color notch_color = sf::Color::Transparent);
	CarObject* create_car(b2Vec2 pos, std::vector<float> lengths, std::vector<float> wheels, sf::Color color);
	GroundObject* create_ground(b2Vec2 pos, std::vector<b2Vec2> vertices, sf::Color color);

};