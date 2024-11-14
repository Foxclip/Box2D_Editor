#include "test_lib/test.h"
#include "common/logger.h"
#include <cassert>
#include <algorithm>

namespace test {

	Test::Test(std::string name, TestFuncType func) {
		this->name = name;
		this->func = func;
		root_error = std::make_unique<Error>("root", Error::Type::Root);
		error_stack.push(root_error.get());
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
			getCurrentError()->add("EXCEPTION: " + std::string(exc.what()));
			result = false;
		}
		return result;
	}

	Test::Error* Test::getCurrentError() const {
		return error_stack.top();
	}

	std::string Test::char_to_str(char c) {
		if (c < -1) {
			return "(" + std::to_string(c) + ")";
		} else if (c == '\n') {
			return "\\n";
		} else if (c == '\r') {
			return "\\r";
		} else if (c == '\t') {
			return "\\t";
		} else if (c == '\0') {
			return "\\0";
		} else if (c == '\\') {
			return "\\\\";
		} else if (c == '"') {
			return "\\\"";
		} else if (c == EOF) {
			return "\\(EOF)";
		} else {
			return std::string(1, c);
		}
	}

	std::string Test::char_to_esc(std::string str, bool convert_quotes) {
		std::string result;
		for (size_t i = 0; i < str.size(); i++) {
			char current_char = str[i];
			if (!convert_quotes && current_char == '"') {
				result += current_char;
			} else {
				result += char_to_str(current_char);
			}
		}
		return result;
	}

	Test::Error::Error(const std::string& str, Type type) {
		this->str = str;
		this->type = type;
	}

	Test::Error* Test::Error::add(const std::string& message, Type type) {
		std::unique_ptr<Error> uptr = std::make_unique<Error>(message, type);
		Error* ptr = uptr.get();
		subentries.push_back(std::move(uptr));
		return ptr;
	}

	void Test::Error::log() const {
		if (type == Type::Container) {
			std::function<bool(const Error& error)> has_non_container_subentries = [&](const Error& error) {
				for (auto& subentry : error.subentries) {
					if (subentry->type != Type::Container) {
						return true;
					} else {
						if (has_non_container_subentries(*subentry)) {
							return true;
						}
					}
				}
				return false;
			};
			if (!has_non_container_subentries(*this)) {
				return;
			}
		}
		if (type != Type::Root) {
			std::string esc_str = char_to_esc(str, false);
			logger << esc_str << "\n";
		}
		if (subentries.size() > 0) {
			LoggerIndent subentries_indent(type != Type::Root);
			for (auto& subentry : subentries) {
				subentry->log();
			}
		}
	}

	ErrorContainer::ErrorContainer(Test& test, const std::string& file, size_t line, const std::string& message) : test(test) {
		std::string filename = std::filesystem::path(file).filename().string();
		std::string location_str = "[" + filename + ":" + std::to_string(line) + "]";
		std::string space_str = message.size() > 0 ? " " : "";
		Test::Error* error = test.getCurrentError()->add(message + space_str + location_str, Test::Error::Type::Container);
		test.error_stack.push(error);
	}

	ErrorContainer::~ErrorContainer() {
		close();
	}

	void ErrorContainer::close() {
		test.error_stack.pop();
	}

	TestList::TestList(const std::string& name, TestModule& module, const std::vector<TestList*>& required_lists) : module(module) {
		this->name = name;
		this->required_lists = required_lists;
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
		OnBeforeRunAllTests();
		for (auto& test : test_list) {
			std::string spacing_str;
			size_t spacing_size = module.manager.max_test_name - test->name.size();
			for (size_t i = 0; i < spacing_size; i++) {
				spacing_str += "-";
			}
			logger << test->name << spacing_str << "|" << LoggerFlush();
			Logger::disableStdWrite();
			logger.manualDeactivate();
			OnBeforeRunTest();
			bool result = test->run();
			OnAfterRunTest();
			logger.manualActivate();
			Logger::enableStdWrite();
			if (result) {
				logger << "passed" << "\n";
				passed_list.push_back(test->name);
			} else {
				if (test->cancelled) {
					logger << "cancelled" << "\n";
					cancelled_list.push_back(test->name);
				} else {
					logger << "FAILED" << "\n";
					LoggerIndent errors_indent;
					test->root_error->log();
					failed_list.push_back(test->name);
				}
			}
		}
		OnAfterRunAllTests();
		is_run = true;
	}

	TestModule::TestModule(
		const std::string& name,
		TestManager& manager,
		const std::vector<TestModule*>& required_modules
	) : manager(manager) {
		this->name = name;
		this->required_modules = required_modules;
	}

	TestList* TestModule::createTestList(const std::string& name, const std::vector<TestList*>& required_lists) {
		std::unique_ptr<TestList> test_list = std::make_unique<TestList>(name, *this, required_lists);
		TestList* ptr = test_list.get();
		test_lists.push_back(std::move(test_list));
		return ptr;
	}

	std::vector<Test*> TestModule::getTestList() const {
		std::vector<Test*> result;
		for (size_t i = 0; i < test_lists.size(); i++) {
			TestList* list = test_lists[i].get();
			std::vector<Test*> list_tests = list->getTestList();
			result.insert(result.end(), list_tests.begin(), list_tests.end());
		}
		return result;
	}

	void TestModule::runTests() {
		beforeRunModule();
		for (auto& test_list : test_lists) {
			logger << "List: " << test_list->name << "\n";
			LoggerIndent test_list_indent;
			bool cancelled = false;
			for (TestList* req_list : test_list->required_lists) {
				if (!req_list->is_run || req_list->failed_list.size() > 0) {
					cancelled = true;
					break;
				}
			}
			if (cancelled) {
				std::vector<Test*> tests = test_list->getTestList();
				for (Test* test : tests) {
					test->cancelled = true;
					test_list->cancelled_list.push_back(test->name);
				}
				logger << "Cancelled " << tests.size() << " tests\n";
			} else {
				test_list->runTests();
				if (manager.print_list_summary) {
					printSummary(
						test_list->passed_list,
						test_list->cancelled_list,
						test_list->failed_list
					);
				}
			}
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
		afterRunModule();
		if (manager.print_module_summary) {
			printSummary(passed_list, cancelled_list, failed_list);
		}
		is_run = true;
	}

	void TestModule::printSummary(
		const std::vector<std::string>& passed_list,
		const std::vector<std::string>& cancelled_list,
		const std::vector<std::string>& failed_list
	) {
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

	void TestModule::beforeRunModule() { }

	void TestModule::afterRunModule() { }

	void TestModule::testMessage(Test& test, const std::string& file, size_t line, const std::string& message) {
		std::string filename = std::filesystem::path(file).filename().string();
		std::string location_str = "[" + filename + ":" + std::to_string(line) + "]";
		test.getCurrentError()->add(message + " " + location_str);
	}

	bool TestModule::testCheck(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message) {
		if (!value) {
			std::string filename = std::filesystem::path(file).filename().string();
			std::string location_str = "[" + filename + ":" + std::to_string(line) + "]";
			test.getCurrentError()->add("Failed condition: " + value_message + " " + location_str);
			test.result = false;
		}
		return value;
	}

	bool TestModule::testCheck(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message, const std::string message) {
		if (!value) {
			std::string filename = std::filesystem::path(file).filename().string();
			std::string location_str = "[" + filename + ":" + std::to_string(line) + "]";
			test.getCurrentError()->add(message + ": " + value_message + " " + location_str);
			test.result = false;
		}
		return value;
	}

	void TestManager::runAllModules() {
		for (auto& module : modules) {
			module->createTestLists();
		}
		for (auto& module : modules) {
			for (auto& list : module->test_lists) {
				for (Test* test : list->getTestList()) {
					if (test->name.size() > max_test_name) {
						max_test_name = test->name.size();
					}
				}
			}
		}
		std::vector<std::string> passed_list;
		std::vector<std::string> cancelled_list;
		std::vector<std::string> failed_list;
		for (size_t i = 0; i < modules.size(); i++) {
			TestModule* module = modules[i].get();
			logger << "Module: " << module->name << "\n";
			LoggerIndent test_module_indent;
			bool cancelled = false;
			for (TestModule* req_list : module->required_modules) {
				if (!req_list->is_run || req_list->failed_list.size() > 0) {
					cancelled = true;
					break;
				}
			}
			if (cancelled) {
				std::vector<Test*> tests = module->getTestList();
				for (Test* test : tests) {
					test->cancelled = true;
					module->cancelled_list.push_back(test->name);
				}
				logger << "Cancelled " << tests.size() << " tests\n";
			} else {
				module->runTests();
			}
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
		TestModule::printSummary(passed_list, cancelled_list, failed_list);
		if (cancelled_list.empty() && failed_list.empty()) {
			logger << "ALL PASSED\n";
		}
	}

}
