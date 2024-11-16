#pragma once

#include "common/compvector.h"
#include "test_lib/test.h"

class CompVectorTests : public test::TestModule {
public:
	CompVectorTests(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { }); 

protected:
	void createCompVectorList(test::TestModule* list);
	void createCompVectorUptrList(test::TestModule* list);
};
