#pragma once

#include "common/searchindex.h"
#include "test_lib/test.h"

class SearchIndexTests : public test::TestModule {
public:
	SearchIndexTests(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });

protected:
	void createTestListUnique(test::TestModule* list);
	void createTestListMultiple(test::TestModule* list);

};
