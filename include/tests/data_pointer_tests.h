#include "test.h"

class DataPointerTests : public test::TestModule {
public:
	DataPointerTests(test::TestManager& manager, const std::vector<TestModule*>& required_modules = { });
	void createTestLists() override;

private:
	void nullTest(test::Test& test);
	void basicTest(test::Test& test);
	void structTest(test::Test& test);
	void customDeleterTest(test::Test& test);
	void getTest(test::Test& test);
	void releaseTest(test::Test& test);
	void resetTest(test::Test& test);
	void dereferenceTest(test::Test& test);
	void pointerAccessTest(test::Test& test);
	void moveTest(test::Test& test);

};
