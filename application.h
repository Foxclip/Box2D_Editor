#pragma once

#include "gameobject.h" // keep above windows imports
#include <Windows.h>
#include <winuser.h>
#include <functional>
#include <set>
#include "logger.h"

Logger& operator<<(Logger& lg, const b2Vec2& value);

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int ANTIALIASING = 0;
const float MOUSE_SCROLL_ZOOM = 1.2f;
const int FPS = 60;
const int TOOL_RECT_WIDTH = 60;
const int TOOL_RECT_HEIGHT = 40;
const int TOOLBOX_PADDING = 10;

class QueryCallback : public b2QueryCallback {
public:
	std::vector<b2Fixture*> fixtures;
	bool ReportFixture(b2Fixture* fixture);
};

class Tool {
public:
	std::string name;
	sf::RectangleShape shape;

	Tool();

private:
};

class DragTool : public Tool {
public:
	b2Body* mouse_body = nullptr;
	b2MouseJoint* mouse_joint = nullptr;

	DragTool();

private:
};

class MoveTool : public Tool {
public:
	GameObject* object = nullptr;
	bool object_was_enabled = false;
	b2Vec2 offset = b2Vec2();

	MoveTool();

private:
};

class RotateTool : public Tool {
public:
	GameObject* object = nullptr;
	bool object_was_enabled = false;
	float angle_offset = 0.0f;

	RotateTool();

private:
};

class EditTool : public Tool {
public:
	static const int VERTEX_SIZE = 9;
	static const int VERTEX_HIGHLIGHT_DISTANCE = 10;
	static const int EDGE_HIGHLIGHT_DISTANCE = 10;
	static const int NORMAL_LENGTH = 30;
	enum EditToolMode {
		HOVER,
		SELECT,
		ADD,
		INSERT,
		MOVE,
	};
	int grabbed_vertex = -1;
	int highlighted_vertex = -1;
	int highlighted_edge = -1;
	int edge_vertex = -1;
	bool selection = false;
	sf::Vector2f select_origin;
	b2Vec2 grabbed_vertex_offset;
	b2Vec2 insertVertexPos;
	sf::RectangleShape vertex_highlight_rect;
	sf::RectangleShape vertex_rect;
	sf::RectangleShape edge_highlight;
	sf::RectangleShape select_rect;
	EditToolMode mode = HOVER;

	EditTool();
	static std::string modeToStr(EditToolMode mode);

};

class HistoryEntry {
public:
	enum Type {
		BASE,
		NORMAL,
		QUICKLOAD,
	};
	std::string str;
	Type type;
	static std::string typeToStr(Type type);

	HistoryEntry(std::string str, Type type);
};

class History {
public:
	History();
	History(std::function<std::string(void)> get, std::function<void(std::string)> set);
	void save(HistoryEntry::Type type);
	void undo();
	void redo();
	void clear();
	HistoryEntry& getCurrent();

private:
	std::function<std::string(void)> get;
	std::function<void(std::string)> set;
	std::vector<HistoryEntry> history;
	int current = 0;
};

class Application {

public:
	void init();
	void start();

private:
	std::unique_ptr<sf::RenderWindow> window;
	sf::View world_view;
	sf::View ui_view;
	sf::Vector2i mousePos;
	sf::Vector2f mousePosf;
	sf::Vector2f sfMousePosWorld;
	sf::Vector2i mousePrevPos;
	float zoomFactor = 30.0f;
	float viewCenterX = 0.0f, viewCenterY = 5.0f;
	sf::VertexArray line_primitive = sf::VertexArray(sf::Lines, 2);
	sf::RectangleShape paused_rect;
	sf::RectangleShape toolbox_rect;
	sf::Font ui_font;
	sf::Text paused_text;
	sf::Text tool_text;
	DragTool drag_tool;
	MoveTool move_tool;
	RotateTool rotate_tool;
	EditTool edit_tool;
	std::vector<Tool*> tools;
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