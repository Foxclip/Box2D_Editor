#include "widgets/widget.h"
#include "test_lib/test.h"

class WidgetTestsDropdown: public test::TestModule {
public:
	WidgetTestsDropdown(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void dropdownWidgetBasicTest(test::Test& test);
	void dropdownWidgetOptions1Test(test::Test& test);
	void dropdownWidgetOptions2Test(test::Test& test);

private:
	sf::RenderWindow& getWindow();
	fw::Font& getFont();

};