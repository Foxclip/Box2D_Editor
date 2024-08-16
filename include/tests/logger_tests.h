#pragma once

#include "common/logger.h"
#include "test.h"

class LoggerTests : public test::TestModule {
public:
	LoggerTests(test::TestManager& manager, const std::vector<TestModule*>& required_modules = { });
	void createTestLists() override;

protected:
	void createLoggerList(test::TestList* list);
	void createTagsList(test::TestList* list);

};
