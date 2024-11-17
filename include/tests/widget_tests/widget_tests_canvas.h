#include "widgets/widget.h"
#include "test_lib/test.h"

class WidgetTestsCanvas : public test::TestModule {
public:
	WidgetTestsCanvas(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void canvasWidgetBasicTest(test::Test& test);
	void canvasWidgetDrawTest(test::Test& test);
	void canvasWidgetAlphaTest(test::Test& test);

private:
	sf::RenderWindow& getWindow();
	fw::Font& getFont();

};
