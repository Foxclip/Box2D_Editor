#pragma once

#include <SFML/Graphics.hpp>
#include "widget_list.h"
#include <queue>

namespace fw {

	struct MouseGesture {
		enum MouseGestureType {
			NORMAL,
			MOVE,
		};
		bool active = false;
		Widget* source = nullptr;
		MouseGestureType type;
		sf::Vector2f startPos;
		sf::Mouse::Button button;
		MouseGesture();
		MouseGesture(
			Widget* source,
			MouseGestureType type,
			sf::Vector2f startPos,
			sf::Mouse::Button button
		);
	};

	enum class Stage {
		NONE,
		PROCESS,
		BEFORE_INPUT,
		INPUT,
		AFTER_INPUT,
		WORLD,
		UPDATE,
		RENDER,
	};

	class Application {
	public:
		Application();
		Application(sf::RenderWindow& window);
		virtual void init(
			const sf::String& window_title,
			unsigned int window_width,
			unsigned int window_height,
			unsigned int antialiasing,
			bool vsync
		);
		virtual void start(bool external_control = false);
		void advance();
		void maximizeWindow() const;
		sf::Vector2u getWindowSize() const;
		const fw::Font& getDefaultFont() const;
		Stage getStage() const;
		Widget* getLeftGestureSource() const;
		Widget* getRightGestureSource() const;
		const sf::RenderTexture& getRenderTexture() const;
		sf::Image getRenderedImage() const;
		void setWindowSize(unsigned int width, unsigned int height);
		void setWindowSize(const sf::Vector2u& size);
		void addExternalEvent(const sf::Event& event);
		void mouseMove(int x, int y);
		void mouseMove(unsigned int x, unsigned int y);
		void mouseMove(float x, float y);
		void mouseMove(const sf::Vector2i& pos);
		void mouseMove(const sf::Vector2f& pos);
		void mouseLeftPress();
		void mouseRightPress();
		void mouseLeftRelease();
		void mouseRightRelease();
		void mouseLeftClick();
		void mouseRightClick();
		void mouseLeftClick(const sf::Vector2f& pos);
		void mouseRightClick(const sf::Vector2f& pos);
		void mouseScrollX(float delta);
		void mouseScrollY(float delta);
		void keyPress(sf::Keyboard::Key key);
		void keyRelease(sf::Keyboard::Key key);
		void textEntered(sf::Uint32 code);
		bool isLeftButtonPressed() const;
		bool isRightButtonPressed() const;
		bool isMiddleButtonPressed() const;
		bool isLCtrlPressed() const;
		bool isLAltPressed() const;
		bool isLShiftPressed() const;
		sf::Vector2i getMousePos() const;
		sf::Vector2f getMousePosf() const;
		const sf::Vector2f& getMousePressPosf() const;
		sf::Vector2f getWindowCenter() const;
		WidgetList& getWidgets();
		void setBackgroundColor(const sf::Color& color);
		void setDefaultFont(const fw::Font& font);
		void setVerticalSyncEnabled(bool value);
		void close();

	protected:
		sf::RenderWindow& window;
		sf::RenderWindow internal_window;
		bool external_window = false;
		bool running = true;
		sf::View window_view;
		WidgetList widgets = WidgetList(*this);
		sf::Vector2i internal_mouse_pos;
		sf::Vector2i mousePrevPos;
		sf::Vector2f mousePrevPosf;
		sf::Vector2f mousePressPosf;
		bool leftButtonPressed = false;
		bool rightButtonPressed = false;
		bool middleButtonPressed = false;
		MouseGesture mouse_gesture_left;
		MouseGesture mouse_gesture_right;
		sf::Cursor arrow_cursor;
		sf::Cursor text_cursor;
		sf::Cursor size_top_left_cursor;
		sf::Cursor size_top_cursor;
		sf::Cursor size_top_right_cursor;
		sf::Cursor size_left_cursor;
		sf::Cursor size_right_cursor;
		sf::Cursor size_bottom_left_cursor;
		sf::Cursor size_bottom_cursor;
		sf::Cursor size_bottom_right_cursor;
		bool external_control = false;
		sf::Vector2i external_mouse_pos;
		bool external_lctrl_pressed = false;
		bool external_lalt_pressed = false;
		bool external_lshift_pressed = false;
		std::queue<sf::Event> external_event_queue;
		sf::Color background_color = sf::Color::Black;
		sf::Cursor::Type current_cursor_type = sf::Cursor::Arrow;

		virtual void onInit();
		virtual void onStart();
		virtual void onFrameBegin();
		virtual void onFrameEnd();
		virtual void onProcessWidgets();
		virtual void onProcessWindowEvent(const sf::Event& event);
		virtual void onProcessKeyboardEvent(const sf::Event& event);
		virtual void onProcessLeftPress();
		virtual void onProcessRightPress();
		virtual void onProcessLeftRelease();
		virtual void onProcessRightRelease();
		virtual void onProcessMouseMove();
		virtual void onProcessMouseScrollX(float delta);
		virtual void onProcessMouseScrollY(float delta);
		virtual void onProcessKeyboard();
		virtual void onProcessMouse();
		virtual void onBeforeProcessInput();
		virtual void onAfterProcessInput();
		virtual void onProcessWorld();
		virtual void onRender();
		virtual void onClose();
		void startMoveGesture(Widget* source);
		void endGestureLeft();
		void endGestureRight();
		void loadShader(sf::Shader& shader, sf::Shader::Type type, const std::filesystem::path& path);
		void loadDefaultFragmentShader(sf::Shader& shader, const std::string& name);
		void loadFragmentShaderPart(sf::Shader& shader, const std::filesystem::path& path);
		void loadFragmentShaderParts(sf::Shader& shader, const std::string& name, const std::vector<std::filesystem::path>& paths);

	private:
		friend class Widget;
		friend class CanvasWidget;
		Stage stage = Stage::NONE;
		sf::Shader premultiply;
		sf::Shader default_shader;
		sf::RenderTexture render_texture;

		void mainLoop();
		void processWidgets();
		void processBeforeInput();
		void processInput();
		void processAfterInput();
		void processEvent(const sf::Event& event);
		void processWindowEvent(const sf::Event& event);
		void processKeyboardEvent(const sf::Event& event);
		void processMouseEvent(const sf::Event& event);
		void startNormalGesture(Widget* source, sf::Mouse::Button button);
		void processLeftPress();
		void processRightPress();
		void processMiddlePress();
		void processLeftRelease();
		void processRightRelease();
		void processMiddleRelease();
		void processMouseMove();
		void processScrollX(float delta);
		void processScrollY(float delta);
		void processKeyboard();
		void processMouse();
		void processWorld();
		void render();
		void setCursorType(sf::Cursor::Type type);
		std::string getCombinedShaderString(const std::vector<std::filesystem::path>& shaders, const std::filesystem::path& combined_template);

	};

}
