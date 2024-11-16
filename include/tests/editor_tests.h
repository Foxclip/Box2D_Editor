#include "test_lib/test.h"
#include "editor/editor.h"

class EditorTests : public test::TestModule {
public:
	EditorTests(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });

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
	void panMoveTest(test::Test& test);
	void serializeEmptyTest(test::Test& test);

	void clickMouse(Editor& editor, const sf::Vector2f& pos);
	void clickObject(Editor& editor, GameObject* object, bool shift = false, bool ctrl = false);
	void tapKey(Editor& editor, sf::Keyboard::Key key);

};
