#pragma once

#include "compvector.h"
#include "test.h"

class CompVectorTests : public test::TestModule {
public:
	CompVectorTests();

protected:
	void createTestLists() override;
	void createCompVectorList(test::TestList* list);
	void createCompVectorUptrList(test::TestList* list);
};
