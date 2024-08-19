#include "tests/test.h"
#include "editor/editor.h"

class EditorTests : public test::TestModule {
public:
	EditorTests(test::TestManager& manager, const std::vector<TestModule*>& required_modules = { });
	void createTestLists() override;

private:
	sf::RenderWindow window;
	fw::Font font;

	void beforeRunModule() override;
	void afterRunModule() override;

	void basicTest(test::Test& test);
	void initTest(test::Test& test);
	void advanceTest(test::Test& test);

};
