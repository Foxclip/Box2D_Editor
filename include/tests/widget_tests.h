#pragma once

#include "widgets/widgets.h"
#include "test_lib/test.h"
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
	fw::Widget::FocusableType focusable_type = fw::Widget::FocusableType::NONE;
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
	WidgetTests(const std::string& name, test::TestModule* manager, const std::vector<TestNode*>& required_nodes = { });

protected:
	void beforeRunModule() override;
	void afterRunModule() override;

private:
	sf::RenderWindow window;
	fw::Font textbox_font;
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
	void polygonWidgetBasicTest(test::Test& test);
	void setParentTest(test::Test& test);
	void widgetMouseEvents1(test::Test& test);
	void widgetMouseEvents2(test::Test& test);
	void dragGestureEventTest(test::Test& test);
	void eventsTest(test::Test& test);
	void coordinatesTest(test::Test& test);
	void findTest(test::Test& test);
	void anchorTest(test::Test& test);
	void remove1Test(test::Test& test);
	void remove2Test(test::Test& test);

	void textWidgetTest(test::Test& test);
	void textWidgetDefaultFontTest(test::Test& test);

	void checkboxWidgetBasicTest(test::Test& test);
	void checkboxWidgetToggleTest(test::Test& test);

	void containerWidgetBasicTest(test::Test& test);
	void containerWidgetChildrenTest(test::Test& test);
	void containerWidgetPaddingTest(test::Test& test);

	void sizePolicyTest(test::Test& test);
	void sizePolicyPositionTest(test::Test& test);
	void sizePolicyExpandTest(test::Test& test);
	void sizePolicyLimitsTest(test::Test& test);
	void sizePolicyCombinedTest(test::Test& test);

	void widgetLinkBasicTest(test::Test& test);
	void widgetLinkTargetsFuncTest(test::Test& test);
	void widgetLinkContainerTest(test::Test& test);
	void widgetLinkRemoveTest(test::Test& test);

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
	void canvasWidgetAlphaTest(test::Test& test);

	void windowWidgetBasicTest(test::Test& test);
	void windowWidgetDragTest(test::Test& test);
	void windowWidgetChildrenTest(test::Test& test);
	void windowWidgetResizeTest(test::Test& test);
	void windowWidgetChainTest(test::Test& test);
	void windowWidgetDragLimitsTest(test::Test& test);
	void windowWidgetResizeLimitsTest(test::Test& test);
	void windowWidgetMoveToTopDragTest(test::Test& test);
	void windowWidgetMoveToTopResizeTest(test::Test& test);

	void dropdownWidgetBasicTest(test::Test& test);
	void dropdownWidgetOptions1Test(test::Test& test);
	void dropdownWidgetOptions2Test(test::Test& test);

	void scrollAreaWidgetBasicTest(test::Test& test);
	void scrollAreaWidgetScrollTest(test::Test& test);
	void scrollAreaWidgetScrollbarVisibilityTest(test::Test& test);
	void scrollAreaWidgetScrollbarContainerTest(test::Test& test);

	void treeviewWidgetBasicTest(test::Test& test);
	void treeviewWidgetEntriesTest(test::Test& test);
	void treeviewWidgetParent1Test(test::Test& test);
	void treeviewWidgetParent2Test(test::Test& test);
	void treeviewWidgetSelectTest(test::Test& test);
	void treeviewWidgetReorderTest(test::Test& test);
	void treeviewWidgetRemoveTest(test::Test& test);
	void treeviewWidgetClearTest(test::Test& test);
	void treeviewWidgetDrag1Test(test::Test& test);
	void treeviewWidgetDrag2Test(test::Test& test);

	static std::string sfVec2fToStr(const sf::Vector2f& vec);
	static std::string sfVec2iToStr(const sf::Vector2i& vec);
	static std::string sfVec2uToStr(const sf::Vector2u& vec);
	static std::string cursorTypeToStr(sf::Cursor::Type type);
	static std::string floatRectToStr(const sf::FloatRect& rect);
	static std::string colorToStr(const sf::Color& color);
	static std::string anchorToStr(fw::Widget::Anchor anchor);
	static bool rectApproxCmp(const sf::FloatRect& left, const sf::FloatRect& right);
	fw::TextBoxWidget* initTextBox(fw::Application& application, const std::string& test_name, float width, float height) const;
	void mouseDragGesture(fw::Application& application, const sf::Vector2f& begin_pos, const sf::Vector2f& offset);
	enum class ResizePoint {
		TOP_LEFT,
		TOP,
		TOP_RIGHT,
		LEFT,
		RIGHT,
		BOTTOM_LEFT,
		BOTTOM,
		BOTTOM_RIGHT
	};
	sf::Vector2f getGrabPos(fw::WindowWidget* window, ResizePoint resize_point);
	void resizeWindow(fw::WindowWidget* window, ResizePoint resize_point, const sf::Vector2f offset);
	void resizeWindowTest(fw::Application& application, test::Test& test, fw::WindowWidget* widget);
	sf::Vector2f getHeaderCenter(fw::WindowWidget* window);
	void dragWindow(fw::Application& application, fw::WindowWidget* window, const sf::Vector2f& offset);
	float calcTreeViewEntryHeight(fw::TreeViewEntry* entry);
	float calcTreeViewHeight(fw::TreeViewWidget* treeview);
	void genericWidgetTest(const GenericWidgetTest& gwt);
	bool layer_contains(const std::vector<Node*>& layer, const std::string& name);

	template <typename TVec>
	TVec get_shuffled(const TVec& vec) {
		TVec copy(vec);
		std::shuffle(copy.begin(), copy.end(), rng);
		return copy;
	}

};

