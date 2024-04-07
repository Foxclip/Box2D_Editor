#pragma once

#include "logger.h"
#include "test.h"

class LoggerTests : public test::TestModule {
public:
	LoggerTests();

protected:
	void createTestLists() override;
};
