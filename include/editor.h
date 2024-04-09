#pragma once

#include <functional>
#include <set>
#include "logger.h"
#include "tools.h"
#include "history.h"
#include "widgets/widgets.h"
#include "simulation/simulation.h"

const sf::String WINDOW_TITLE = "Box2D Editor";
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
	int getFps() const;

private:
	std::chrono::steady_clock::time_point t1;
	std::chrono::steady_clock::time_point t2;
	std::vector<double> frame_times;
	std::chrono::steady_clock::time_point last_fps_time;
	int fps = 0;
};

class Editor : public fw::Application {
public:
	void init();
	void load(const std::string& filename);
	void setCameraPos(float x, float y);
	void setCameraZoom(float zoom);
	fw::WidgetList& getWidgetList();
	void selectSingleObject(GameObject* object, bool with_children = false);
	Simulation& getSimulation();
	BoxObject* createBox(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const b2Vec2& size,
		const sf::Color& color
	);
	BallObject* createBall(
		const std::string& name,
		const b2Vec2& pos,
		float radius,
		const sf::Color& color,
		const sf::Color& notch_color = sf::Color::Transparent
	);
	PolygonObject* createPolygon(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const std::vector<b2Vec2>& vertices,
		const sf::Color& color
	);
	PolygonObject* createCar(
		const std::string& name,
		const b2Vec2& pos,
		const std::vector<float>& lengths,
		const std::vector<float>& wheels,
		const sf::Color& color
	);
	ChainObject* createChain(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const std::vector<b2Vec2>& vertices,
		const sf::Color& color
	);

private:
	friend class EditWindow;
	friend class EditWindowParameter;
	friend class TextParameter;
	friend class BoolParameter;
	friend class FloatParameter;
	friend class Toolbox;
	friend class CreatePanel;
	sf::View world_view;
	sf::View ui_view;
	sf::RenderTexture world_texture;
	sf::RenderTexture ui_texture;
	sf::RenderTexture selection_mask;
	sf::Shader desat_shader;
	sf::Shader selection_shader;
	sf::Vector2f sfMousePosWorld;
	float zoomFactor = 30.0f;
	float viewCenterX = 0.0f, viewCenterY = 5.0f;
	SelectTool select_tool;
	CreateTool create_tool;
	DragTool drag_tool;
	MoveTool move_tool;
	RotateTool rotate_tool;
	EditTool edit_tool;
	std::vector<Tool*> tools = {
		&select_tool,
		&drag_tool,
		&move_tool,
		&rotate_tool,
		&edit_tool,
		&create_tool,
	};
	sf::Font ui_font;
	sf::Font fps_font;
	sf::Font console_font;
	sf::Font small_font;
	sf::Font textbox_font;
	fw::ContainerWidget* paused_rect_widget;
	fw::ContainerWidget* toolbox_widget;
	Tool* selected_tool = nullptr;
	std::vector<Tool*> tools_in_tool_panel;
	FpsCounter fps_counter;
	fw::ContainerWidget* fps_widget;
	fw::TextWidget* fps_text_widget;
	fw::RectangleWidget* logger_widget;
	fw::TextWidget* logger_text_widget;
	sf::CircleShape origin_shape;
	sf::Text object_info_text;
	sf::Text id_text;

	const float MOUSE_FORCE_SCALE = 50.0f;
	float timeStep = 1.0f / FPS;
	bool paused = true;
	bool render_object_info = true;
	b2Vec2 b2MousePosWorld;
	GameObject* active_object = nullptr;
	Simulation simulation;

	History<std::string> history;
	bool commit_action = false;
	struct LoadRequest {
		bool requested = false;
		std::string filename;
	};
	LoadRequest load_request;
	std::string quicksave_str;
	bool quickload_requested = false;
	bool debug_break = false;

	void onInit() override;
	void onStart() override;
	void onFrameBegin() override;
	void onFrameEnd() override;
	void onProcessWidgets() override;
	void onProcessWindowEvent(const sf::Event& event) override;
	void onProcessKeyboardEvent(const sf::Event& event) override;
	void beforeProcessMouseEvent(const sf::Event& event) override;
	void onProcessLeftClick() override;
	void onProcessLeftRelease() override;
	void onProcessMouseScroll(const sf::Event& event) override;
	void onProcessMouse() override;
	void afterProcessInput() override;
	void onProcessWorld() override;
	void onRender() override;
	void initTools();
	void initUi();
	void initWidgets();
	void renderWorld();
	void renderUi();
	std::string serialize() const;
	void deserialize(const std::string& str, bool set_camera);
	void saveToFile(const std::string& filename) const;
	void loadAction(const std::string& filename);
	void loadFromFile(const std::string& filename);
	void quicksave();
	void quickload();
	void setActiveObject(GameObject* object);
	Tool* trySelectToolByIndex(size_t index);
	Tool* trySelectTool(Tool* tool);
	void selectCreateType(size_t type);
	void togglePause();
	void loadFont(sf::Font& font, const std::string& filename, bool smooth = false);
	sf::Vector2f screenToWorld(const sf::Vector2f& screen_pos) const;
	sf::Vector2f pixelToWorld(const sf::Vector2i& screen_pos) const;
	sf::Vector2f worldToScreen(const sf::Vector2f& world_pos) const;
	sf::Vector2f worldToScreen(const b2Vec2& world_pos) const;
	sf::Vector2i worldToPixel(const sf::Vector2f& world_pos) const;
	sf::Vector2i worldToPixel(const b2Vec2& world_pos) const;
	sf::Vector2f worldDirToScreenf(const b2Vec2& world_dir) const;
	ptrdiff_t mouseGetChainEdge(const b2Fixture* fixture) const;
	b2Fixture* getFixtureAt(const sf::Vector2f& screen_pos) const;
	GameObject* getObjectAt(const sf::Vector2f& screen_pos) const;
	ptrdiff_t mouseGetObjectVertex() const;
	ptrdiff_t mouseGetObjectEdge() const;
	ptrdiff_t mouseGetEdgeVertex() const;
	void selectVerticesInRect(const RectangleSelect& rectangle_select);
	void selectObjectsInRect(const RectangleSelect& rectangle_select);
	void renderRectangleSelect(sf::RenderTarget& target, RectangleSelect& rectangle_select);
	void getScreenNormal(const b2Vec2& v1, const b2Vec2& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2) const;
	void getScreenNormal(const sf::Vector2i& v1, const sf::Vector2i& v2, sf::Vector2f& norm_v1, sf::Vector2f& norm_v2) const;
	bool isParentSelected(const GameObject* object) const;
	void grabSelected();
	void rotateSelected();
	void deleteObject(GameObject* object, bool remove_children);
	void checkDebugbreak();

};
