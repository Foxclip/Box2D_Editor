#pragma once

#include "widgets/widgets.h"
#include "test.h"

class WidgetTests : public test::TestModule {
public:
	WidgetTests(test::TestManager& manager);
	void createTestLists() override;

protected:
	void createApplicationList();

private:

};
