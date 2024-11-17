#include "widgets/widget.h"
#include "test_lib/test.h"

class WidgetTestsTextbox : public test::TestModule {
public:
	WidgetTestsTextbox(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
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

private:
	sf::RenderWindow& getWindow();
	fw::Font& getFont();
	fw::TextBoxWidget* initTextBox(fw::Application& application, const std::string& test_name, float width, float height);

};
