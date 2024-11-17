#include "widgets/widget.h"
#include "test_lib/test.h"

class WidgetTestsContainer : public test::TestModule {
public:
	WidgetTestsContainer(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void containerWidgetBasicTest(test::Test& test);
	void containerWidgetChildrenTest(test::Test& test);
	void containerWidgetPaddingTest(test::Test& test);

private:
	sf::RenderWindow& getWindow();
	fw::Font& getFont();

};
