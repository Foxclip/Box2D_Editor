#include "widgets/widget.h"
#include "test_lib/test.h"

class WidgetTestsSizePolicy : public test::TestModule {
public:
	WidgetTestsSizePolicy(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void sizePolicyTest(test::Test& test);
	void sizePolicyPositionTest(test::Test& test);
	void sizePolicyExpandTest(test::Test& test);
	void sizePolicyLimitsTest(test::Test& test);
	void sizePolicyCombinedTest(test::Test& test);

private:
	sf::RenderWindow& getWindow();
	fw::Font& getFont();

};
