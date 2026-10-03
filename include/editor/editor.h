#pragma once

#include <functional>
#include <memory>
#include <set>
#include "tools.h"
#include "simulation/simulation.h"
#include "common/history.h"
#include "logger/logger.h"
#include "widgets/application.h"
#include "widgets/font.h"
#include <glvx/circle.h>
#include <glvx/shader.h>

const std::string WINDOW_TITLE = "Box2D Editor";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int ANTIALIASING = 0;
const float MOUSE_SCROLL_ZOOM = 1.2f;
const int FPS = 60;
const int SCREENSHOT_WARMUP_FRAMES = 8;
const float WORLD_SATURATION = 0.75f;
const float WORLD_COLOR_SCALE_CENTER = 0.25f;
const float WORLD_COLOR_SCALE_PERCENT = 0.6f;
const glvx::Vector3 SELECTION_OUTLINE_COLOR = glvx::Vector3(1.0f, 1.0f, 0.0f);
const glvx::Vector3 HOVER_OUTLINE_COLOR = glvx::Vector3(1.0f, 1.0f, 0.0f);
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

class Editor;

class Camera {
public:
	Camera(Editor& editor);
	const b2Vec2& getPosition() const;
	float getZoom() const;
	void setPosition(float x, float y);
	void setPosition(const b2Vec2& p_pos);
	void move(float x, float y);
	void move(const b2Vec2& offset);
	void setZoom(float zoom);
	std::string serialize() const;
	TokenWriter& serialize(TokenWriter& tw) const;
	void deserialize(const std::string& str);
	void deserialize(TokenReader& tr);
	Camera& operator=(const Camera& right);
private:
	Editor& editor;
	b2Vec2 pos = b2Vec2_zero;
	float zoom = 30.0f;
};

glvx::Vector2f to2f(glvx::Vector2i vec);
glvx::Vector2f to2f(glvx::Vector2u vec);

namespace fw {
	class RectangleWidget;
	class TextWidget;
	class ContainerWidget;
}

class Outliner;
class Toolbox;
class Menu;

class Editor : public fw::Application {
public:
	Editor(bool maximized = false);
	Editor(glvx::Window& window, bool maximized = false);
	void init(const std::string& title, bool vsync = true, bool minimized = false);
	void load(const std::string& filename);
	bool saveScreenshot(const std::string& file_path);
	void setCameraPos(float x, float y);
	void setCameraPos(const b2Vec2& pos);
	void setCameraZoom(float zoom);
	Camera& getCamera();
	void selectSingleObject(GameObject* object, bool with_children = false);
	Simulation& getSimulation();
	const CompVector<GameObject*>& getTopObjects() const;
	const CompVector<GameObject*>& getAllObjects() const;
	SelectTool& getSelectTool();
	void setActiveObject(GameObject* object);
	BoxObject* createBox(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const b2Vec2& size,
		const glvx::Color& color
	);
	BallObject* createBall(
		const std::string& name,
		const b2Vec2& pos,
		float radius,
		const glvx::Color& color,
		const glvx::Color& notch_color = glvx::Color::Transparent
	);
	PolygonObject* createPolygon(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const std::vector<b2Vec2>& vertices,
		const glvx::Color& color
	);
	PolygonObject* createCar(
		const std::string& name,
		const b2Vec2& pos,
		const std::vector<float>& lengths,
		const std::vector<float>& wheels,
		const glvx::Color& color
	);
	ChainObject* createChain(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const std::vector<b2Vec2>& vertices,
		const glvx::Color& color
	);

private:
	friend class EditWindow;
	friend class EditWindowParameter;
	friend class TextParameter;
	friend class BoolParameter;
	friend class FloatParameter;
	friend class Toolbox;
	friend class CreatePanel;
	friend class Outliner;
	friend class Menu;
	friend class EditorTests;
	friend class Camera;
	fw::CanvasWidget* world_widget = nullptr;
	fw::CanvasWidget* ui_widget = nullptr;
	fw::CanvasWidget* selection_mask_widget = nullptr;
	std::unique_ptr<glvx::Shader> desat_shader;
	std::unique_ptr<glvx::Shader> selection_shader;
	Camera camera = Camera(*this);
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
	fw::Font ui_font;
	fw::Font fps_font;
	fw::Font console_font;
	fw::Font small_font;
	fw::Font textbox_font;
	fw::ContainerWidget* paused_rect_widget = nullptr;
	Toolbox* toolbox_widget = nullptr;
	Tool* selected_tool = nullptr;
	std::vector<Tool*> tools_in_tool_panel;
	FpsCounter fps_counter;
	fw::ContainerWidget* fps_widget = nullptr;
	fw::TextWidget* fps_text_widget = nullptr;
	fw::RectangleWidget* logger_widget = nullptr;
	fw::TextWidget* logger_text_widget = nullptr;
	fw::TextWidget* step_widget = nullptr;
	glvx::Circle origin_shape;
	glvx::Circle origin_shape_outline;
	glvx::Text object_info_text;
	glvx::Text id_text;
	Outliner* outliner_widget = nullptr;
	Menu* menu_widget = nullptr;
	fw::TextWidget* debug_release_widget = nullptr;

