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
#include "objectlist.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int ANTIALIASING = 0;
const float MOUSE_SCROLL_ZOOM = 1.2f;
const int FPS = 60;
const float WORLD_SATURATION = 0.75f;
const float WORLD_COLOR_SCALE_CENTER = 0.25f;
const float WORLD_COLOR_SCALE_PERCENT = 0.6f;
const sf::Glsl::Vec3 SELECTION_OUTLINE_COLOR = sf::Glsl::Vec3(1.0f, 1.0f, 0.0f);
const sf::Glsl::Vec3 HOVER_OUTLINE_COLOR = sf::Glsl::Vec3(1.0f, 1.0f, 0.0f);
const int SELECTION_OUTLINE_THICKNESS = 3;
const int HOVER_OUTLINE_THICKNESS = 1;
const int MOUSE_DRAG_THRESHOLD = 10;

Logger& operator<<(Logger& lg, const b2Vec2& value);
Logger& operator<<(Logger& lg, const sf::Vector2f& value);

class QueryCallback : public b2QueryCallback {
public:
	std::vector<b2Fixture*> fixtures;
	bool ReportFixture(b2Fixture* fixture);
};

class FpsCounter {
public:
	void init();
	void frameBegin();
	int frameEnd();
	int getFps();

private:
	std::chrono::steady_clock::time_point t1;
	std::chrono::steady_clock::time_point t2;
	std::vector<double> frame_times;
	std::chrono::steady_clock::time_point last_fps_time;
	int fps = 0;
};

class Application {

public:
	void init();
	void start();
	void load(const std::string& filename);
	void setCameraPos(float x, float y);
	void setCameraZoom(float zoom);
	GameObjectList& getObjectList();
	void selectSingleObject(GameObject* object, bool with_children = false);
	BoxObject* create_box(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const b2Vec2& size,
		const sf::Color& color
	);
	BallObject* create_ball(
		const std::string& name,
		const b2Vec2& pos,
		float radius,
		const sf::Color& color,
		const sf::Color& notch_color = sf::Color::Transparent
	);
	PolygonObject* create_polygon(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const std::vector<b2Vec2>& vertices,
		const sf::Color& color
	);
	PolygonObject* create_car(
		const std::string& name,
		const b2Vec2& pos,
		const std::vector<float>& lengths,
		const std::vector<float>& wheels,
		const sf::Color& color
	);
	ChainObject* create_chain(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const std::vector<b2Vec2>& vertices,
		const sf::Color& color
	);

private:
	sf::Shader desat_shader;
	sf::Shader selection_shader;
	sf::Vector2i mousePos;
	sf::Vector2f mousePosf;
	sf::Vector2f sfMousePosWorld;
	sf::Vector2i mousePrevPos;
	sf::Vector2f mousePressPosf;
	bool leftButtonPressed = false;
	bool rightButtonPressed = false;
	bool leftButtonPressGesture = false;
	bool leftButtonProcessWidgetsOnPress = true;
	bool leftButtonProcessWidgetsOnRelease = true;
	float zoomFactor = 30.0f;
	float viewCenterX = 0.0f, viewCenterY = 5.0f;
	sf::Font ui_font;
	ContainerWidget* paused_rect_widget;
	ContainerWidget* toolbox_widget;
	Tool* selected_tool = nullptr;
	std::vector<Tool*> tools_in_tool_panel;
	sf::Font fps_font;
	FpsCounter fps_counter;
	ContainerWidget* fps_widget;
	TextWidget* fps_text_widget;
	RectangleWidget* logger_widget;
	TextWidget* logger_text_widget;
	sf::Font console_font;
	sf::CircleShape origin_shape;
	sf::Font small_font;
	sf::Text name_text;
	sf::Text id_text;

	const int32 VELOCITY_ITERATIONS = 6;
	const int32 POSITION_ITERATIONS = 2;
	const float MOUSE_FORCE_SCALE = 50.0f;
	std::unique_ptr<b2World> world;
	float timeStep = 1.0f / FPS;
	bool paused = true;
	bool render_object_info = true;
	b2Vec2 b2MousePosWorld;
	GameObject* active_object = nullptr;
	GameObjectList game_objects;

	History history;
	bool commit_action = false;
	struct LoadRequest {
		bool requested = false;
		std::string filename;
	};
	LoadRequest load_request;
	std::string quicksave_str;
	bool quickload_requested = false;

	void init_tools();
	void init_world();
	void init_ui();
	void init_widgets();
	void main_loop();
	void resetView();
	void process_widgets();
	void process_input();
	void process_keyboard_event(sf::Event event);
	void process_mouse_event(sf::Event event);
	void process_keyboard();
	void process_mouse();
	void process_left_click();
	void process_left_release();
	void process_world();
	void render();
	void render_world();
	void render_ui();
	void maximize_window();
	std::string serialize();
	void deserialize(std::string str, bool set_camera);
	void save_to_file(std::string filename);
	void load_action(std::string filename);
	void load_from_file(std::string filename);
	void quicksave();
	void quickload();
	Tool* try_select_tool_by_index(size_t index);
	Tool* try_select_tool(Tool* tool);
	void select_create_type(int type);
	void toggle_pause();
	void load_font(sf::Font& font, const std::string& filename);
	sf::Vector2f screen_to_world(const sf::Vector2f& screen_pos);
	sf::Vector2f pixel_to_world(const sf::Vector2i& screen_pos);
	sf::Vector2f world_to_screen(const sf::Vector2f& world_pos);
	sf::Vector2f world_to_screen(const b2Vec2& world_pos);
	sf::Vector2i world_to_pixel(const sf::Vector2f& world_pos);
	sf::Vector2i world_to_pixel(const b2Vec2& world_pos);
	sf::Vector2f world_dir_to_screenf(const b2Vec2& world_dir);
	ptrdiff_t mouse_get_chain_edge(const b2Fixture* fixture);
	b2Fixture* get_fixture_at(sf::Vector2f screen_pos);
	GameObject* get_object_at(sf::Vector2f screen_pos);
	ptrdiff_t mouse_get_object_vertex();
	ptrdiff_t mouse_get_object_edge();
	ptrdiff_t mouse_get_edge_vertex();
	void select_vertices_in_rect(const RectangleSelect& rectangle_select);
	void select_objects_in_rect(const RectangleSelect& rectangle_select);
	void render_rectangle_select(sf::RenderTarget& target, RectangleSelect& rectangle_select);
	void get_screen_normal(const b2Vec2& v1, const b2Vec2& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2);
	void get_screen_normal(const sf::Vector2i& v1, const sf::Vector2i& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2);
	void draw_line(sf::RenderTarget& target, const sf::Vector2f& v1, const sf::Vector2f& v2, const sf::Color& color);
	bool is_parent_selected(GameObject* object);
	void grab_selected();
	void rotate_selected();
	void delete_object(GameObject* object, bool remove_children);

};