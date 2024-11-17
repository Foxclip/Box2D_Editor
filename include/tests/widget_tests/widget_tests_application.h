#include "widgets/widget.h"
#include "widgets/application.h"
#include "test_lib/test.h"

class WidgetTests;

class WidgetTestsApplication : public test::TestModule {
public:
	WidgetTestsApplication(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void basicTest(test::Test& test);
	void initTest(test::Test& test);
	void startTest(test::Test& test);
	void advanceTest(test::Test& test);
	void closeTest(test::Test& test);
	void mouseEventsTest(test::Test& test);
	void keyboardEventsTest(test::Test& test);

private:
	sf::RenderWindow& getWindow();

};

class TestApplication : public fw::Application {
public:
	TestApplication(sf::RenderWindow& window);

	bool initialized = false;
	bool started = false;
	bool frame_begin = false;
	bool frame_end = false;
	bool process_widgets = false;
	bool process_window_event = false;
	bool process_keyboard_event = false;
	bool process_left_press = false;
	bool process_right_press = false;
	bool process_left_release = false;
	bool process_right_release = false;
	bool process_mouse_move = false;
	bool process_mouse_scroll_x = false;
	bool process_mouse_scroll_y = false;
	bool process_keyboard = false;
	bool process_mouse = false;
	bool after_process_input = false;
	bool process_world = false;
	bool rendered = false;
	bool closed = false;
	sf::Vector2i left_click_pos;
	sf::Vector2i left_release_pos;
	sf::Vector2i right_click_pos;
	sf::Vector2i right_release_pos;
	float scroll_x_delta = 0.0f;
	float scroll_y_delta = 0.0f;
	bool space_key_pressed = false;

	void onInit() override;
	void onStart() override;
	void onFrameBegin() override;
	void onFrameEnd() override;
	void onProcessWidgets() override;
	void onProcessWindowEvent(const sf::Event& event) override;
	void onProcessKeyboardEvent(const sf::Event& event) override;
	void onProcessLeftPress() override;
	void onProcessRightPress() override;
	void onProcessLeftRelease() override;
	void onProcessRightRelease() override;
	void onProcessMouseMove() override;
	void onProcessMouseScrollX(float delta) override;
	void onProcessMouseScrollY(float delta) override;
	void onProcessKeyboard() override;
	void onProcessMouse() override;
	void onAfterProcessInput() override;
	void onProcessWorld() override;
	void onRender() override;
	void onClose() override;
};
