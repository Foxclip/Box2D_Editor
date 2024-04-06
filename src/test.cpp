#include "test.h"
#include "logger.h"
#include <cassert>
#include <algorithm>

namespace test {

	Test::Test(std::string name, TestFuncType func) {
		this->name = name;
		this->func = func;
	}

	Test::Test(std::string name, std::vector<Test*> required, TestFuncType func)
	: Test(name, func) {
		this->required = required;
	}

	bool Test::run() {
		if (!std::all_of(required.begin(), required.end(), [](Test* test) {
			return test->result;
		})) {
			cancelled = true;
			return false;
		}
		result = true;
		try {
			func(*this);
		} catch (std::exception exc) {
			errors.push_back(Error("EXCEPTION: " + std::string(exc.what())));
			result = false;
		}
		return result;
	}

	Test::Error::Error(const std::string& str) {
		this->str = str;
	}

	Test::Error::Error(const std::string& str, const std::vector<Error>& subentries) {
		this->str = str;
		this->subentries = subentries;
	}

	void Test::Error::add(const Error& error) {
		subentries.push_back(error);
	}

	void Test::Error::log() const {
		logger << str << "\n";
		LoggerIndent subentries_indent;
		for (const Error& subentry : subentries) {
			subentry.log();
		}
	}

	TestList::TestList(const std::string& name) {
		this->name = name;
	}

	Test* TestList::addTest(std::string name, TestFuncType func) {
		return addTest(name, { }, func);
	}

	Test* TestList::addTest(std::string name, std::vector<Test*> required, TestFuncType func) {
		std::unique_ptr<Test> uptr = std::make_unique<Test>(name, required, func);
		Test* ptr = uptr.get();
		test_list.push_back(std::move(uptr));
		return ptr;
	}

	std::vector<Test*> TestList::getTestList() const {
		std::vector<Test*> result;
		for (size_t i = 0; i < test_list.size(); i++) {
			result.push_back(test_list[i].get());
		}
		return result;
	}

	void TestList::runTests() {
		LoggerIndent test_list_indent;
		for (size_t i = 0; i < test_list.size(); i++) {
			Test* test = test_list[i].get();
			bool result = test->run();
			std::string result_str;
			if (result) {
				logger << "passed: " << test->name << "\n";
				passed_list.push_back(test->name);
			} else {
				if (test->cancelled) {
					logger << "cancelled: " << test->name << "\n";
					cancelled_list.push_back(test->name);
				} else {
					logger << "FAILED: " << test->name << "\n";
					LoggerIndent errors_indent;
					if (!test->errors.empty()) {
						for (const Test::Error& error : test->errors) {
							error.log();
						}
					} else {
						logger << "<empty>" << "\n";
					}
					failed_list.push_back(test->name);
				}
			}
		}
		logger << "Passed " << passed_list.size() << " tests, "
			<< "cancelled " << cancelled_list.size() << " tests, "
			<< "failed " << failed_list.size() << " tests";
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

	TestModule::TestModule(const std::string& name) {
		this->name = name;
	}

	TestList* TestModule::createTestList(const std::string& name) {
		std::unique_ptr<TestList> test_list = std::make_unique<TestList>(name);
		TestList* ptr = test_list.get();
		test_lists.push_back(std::move(test_list));
		return ptr;
	}

	void TestModule::runTests() {
		createTestLists();
		logger << "Running test module: " << name << "\n";
		LoggerIndent test_module_indent;
		for (auto& test_list : test_lists) {
			logger << "Running test list: " << test_list->name << "\n";
			test_list->runTests();
			for (const std::string& name : test_list->passed_list) {
				passed_list.push_back(test_list->name + "/" + name);
			}
			for (const std::string& name : test_list->cancelled_list) {
				cancelled_list.push_back(test_list->name + "/" + name);
			}
			for (const std::string& name : test_list->failed_list) {
				failed_list.push_back(test_list->name + "/" + name);
			}
		}
		logger << "Passed " << passed_list.size() << " tests, "
			<< "cancelled " << cancelled_list.size() << " tests, "
			<< "failed " << failed_list.size() << " tests";
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

	void TestModule::testAssert(Test& test, bool value, const std::string& value_message) {
		if (!value) {
			test.errors.push_back(Test::Error("Failed condition: " + value_message));
			test.result = false;
		}
	}

	void TestModule::testAssert(Test& test, bool value, const std::string& value_message, const std::string message) {
		if (!value) {
			test.errors.push_back(Test::Error(message + ": " + value_message));
			test.result = false;
		}
	}

	void TestManager::runAllModules() {
		std::vector<std::string> passed_list;
		std::vector<std::string> cancelled_list;
		std::vector<std::string> failed_list;
		for (size_t i = 0; i < modules.size(); i++) {
			TestModule* module = modules[i].get();
			module->runTests();
			for (const std::string& name : module->passed_list) {
				passed_list.push_back(module->name + "/" + name);
			}
			for (const std::string& name : module->cancelled_list) {
				cancelled_list.push_back(module->name + "/" + name);
			}
			for (const std::string& name : module->failed_list) {
				failed_list.push_back(module->name + "/" + name);
			}
		}
		logger << "Passed " << passed_list.size() << " tests, "
			<< "cancelled " << cancelled_list.size() << " tests, "
			<< "failed " << failed_list.size() << " tests";
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

}
