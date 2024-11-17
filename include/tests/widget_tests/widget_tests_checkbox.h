#include "widgets/widget.h"
#include "test_lib/test.h"

class WidgetTestsCheckbox : public test::TestModule {
public:
	WidgetTestsCheckbox(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void checkboxWidgetBasicTest(test::Test& test);
	void checkboxWidgetToggleTest(test::Test& test);

private:
	sf::RenderWindow& getWindow();
	fw::Font& getFont();

};
