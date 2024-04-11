#pragma once

#include "logger.h"
#include "test.h"

class LoggerTests : public test::TestModule {
public:
	LoggerTests(test::TestManager& manager);
	void createTestLists() override;

protected:
};
