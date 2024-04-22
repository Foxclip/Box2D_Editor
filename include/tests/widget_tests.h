#pragma once

#include "widgets/widgets.h"
#include "test.h"

class TestApplication : public fw::Application {
public:
	bool initialized = false;
	bool started = false;
	bool frame_begin = false;
	bool frame_end = false;
	bool process_widgets = false;
	bool process_window_event = false;
	bool process_keyboard_event = false;
	bool before_process_mouse_event = false;
	bool process_left_click = false;
	bool process_left_release = false;
	bool process_mouse_scroll = false;
	bool process_keyboard = false;
	bool process_mouse = false;
	bool after_process_input = false;
	bool process_world = false;
	bool rendered = false;
	bool closed = false;
	sf::Vector2i click_pos;
	sf::Vector2i release_pos;
	bool space_key_pressed = false;

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
	void onProcessKeyboard() override;
	void onProcessMouse() override;
	void afterProcessInput() override;
	void onProcessWorld() override;
	void onRender() override;
	void onClose() override;
};

class WidgetTests : public test::TestModule {
public:
	WidgetTests(test::TestManager& manager);
	void createTestLists() override;

protected:
	void createApplicationList();
	void createWidgetsList();

private:
	static std::string sfVec2fToStr(const sf::Vector2f& vec);
	static std::string sfVec2iToStr(const sf::Vector2i& vec);
	static std::string sfVec2uToStr(const sf::Vector2u& vec);
	static std::string cursorTypeToStr(sf::Cursor::Type type);
	static std::string floatRectToStr(const sf::FloatRect& rect);
	static std::string colorToStr(const sf::Color& color);

};
