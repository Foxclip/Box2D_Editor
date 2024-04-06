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

#define tAssertCompare(actual, expected) \
	{ \
		bool v = testCompare(test, #actual, actual, expected); \
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
		void runTests();

	protected:
		std::vector<std::unique_ptr<Test>> test_list;

	private:

	};

	class TestModule {
	public:
		std::string name;
		std::vector<std::unique_ptr<TestList>> test_lists;
		std::vector<std::string> passed_list;
		std::vector<std::string> cancelled_list;
		std::vector<std::string> failed_list;

		TestModule(const std::string& name);
		void runTests();

	protected:
		virtual void createTestLists() = 0;
		void testAssert(Test& test, bool value, const std::string& value_message);
		void testAssert(Test& test, bool value, const std::string& value_message, const std::string message);
		template<typename T, typename U>
		bool testCompare(Test& test, const std::string& name, T actual, U expected);
		template<typename T>
		bool testApproxCompare(Test& test, const std::string& name, T actual, T expected, T epsilon = 0.001f);
		template<typename T>
		static bool equals(T left, T right, T epsilon = 0.001f);

	private:
		template<typename T, typename U>
		void compareFail(Test& test, const std::string& name, T actual, U expected);

	};

	class TestManager {
	public:
		TestManager();
		void addModule(std::unique_ptr<TestModule> module);
		void runAllModules();

	private:
		std::vector<std::unique_ptr<TestModule>> modules;

	};

	template<typename T, typename U>
	inline bool TestModule::testCompare(Test& test, const std::string& name, T actual, U expected) {
		if (actual != expected) {
			compareFail(test, name, actual, expected);
			return false;
		}
		return true;
	}

	template<typename T>
	inline bool TestModule::testApproxCompare(Test& test, const std::string& name, T actual, T expected, T epsilon) {
		if (!equals(actual, expected, epsilon)) {
			compareFail(test, name, actual, expected);
			return false;
		}
		return true;
	}

	template<typename T>
	inline bool TestModule::equals(T left, T right, T epsilon) {
		return abs(left - right) < epsilon;
	}

	template<typename T, typename U>
	inline void TestModule::compareFail(Test& test, const std::string& name, T actual, U expected) {
		std::string actual_value_str;
		std::string expected_value_str;
		if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>) {
			actual_value_str = actual;
		} else {
			actual_value_str = std::to_string(actual);
		}
		if constexpr (std::is_same_v<U, std::string> || std::is_same_v<U, const char*>) {
			expected_value_str = expected;
		} else {
			expected_value_str = std::to_string(expected);
		}
		Test::Error error(name);
		error.add(Test::Error("Expected value: " + expected_value_str));
		error.add(Test::Error("  Actual value: " + actual_value_str));
		test.errors.push_back(error);
		test.result = false;
	}

}
