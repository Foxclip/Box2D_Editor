#pragma once

#include <vector>
#include <functional>
#include <string>
#include <memory>

namespace test {

	class Test;
	using TestFuncType = std::function<void(Test& test)>;

#define tCheck(value, ...) \
	testAssert(test, value, #value, __VA_ARGS__)

#define tCompare(actual, expected) \
	testCompare(test, #actual, actual, expected);

#define tApproxCompare(actual, expected, ...) \
	testApproxCompare(test, #actual, actual, expected, __VA_ARGS__)

#define tAssert(value, ...) \
	{ \
		bool v = value; \
		testAssert(test, v, #value, __VA_ARGS__); \
		if (!v) { \
			return; \
		} \
	}

	class Test {
	public:
		struct Error {
			std::string str;
			std::vector<Error> subentries;
			explicit Error(const std::string& str);
			explicit Error(const std::string& str, const std::vector<Error>& subentries);
			void add(const Error& error);
			void log() const;
		};
		std::string name = "<unnamed>";
		std::vector<Error> errors;
		std::vector<Test*> required;
		bool result = false;
		bool cancelled = false;

		Test(std::string name, TestFuncType func);
		Test(std::string name, std::vector<Test*> required, TestFuncType func);
		bool run();

	private:
		TestFuncType func;
	};

	class TestList {
	public:
		std::string name;
		std::vector<std::string> passed_list;
		std::vector<std::string> cancelled_list;
		std::vector<std::string> failed_list;

		TestList(const std::string& name);
		Test* addTest(std::string name, TestFuncType func);
		Test* addTest(std::string name, std::vector<Test*> required, TestFuncType func);
		std::vector<Test*> getTestList() const;
		void runTestList();

	protected:
		std::vector<std::unique_ptr<Test>> test_list;

		void testAssert(Test& test, bool value, const std::string& value_message);
		void testAssert(Test& test, bool value, const std::string& value_message, const std::string message);
		template<typename T>
		void testCompare(Test& test, const std::string& name, T actual, T expected);
		template<typename T>
		void testApproxCompare(Test& test, const std::string& name, T actual, T expected, T epsilon = 0.001f);
		template<typename T>
		static bool equals(T left, T right, T epsilon = 0.001f);

	private:
		template<typename T>
		void compareFail(Test& test, const std::string& name, T actual, T expected);

	};

	class TestModule {
	public:
		std::string name;
		std::vector<TestList> test_lists;
		std::vector<std::string> passed_list;
		std::vector<std::string> cancelled_list;
		std::vector<std::string> failed_list;

		TestModule(const std::string& name);
		void runModuleTests();

	protected:
		virtual void createTestLists() = 0;

	private:

	};

	class TestManager {
	public:
		TestManager();
		void addModule(std::unique_ptr<TestModule> module);
		void runAllModules();

	private:
		std::vector<std::unique_ptr<TestModule>> modules;

	};

	template<typename T>
	inline void TestList::testCompare(Test& test, const std::string& name, T actual, T expected) {
		if (actual != expected) {
			compareFail(test, name, actual, expected);
		}
	}

	template<typename T>
	inline void TestList::testApproxCompare(Test& test, const std::string& name, T actual, T expected, T epsilon) {
		if (!equals(actual, expected, epsilon)) {
			compareFail(test, name, actual, expected);
		}
	}

	template<typename T>
	inline bool TestList::equals(T left, T right, T epsilon) {
		return abs(left - right) < epsilon;
	}

	template<typename T>
	inline void TestList::compareFail(Test& test, const std::string& name, T actual, T expected) {
		std::string actual_value_str;
		std::string expected_value_str;
		if constexpr (std::is_same_v<T, std::string>) {
			actual_value_str = actual;
			expected_value_str = expected;
		} else {
			expected_value_str = std::to_string(expected);
			actual_value_str = std::to_string(actual);
		}
		Test::Error error(name);
		error.add(Test::Error("Expected value: " + expected_value_str));
		error.add(Test::Error("  Actual value: " + actual_value_str));
		test.errors.push_back(error);
		test.result = false;
	}

}
