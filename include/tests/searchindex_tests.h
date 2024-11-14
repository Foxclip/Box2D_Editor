#pragma once

#include "common/searchindex.h"
#include "test_lib/test.h"

class SearchIndexTests : public test::TestModule {
public:
	SearchIndexTests(test::TestManager& manager, const std::vector<TestModule*>& required_modules = { });
	void createTestLists() override;

protected:
	void createTestListUnique(test::TestList* list);
	void createTestListMultiple(test::TestList* list);

};