	const float MOUSE_FORCE_SCALE = 50.0f;
	float timeStep = 1.0f / FPS;
	bool paused = true;
	bool render_object_info = true;
	GameObject* active_object = nullptr;
	GameObject* follow_object = nullptr;
	Simulation simulation;

	glvx::Vector2f mouse_world_pos;
	History<std::string> history;
	bool commit_action = false;
	struct LoadRequest {
		bool requested = false;
		std::filesystem::path path;
	};
	LoadRequest load_request;
	std::string quicksave_str;
	bool quickload_requested = false;
	std::filesystem::path save_file_location;
	bool debug_break = false;
	mutable Logger editor_logger;
	bool maximize_window = false;

	void onInit() override;
	void onStart() override;
	void onFrameBegin() override;
	void onFrameEnd() override;
	void onProcessWidgets() override;
	void onProcessWindowEvent(const glvx::Event& event) override;
	void onProcessKeyboardEvent(const glvx::Event& event) override;
	void processLeftPress(const glvx::Vector2f& pos);
	void processGlobalLeftRelease(const glvx::Vector2f& pos);
	void processBlockableLeftRelease(const glvx::Vector2f& pos);
	void processMouseScrollY(float delta);
	void processMouse(const glvx::Vector2f& pos);
	void processDragGestureLeft(const glvx::Vector2f& pos);
	void processDragGestureRight(const glvx::Vector2f& pos);
	void onAfterProcessInput() override;
	void onProcessWorld() override;
	void onRender() override;
	void initTools();
	void initUi();
	void initWidgets();
	void renderWorld();
	void renderUi();
	std::string serialize() const;
	void deserialize(const std::string& str, bool set_camera);
	void save();
	void saveToFile(const std::filesystem::path& path);
	void requestLoad(const std::filesystem::path& path);
	void loadFromFile(const std::filesystem::path& path);
	void quicksave();
	void quickload();
	void showOpenFileMenu();
	void showSaveFileMenu();
	Tool* trySelectToolByIndex(size_t index);
	Tool* trySelectTool(Tool* tool);
	void selectCreateType(size_t type);
	void togglePause();
	glvx::Vector2f screenToWorld(const glvx::Vector2f& screen_pos) const;
	glvx::Vector2f pixelToWorld(const glvx::Vector2i& screen_pos) const;
	glvx::Vector2f worldToScreen(const glvx::Vector2f& world_pos) const;
	glvx::Vector2f worldToScreen(const b2Vec2& world_pos) const;
	glvx::Vector2i worldToPixel(const glvx::Vector2f& world_pos) const;
	glvx::Vector2i worldToPixel(const b2Vec2& world_pos) const;
	glvx::Vector2f worldDirToScreenf(const b2Vec2& world_dir) const;
	glvx::Vector2f getMouseWorldPos() const;
	b2Vec2 getMouseWorldPosb2() const;
	ptrdiff_t mouseGetChainEdge(const b2Fixture* fixture) const;
	b2Fixture* getFixtureAt(const glvx::Vector2f& screen_pos) const;
	GameObject* getObjectAt(const glvx::Vector2f& screen_pos) const;
	glvx::Vector2f getObjectScreenPos(GameObject* object) const;
	b2AABB getObjectsAABB(const CompVector<GameObject*>& objects) const;
	ptrdiff_t mouseGetObjectVertex() const;
	ptrdiff_t mouseGetObjectEdge() const;
	ptrdiff_t mouseGetEdgeVertex() const;
	void selectVerticesInRect(const RectangleSelect& rectangle_select);
	void selectObjectsInRect(const RectangleSelect& rectangle_select);
	void renderRectangleSelect(glvx::RenderTarget& target, RectangleSelect& rectangle_select);
	void renderRectangleSelect(fw::CanvasWidget* canvas, RectangleSelect& rectangle_select);
	void getScreenNormal(const b2Vec2& v1, const b2Vec2& v2, glvx::Vector2f& norm_v1, glvx::Vector2f& norm_v2) const;
	void getScreenNormal(const glvx::Vector2i& v1, const glvx::Vector2i& v2, glvx::Vector2f& norm_v1, glvx::Vector2f& norm_v2) const;
	bool isParentSelected(const GameObject* object) const;
	void grabSelected(Tool* selected_tool);
	void rotateSelected(Tool* selected_tool);
	void endMove(bool confirm);
	void endRotate(bool confirm);
	void deleteObject(GameObject* object, bool remove_children);
	void viewSelectedObjects();
	void checkDebugbreak();
	void canvasDraw(fw::CanvasWidget* canvas, const glvx::Drawable& drawable, const glvx::RenderStates& states = glvx::RenderStates());

};
