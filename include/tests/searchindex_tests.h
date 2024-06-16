#pragma once

#include "common/searchindex.h"
#include "test.h"

class SearchIndexTests : public test::TestModule {
public:
	SearchIndexTests(test::TestManager& manager);
	void createTestLists() override;

protected:
	void createTestListUnique(test::TestList* list);
	void createTestListMultiple(test::TestList* list);

};
