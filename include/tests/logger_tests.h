#pragma once

#include "common/logger.h"
#include "test_lib/test.h"

class LoggerTests : public test::TestModule {
public:
	LoggerTests(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });

protected:
	void createLoggerList(test::TestModule* list);
	void createTagsList(test::TestModule* list);

};
