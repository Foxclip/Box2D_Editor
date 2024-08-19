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
	void panTest(test::Test& test);
	void selectTest(test::Test& test);
	void multiSelectTest(test::Test& test);
	void moveTest(test::Test& test);

	void clickMouse(Editor& editor, const sf::Vector2f& pos);
	void clickObject(Editor& editor, GameObject* object, bool shift = false, bool ctrl = false);
	void tapKey(Editor& editor, sf::Keyboard::Key key);

};
