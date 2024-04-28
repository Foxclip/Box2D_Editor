#pragma once

#include <SFML/Graphics.hpp>
#include "widget_list.h"
#include <queue>

namespace fw {

	Logger& operator<<(Logger& lg, const sf::Vector2f& value);

	struct MouseGesture {
		enum MouseGestureType {
			NORMAL,
			MOVE,
		};
		enum MouseGestureSource {
			WIDGETS,
			SCREEN,
		};
		bool active = false;
		MouseGestureSource source;
		MouseGestureType type;
		sf::Vector2f startPos;
		sf::Mouse::Button button;
		MouseGesture();
		MouseGesture(
			MouseGestureSource source,
			MouseGestureType type,
			sf::Vector2f startPos,
			sf::Mouse::Button button
		);
	};

	class Application {
	public:
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
		void addExternalEvent(const sf::Event& event);
		void mouseMove(int x, int y);
		void mouseMove(const sf::Vector2i& pos);
		void mouseLeftPress();
		void mouseLeftRelease();
		void keyPress(sf::Keyboard::Key key);
		void keyRelease(sf::Keyboard::Key key);
		void textEntered(sf::Uint32 code);
		bool isLCtrlPressed() const;
		bool isLAltPressed() const;
		bool isLShiftPressed() const;
		sf::Vector2i getMousePos() const;
		sf::Vector2f getMousePosf() const;
		const sf::Vector2f& getMousePressPosf() const;
		WidgetList& getWidgets();
		void setBackgroundColor(const sf::Color& color);
		void close();

	protected:
		sf::RenderWindow window;
		sf::View window_view;
		WidgetList widgets = WidgetList(*this);
		sf::Vector2i mousePrevPos;
		sf::Vector2f mousePressPosf;
		bool leftButtonPressed = false;
		bool rightButtonPressed = false;
		MouseGesture mouseGesture;
		sf::Cursor arrow_cursor;
		sf::Cursor text_cursor;
		bool external_control = false;
		sf::Vector2i external_mouse_pos;
		bool external_lctrl_pressed = false;
		bool external_lalt_pressed = false;
		bool external_lshift_pressed = false;
		std::queue<sf::Event> external_event_queue;
		sf::Color background_color = sf::Color::Black;

		virtual void onInit();
		virtual void onStart();
		virtual void onFrameBegin();
		virtual void onFrameEnd();
		virtual void onProcessWidgets();
		virtual void onProcessWindowEvent(const sf::Event& event);
		virtual void onProcessKeyboardEvent(const sf::Event& event);
		virtual void beforeProcessMouseEvent(const sf::Event& event);
		virtual void onProcessLeftClick();
		virtual void onProcessLeftRelease();
		virtual void onProcessMouseScroll(const sf::Event& event);
		virtual void onProcessKeyboard();
		virtual void onProcessMouse();
		virtual void afterProcessInput();
		virtual void onProcessWorld();
		virtual void onRender();
		virtual void onClose();
		void startMoveGesture();

	private:
		void mainLoop();
		void processWidgets();
		void processInput();
		void processEvent(const sf::Event& event);
		void processWindowEvent(const sf::Event& event);
		void processKeyboardEvent(const sf::Event& event);
		void processMouseEvent(const sf::Event& event);
		void startNormalGesture(
			MouseGesture::MouseGestureSource source,
			sf::Mouse::Button button
		);
		void startMoveGesture(
			MouseGesture::MouseGestureSource source
		);
		void endGesture();
		void processLeftClick();
		void processLeftRelease();
		void processKeyboard();
		void processMouse();
		void processWorld();
		void render();
	};

}
