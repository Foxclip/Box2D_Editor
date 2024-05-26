#pragma once

#include "widgets/widgets.h"
#include "test.h"
#include <random>

struct Node {
	std::string str;
	std::vector<Node*> parents;
	Node(const std::string& str);
	void addParent(Node* parent);
	static const std::vector<Node*>& getParents(Node* node);
};

class NodeList {
public:
	Node* createNode(const std::string& name);
	const std::vector<Node*>& getNodes() const;
private:
	CompVectorUptr<Node> nodes;
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
	bool before_process_mouse_event = false;
	bool process_left_press = false;
	bool process_right_press = false;
	bool process_left_release = false;
	bool process_right_release = false;
	bool process_mouse_scroll = false;
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
	bool space_key_pressed = false;

	void onInit() override;
	void onStart() override;
	void onFrameBegin() override;
	void onFrameEnd() override;
	void onProcessWidgets() override;
	void onProcessWindowEvent(const sf::Event& event) override;
	void onProcessKeyboardEvent(const sf::Event& event) override;
	void beforeProcessMouseEvent(const sf::Event& event) override;
	void onProcessLeftPress() override;
	void onProcessRightPress() override;
	void onProcessLeftRelease() override;
	void onProcessRightRelease() override;
	void onProcessMouseScroll(const sf::Event& event) override;
	void onProcessKeyboard() override;
	void onProcessMouse() override;
	void afterProcessInput() override;
	void onProcessWorld() override;
	void onRender() override;
	void onClose() override;
};

struct GenericWidgetTest {
	GenericWidgetTest(fw::Application& application, test::Test& test);
	fw::Application& application;
	test::Test& test;
	fw::Widget* widget = nullptr;
    size_t total_widgets = 0;
	fw::Widget::WidgetType type = fw::Widget::WidgetType::None;
    std::string name;
    std::string fullname;
    bool is_visual_position_quantized = false;
    fw::WidgetVisibility visibility;
	bool is_click_through = false;
	bool is_mouse_over = false;
	bool is_focusable = false;
	bool is_focused = false;
	bool clip_children = false;
	bool force_custom_cursor = false;
	fw::Widget* parent = nullptr;
	sf::FloatRect local_bounds;
	sf::FloatRect global_bounds;
	sf::FloatRect parent_local_bounds;
	sf::FloatRect visual_local_bounds;
	sf::FloatRect visual_global_bounds;
	sf::FloatRect visual_parent_local_bounds;
};

class WidgetTests : public test::TestModule {
public:
	WidgetTests(test::TestManager& manager);
	void createTestLists() override;

protected:
	void createToposortList(test::TestList* list);
	void createApplicationList(test::TestList* list);
	void createWidgetsList(test::TestList* list);
	void beforeRunModule() override;
	void afterRunModule() override;

private:
	sf::RenderWindow window;
	sf::Font textbox_font;
	std::mt19937 rng;

	void toposortEmptyTest(test::Test& test);
	void toposort1NodeTest(test::Test& test);
	void toposort3NodesTest(test::Test& test);
	void toposort4NodesDiamondTest(test::Test& test);
	void toposort5NodesXTest(test::Test& test);
	void toposort5NodesRandomTest(test::Test& test);
	void toposortHairTest(test::Test& test);
	void toposortInverseHairTest(test::Test& test);
	void toposortLoopExceptionTest(test::Test& test);
	void toposortLoopEventTest(test::Test& test);
	void toposortTwoLoopsTest(test::Test& test);
	void toposortForkingLoopTest(test::Test& test);
	void toposortTrianglesLoopTest(test::Test& test);

	void basicTest(test::Test& test);
	void initTest(test::Test& test);
	void startTest(test::Test& test);
	void advanceTest(test::Test& test);
	void closeTest(test::Test& test);
	void mouseEventsTest(test::Test& test);
	void keyboardEventsTest(test::Test& test);

	void rootWidgetTest(test::Test& test);
	void emptyWidgetTest(test::Test& test);
	void rectangleWidgetTest(test::Test& test);
	void setParentTest(test::Test& test);
	void widgetMouseEvents1(test::Test& test);
	void widgetMouseEvents2(test::Test& test);
	void eventsTest(test::Test& test);
	void coordinatesTest(test::Test& test);
	void findTest(test::Test& test);
	void anchorTest(test::Test& test);
	void textWidgetTest(test::Test& test);
	void checkboxWidgetBasicTest(test::Test& test);
	void checkboxWidgetToggleTest(test::Test& test);
	void containerWidgetBasicTest(test::Test& test);
	void containerWidgetChildrenTest(test::Test& test);
	void sizePolicyTest(test::Test& test);
	void sizePolicyPositionTest(test::Test& test);
	void sizePolicyExpandTest(test::Test& test);
	void sizePolicyLimitsTest(test::Test& test);
	void sizePolicyCombinedTest(test::Test& test);
	void textboxWidgetBasicTest(test::Test& test);
	void textboxWidgetInputTest(test::Test& test);
	void textboxWidgetEventsTest(test::Test& test);
	void textboxWidgetCursorTest(test::Test& test);
	void textboxWidgetScrollTest(test::Test& test);
	void textboxWidgetResizingTest(test::Test& test);
	void textboxWidgetSelectionTest(test::Test& test);
	void textboxWidgetMouseClickTest(test::Test& test);
	void textboxWidgetMouseDragTest(test::Test& test);
	void textboxWidgetCopyPasteTest(test::Test& test);
	void textboxWidgetHistoryTest(test::Test& test);
	void textboxWidgetIntegerTest(test::Test& test);
	void textboxWidgetFloatTest(test::Test& test);
	void canvasWidgetBasicTest(test::Test& test);
	void canvasWidgetDrawTest(test::Test& test);
	void windowWidgetBasicTest(test::Test& test);
	void windowWidgetDragTest(test::Test& test);
	void windowWidgetChildrenTest(test::Test& test);
	void windowWidgetResizeTest(test::Test& test);
	void windowWidgetChainTest(test::Test& test);
	void windowWidgetDragLimitsTest(test::Test& test);
	void windowWidgetResizeLimitsTest(test::Test& test);

	static std::string sfVec2fToStr(const sf::Vector2f& vec);
	static std::string sfVec2iToStr(const sf::Vector2i& vec);
	static std::string sfVec2uToStr(const sf::Vector2u& vec);
	static std::string cursorTypeToStr(sf::Cursor::Type type);
	static std::string floatRectToStr(const sf::FloatRect& rect);
	static std::string colorToStr(const sf::Color& color);
	static std::string anchorToStr(fw::Widget::Anchor anchor);
	fw::TextBoxWidget* initTextBox(fw::Application& application, float width, float height) const;
	void resizeWindow(
		fw::Application& application,
		test::Test& test,
		fw::WindowWidget* widget,
		const sf::Vector2f& begin_pos,
		const sf::Vector2f& resize_offset,
		const sf::FloatRect& new_bounds
	);
	void resizeWindowTest(
		fw::Application& application,
		test::Test& test,
		fw::WindowWidget* widget
	);
	void genericWidgetTest(const GenericWidgetTest& gwt);
	bool layer_contains(const std::vector<Node*>& layer, const std::string& name);

	template <typename TVec>
	TVec get_shuffled(const TVec& vec) {
		TVec copy(vec);
		std::shuffle(copy.begin(), copy.end(), rng);
		return copy;
	}

};
