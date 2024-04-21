#include "tests/logger_tests.h"

LoggerTests::LoggerTests(test::TestManager& manager) : TestModule("Logger", manager) { }

void LoggerTests::createTestLists() {
	createLoggerList();
	createTagsList();
}

void LoggerTests::createLoggerList() {
	test::TestList* list = createTestList("Logger");
	list->OnBeforeRunTest = []() { logger.lock(); };
	list->OnAfterRunTest = []() { logger.unlock(); };

	test::Test* basic_test = list->addTest("basic", [&](test::Test& test) {
		Logger logger(true);
		logger << "Test\n";
		tCompare(logger.getTotalBuffer(), "Test\n");
	});
	test::Test* multiple_lines_test = list->addTest("multiple_lines", { basic_test }, [&](test::Test& test) {
		Logger logger(true);
		logger << "Line1\n";
		logger << "Line2\n";
		tCompare(logger.getTotalBuffer(), "Line1\nLine2\n");
	});
}

void LoggerTests::createTagsList() {
	test::TestList* list = createTestList("Tags");
	list->OnBeforeRunTest = []() { logger.lock(); };
	list->OnAfterRunTest = []() { logger.unlock(); };

	test::Test* tag_test = list->addTest("tag", [&](test::Test& test) {
		Logger logger(true);
		LoggerTag tag1(logger, "tag1");
		logger << "tag1\n";
		tCompare(logger.getTotalBuffer(), "tag1\n");
	});
	test::Test* deactivate_test = list->addTest("deactivate", { tag_test }, [&](test::Test& test) {
		Logger logger(true);
		LoggerDeactivate deact(logger);
		LoggerTag tag1(logger, "tag1");
		logger << "tag1\n";
		tCompare(logger.getTotalBuffer(), "");
	});
	test::Test* enable_tag_test = list->addTest("enable_tag", { deactivate_test }, [&](test::Test& test) {
		Logger logger(true);
		LoggerDeactivate deact(logger);
		LoggerEnableTag enable_tag1(logger, "tag1");
		LoggerEnableTag enable_someTag(logger, "someTag");
		LoggerDisableTag disable_someOtherTag(logger, "someOtherTag");
		{
			LoggerTag tag1(logger, "tag1");
			logger << "tag1\n";
		}
		{
			LoggerTag tag2(logger, "tag2");
			logger << "tag2\n";
		}
		tCompare(logger.getTotalBuffer(), "tag1\n");
	});
	test::Test* disable_tag_test = list->addTest("disable_tag", { tag_test }, [&](test::Test& test) {
		Logger logger(true);
		LoggerDisableTag disable_tag2(logger, "tag2");
		LoggerEnableTag enable_someTag(logger, "someTag");
		LoggerDisableTag disable_someOtherTag(logger, "someOtherTag");
		{
			LoggerTag tag1(logger, "tag1");
			logger << "tag1\n";
		}
		{
			LoggerTag tag2(logger, "tag2");
			logger << "tag2\n";
		}
		tCompare(logger.getTotalBuffer(), "tag1\n");
	});

	std::vector<test::Test*> tag_tests = list->getTestList();

	test::Test* return_disabled_test = list->addTest("return_disabled", { tag_tests }, [&](test::Test& test) {
		Logger logger(true);
		LoggerDisableTag disable_tag1(logger, "tag1");
		LoggerTag tag1(logger, "tag1");
		logger << "tag1 frist\n";
		{
			LoggerTag tag2(logger, "tag2");
			logger << "tag2\n";
		}
		logger << "tag1 second\n";
		tCompare(logger.getTotalBuffer(), "tag2\n");
	});
	test::Test* nested_tags_1_test = list->addTest("nested_tags_1", { tag_tests }, [&](test::Test& test) {
		Logger logger(true);
		LoggerDeactivate deact(logger);
		LoggerEnableTag enable_tag1(logger, "tag1");
		LoggerEnableTag enable_tag3(logger, "tag3");
		{
			LoggerTag tag1(logger, "tag1");
			logger << "tag1\n";
			{
				LoggerTag tag2(logger, "tag2");
				logger << "tag2\n";
				{
					LoggerTag tag3(logger, "tag3");
					logger << "tag3\n";
				}
			}
		}
		tCompare(logger.getTotalBuffer(), "tag1\ntag3\n");
	});
	test::Test* reenable_tag_1_test = list->addTest("reenable_tag_1", { tag_tests }, [&](test::Test& test) {
		Logger logger(true);
		{
			LoggerTag tag1(logger, "tag1");
			logger << "tag1\n";
			{
				LoggerDisableTag disable_tag2(logger, "tag2");
				LoggerTag tag2(logger, "tag2");
				logger << "tag2 first\n";
			}
			{
				LoggerTag tag2(logger, "tag2");
				logger << "tag2 second\n";
			}
		}
		tCompare(logger.getTotalBuffer(), "tag1\ntag2 second\n");
	});
	test::Test* disable_after_tag_test = list->addTest("disable_after_tag", { tag_tests }, [&](test::Test& test) {
		Logger logger(true);
		{
			LoggerTag tag1(logger, "tag1");
			logger << "tag1 first\n";
			LoggerDisableTag disable_tag1(logger, "tag1");
			logger << "tag1 second\n";
			{
				LoggerTag tag2(logger, "tag2");
				logger << "tag2\n";
			}
			logger << "tag1 third\n";
		}
		tCompare(logger.getTotalBuffer(), "tag1 first\ntag2\n");
	});
	test::Test* reenable_tag_2_test = list->addTest("reenable_tag_2", { tag_tests }, [&](test::Test& test) {
		Logger logger(true);
		{
			{
				LoggerTag tag1(logger, "tag1");
				{
					LoggerDisableTag disable_tag1(logger, "tag1");
					LoggerTag tag2(logger, "tag2");
				}
				logger << "tag1 first\n";
			}
			{
				LoggerTag tag1(logger, "tag1");
				{
					LoggerTag tag2(logger, "tag2");
					LoggerDisableTag disable_tag1(logger, "tag1");
				}
				logger << "tag1 second\n";
			}
		}
		tCompare(logger.getTotalBuffer(), "tag1 first\ntag1 second\n");
	});
	test::Test* nested_tags_2_test = list->addTest("nested_tags_2", { tag_tests }, [&](test::Test& test) {
		Logger logger(true);
		LoggerTag tag1(logger, "tag1");
		LoggerDisableTag disable_tag2(logger, "tag2");
		logger << "tag1 first\n";
		{
			LoggerTag tag2(logger, "tag2");
			logger << "tag2 1\n";
			{
				LoggerTag tag3(logger, "tag3");
				logger << "tag3 1\n";
				{
					LoggerTag tag2(logger, "tag2");
					logger << "tag2 2\n";
					{
						LoggerTag tag4(logger, "tag4");
						logger << "tag4 1\n";
						{
							LoggerTag tag1_again(logger, "tag1");
							logger << "tag1 second\n";
						}
						logger << "tag4 2\n";
					}
					logger << "tag2 3\n";
				}
				logger << "tag3 2\n";
			}
			logger << "tag2 4\n";
		}
		logger << "tag1 third\n";
		tCompare(logger.getTotalBuffer(),
			"tag1 first\n"
			"tag3 1\n"
			"tag4 1\n"
			"tag1 second\n"
			"tag4 2\n"
			"tag3 2\n"
			"tag1 third\n"
		);
	});
}
