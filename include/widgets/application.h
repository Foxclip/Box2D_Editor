#pragma once

#include <SFML/Graphics.hpp>
#include "widget_list.h"
#include <queue>

Logger& operator<<(Logger& lg, const sf::Vector2f& value);

namespace fw {

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
			unsigned int antialiasing
		);
		virtual void start(bool external_control = false);
		void advance();
		void maximizeWindow() const;
		sf::Vector2u getWindowSize() const;
		void setExternalMousePos(const sf::Vector2i& pos);
		void addExternalEvent(const sf::Event& event);
		sf::Vector2i getMousePos() const;
		void close();

	protected:
		sf::RenderWindow window;
		sf::View window_view;
		WidgetList widgets;
		sf::Vector2i mousePos;
		sf::Vector2f mousePosf;
		sf::Vector2i mousePrevPos;
		sf::Vector2f mousePressPosf;
		bool leftButtonPressed = false;
		bool rightButtonPressed = false;
		MouseGesture mouseGesture;
		sf::Cursor arrow_cursor;
		sf::Cursor text_cursor;
		bool external_control = false;
		sf::Vector2i external_mouse_pos;
		std::queue<sf::Event> external_event_queue;

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
