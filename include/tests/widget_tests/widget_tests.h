#pragma once

#include "widgets/widgets.h"
#include "test_lib/test.h"

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
	sf::RenderWindow window;
	fw::Font textbox_font;

	WidgetTests(const std::string& name, test::TestModule* manager, const std::vector<TestNode*>& required_nodes = { });
	static std::string sfVec2fToStr(const sf::Vector2f& vec);
	static std::string sfVec2iToStr(const sf::Vector2i& vec);
	static std::string sfVec2uToStr(const sf::Vector2u& vec);
	static std::string cursorTypeToStr(sf::Cursor::Type type);
	static std::string floatRectToStr(const sf::FloatRect& rect);
	static std::string colorToStr(const sf::Color& color);
	static std::string anchorToStr(fw::Widget::Anchor anchor);
	static bool rectApproxCmp(const sf::FloatRect& left, const sf::FloatRect& right);
	static void genericWidgetTest(const GenericWidgetTest& gwt);

protected:
	void beforeRunModule() override;
	void afterRunModule() override;

private:
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
