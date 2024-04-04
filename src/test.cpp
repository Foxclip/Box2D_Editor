#include "test.h"
#include "logger.h"
#include <cassert>

namespace test {

	Test::Test(std::string name, TestFuncType func) {
		this->name = name;
		this->func = func;
	}

	bool Test::run() {
		try {
			func(*this);
		} catch (std::exception exc) {
			errors.push_back("EXCEPTION: " + std::string(exc.what()));
			result = false;
		}
		return result;
	}

	TestList::TestList(const std::string& name) {
		this->name = name;
	}

	void TestList::runTests() {
		logger << "Running tests: " << name << "\n";
		size_t passed_count = 0;
		size_t failed_count = 0;
		std::vector<std::string> failed_list;
		LoggerIndent test_indent;
		for (Test& test : test_list) {
			bool result = test.run();
			logger << (result ? "passed: " : "FAILED: ") << test.name << "\n";
			if (result) {
				passed_count++;
			} else {
				LoggerIndent errors_indent;
				if (!test.errors.empty()) {
					for (const std::string& error : test.errors) {
						logger << error << "\n";
					}
				} else {
					logger << "<empty>" << "\n";
				}
				failed_list.push_back(test.name);
				failed_count++;
			}
		}
		logger << "Passed " << passed_count << " tests, failed " << failed_count << " tests";
		if (failed_list.size() > 0) {
			logger << ":\n";
			LoggerIndent failed_list_indent;
			for (const std::string& name : failed_list) {
				logger << name << "\n";
			}
		} else {
			logger << "\n";
		}
	}

	void TestList::addTest(std::string name, TestFuncType func) {
		Test test(name, func);
		test_list.push_back(test);
	}

	void TestList::testAssert(Test& test, bool value, const std::string& value_message) {
		if (!value) {
			test.errors.push_back("Failed condition: " + value_message);
			test.result = false;
		}
	}

	void TestList::testAssert(Test& test, bool value, const std::string& value_message, const std::string message) {
		if (!value) {
			test.errors.push_back(message + ": " + value_message);
			test.result = false;
		}
	}

}
