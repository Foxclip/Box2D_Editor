#include "tests/test.h"
#include "editor/editor.h"

class EditorTests : public test::TestModule {
public:
	EditorTests(test::TestManager& manager, const std::vector<TestModule*>& required_modules = { });
	void createTestLists() override;

private:
	void basicTest(test::Test& test);

};
