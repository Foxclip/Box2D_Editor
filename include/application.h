#include "widgets/widgets.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int ANTIALIASING = 0;

class Application {
public:
	virtual void init();
	void start();

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
	bool leftButtonPressGesture = false;
	bool leftButtonProcessWidgetsOnPress = true;
	bool leftButtonProcessWidgetsOnRelease = true;
	sf::Cursor arrow_cursor;
	sf::Cursor text_cursor;

	virtual void onInit();
	virtual void onStart();
	virtual void onFrameBegin();
	virtual void onFrameEnd();
	virtual void onProcessWidgets();
	virtual void onProcessWindowEvent(const sf::Event& event);
	virtual void onProcessKeyboardEvent(const sf::Event& event);
	virtual void beforeProcessMouseEvent(const sf::Event& event);
	virtual void onProcessMouseEvent(const sf::Event& event);
	virtual void onProcessLeftClick();
	virtual void onProcessLeftRelease();
	virtual void onProcessMouseScroll(const sf::Event& event);
	virtual void onProcessKeyboard();
	virtual void onProcessMouse();
	virtual void afterProcessInput();
	virtual void onProcessWorld();
	virtual void onRender();

private:
	void mainLoop();
	void processWidgets();
	void processInput();
	void processWindowEvent(const sf::Event& event);
	void processKeyboardEvent(const sf::Event& event);
	void processMouseEvent(const sf::Event& event);
	void processLeftClick();
	void processLeftRelease();
	void processKeyboard();
	void processMouse();
	void processWorld();
	void render();
};