#define PRESS_MOUSE_LEFT(pos) \
    application.mouseMove(pos); \
    application.mouseLeftPress(); \
    application.advance();

#define RELEASE_MOUSE_LEFT() \
    application.mouseLeftRelease(); \
    application.advance();

#define CLICK_MOUSE(pos) \
    PRESS_MOUSE_LEFT(pos); \
    RELEASE_MOUSE_LEFT();

#define MOVE_MOUSE(pos) \
    application.mouseMove(pos); \
    application.advance();

#define PRESS_KEY(key) \
    application.keyPress(key); \
    application.advance();

#define RELEASE_KEY(key) \
    application.keyRelease(key); \
    application.advance();

#define TAP_KEY(key) \
    PRESS_KEY(key) \
    RELEASE_KEY(key)

#define ENTER_TEXT(key, code) \
    application.keyPress(key); \
    application.textEntered(code); \
    application.advance(); \
    application.keyRelease(key); \
    application.advance();

#define SELECT_ALL() \
    application.keyPress(sf::Keyboard::LControl); \
    application.keyPress(sf::Keyboard::A); \
    application.advance(); \
    application.keyRelease(sf::Keyboard::A); \
    application.keyRelease(sf::Keyboard::LControl); \
    application.advance();

#define COPY() \
    PRESS_KEY(sf::Keyboard::LControl); \
    ENTER_TEXT(sf::Keyboard::C, 'c'); \
    RELEASE_KEY(sf::Keyboard::LControl);

#define PASTE() \
    PRESS_KEY(sf::Keyboard::LControl); \
    ENTER_TEXT(sf::Keyboard::V, 'v'); \
    RELEASE_KEY(sf::Keyboard::LControl);

#define CUT() \
    PRESS_KEY(sf::Keyboard::LControl); \
    ENTER_TEXT(sf::Keyboard::X, 'x'); \
    RELEASE_KEY(sf::Keyboard::LControl);

#define _CHECK_SELECTION(active, text, cursor_pos, left, right) \
    T_CHECK(textbox_widget->isSelectionActive() == active); \
    T_COMPARE(textbox_widget->getSelectedText(), text); \
    T_COMPARE(textbox_widget->getCursorPos(), cursor_pos); \
    T_COMPARE(textbox_widget->getSelectionLeft(), left); \
    T_COMPARE(textbox_widget->getSelectionRight(), right); \

#define CHECK_SELECTION(active, text, cursor_pos, left, right) \
    T_WRAP_CONTAINER(_CHECK_SELECTION(active, text, cursor_pos, left, right))
