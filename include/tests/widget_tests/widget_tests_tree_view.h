#include "widgets/widget.h"
#include "test_lib/test.h"

class WidgetTestsTreeView: public test::TestModule {
public:
	WidgetTestsTreeView(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void treeviewWidgetBasicTest(test::Test& test);
	void treeviewWidgetEntriesTest(test::Test& test);
	void treeviewWidgetParent1Test(test::Test& test);
	void treeviewWidgetParent2Test(test::Test& test);
	void treeviewWidgetSelectTest(test::Test& test);
	void treeviewWidgetReorderTest(test::Test& test);
	void treeviewWidgetRemoveTest(test::Test& test);
	void treeviewWidgetClearTest(test::Test& test);
	void treeviewWidgetDrag1Test(test::Test& test);
	void treeviewWidgetDrag2Test(test::Test& test);
	void treeviewWidgetDrag3Test(test::Test& test);

private:
	sf::RenderWindow& getWindow();
	fw::Font& getFont();
	static float calcTreeViewEntryHeight(fw::TreeViewEntry* entry);
	static float calcTreeViewHeight(fw::TreeViewWidget* treeview);
	static void checkTopEntries(test::Test& test, fw::TreeViewWidget* tree_view_widget, const std::initializer_list<fw::TreeViewEntry*>& entries);
	static void checkEntries(test::Test& test, fw::TreeViewEntry* entry, const std::initializer_list<fw::TreeViewEntry*> entries);
	static void dragEntry(fw::Application& application, fw::TreeViewEntry* entry, const sf::Vector2f& pos, bool drop = false);

};
