#pragma once

#include "searchindex.h"
#include "test.h"

class SearchIndexTests : public test::TestModule {
public:
	SearchIndexTests();

protected:
	void createTestLists() override;
	void createTestListUnique(test::TestList* list);
	void createTestListMultiple(test::TestList* list);

};
