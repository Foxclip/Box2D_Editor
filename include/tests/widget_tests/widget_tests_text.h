#include "widgets/widget.h"
#include "test_lib/test.h"

class WidgetTestsText : public test::TestModule {
public:
	WidgetTestsText(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void textWidgetTest(test::Test& test);
	void textWidgetDefaultFontTest(test::Test& test);

private:
	sf::RenderWindow& getWindow();
	fw::Font& getFont();

};
