#include "test_lib/test.h"

class DataPointerTests : public test::TestModule {
public:
	DataPointerTests(test::TestManager& manager, const std::vector<TestModule*>& required_modules = { });
	void createTestLists() override;

private:
	void nullTest(test::Test& test);
	void basicTest(test::Test& test);
	void structTest(test::Test& test);
	void destructorTest(test::Test& test);
	void derivedDestructorTest(test::Test& test);
	void vectorDestructorTest(test::Test& test);
	void customDeleterTest(test::Test& test);
	void lambdaDeleterTest(test::Test& test);
	void getTest(test::Test& test);
	void releaseTest(test::Test& test);
	void resetTest(test::Test& test);
	void resetDeleterTest(test::Test& test);
	void releaseSilentTest(test::Test& test);
	void resetSilentTest(test::Test& test);
	void moveConstructorTest(test::Test& test);
	void moveConstructorDeleterTest(test::Test& test);
	void moveConstructorDerivedTest(test::Test& test);
	void moveConstructorDerivedDeleterTest(test::Test& test);
	void moveAssignmentTest(test::Test& test);
	void moveAssignmentDeleterTest(test::Test& test);
	void moveAssignmentDerivedTest(test::Test& test);
	void moveAssignmentDerivedDeleterTest(test::Test& test);
	void swapTest(test::Test& test);
	void dereferenceTest(test::Test& test);
	void pointerAccessTest(test::Test& test);
	void moveTest(test::Test& test);
	void makeDataPointerTest(test::Test& test);
	void makeDataPointerDerivedTest(test::Test& test);
	void blockNameTest(test::Test& test);
	
	void checkDataBlock(test::Test& test, void* p_block, size_t p_size);
	void checkNoDataBlock(test::Test& test, void* p_block);

};
