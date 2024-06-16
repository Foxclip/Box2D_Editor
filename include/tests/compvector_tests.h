#pragma once

#include "common/compvector.h"
#include "test.h"

class CompVectorTests : public test::TestModule {
public:
	CompVectorTests(test::TestManager& manager);
	void createTestLists() override;

protected:
	void createCompVectorList(test::TestList* list);
	void createCompVectorUptrList(test::TestList* list);
};
