#pragma once

#include <vector>
#include <functional>
#include <string>
#include <memory>
#include <filesystem>

namespace test {

	class Test;
	using TestFuncType = std::function<void(Test& test)>;

#define tCheck(value, ...) \
	testAssert(test, __FILE__, __LINE__, value, #value, __VA_ARGS__)

#define tCompare(actual, expected, ...) \
	testCompare(test, __FILE__, __LINE__, #actual, actual, expected, __VA_ARGS__);

#define tApproxCompare(actual, expected, ...) \
	testApproxCompare(test, __FILE__, __LINE__, #actual, actual, expected, __VA_ARGS__)

#define tAssert(value, ...) \
	{ \
		bool v = value; \
		testAssert(test, __FILE__, __LINE__, v, #value, __VA_ARGS__); \
		if (!v) { \
			return; \
		} \
	}

#define tAssertCompare(actual, expected) \
	{ \
		bool v = testCompare(test, __FILE__, __LINE__, #actual, actual, expected); \
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

		static std::string char_to_str(char c);
		static std::string char_to_esc(std::string str);
	};

	class TestList {
	public:
		std::string name;
		std::vector<std::string> passed_list;
		std::vector<std::string> cancelled_list;
		std::vector<std::string> failed_list;
		std::function<void(void)> OnBeforeRunAllTests = []() { };
		std::function<void(void)> OnAfterRunAllTests = []() { };
		std::function<void(void)> OnBeforeRunTest = []() { };
		std::function<void(void)> OnAfterRunTest = []() { };

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
		TestList* createTestList(const std::string& name);
		void runTests();
		static void printSummary(
			const std::vector<std::string>& passed_list,
			const std::vector<std::string>& cancelled_list,
			const std::vector<std::string>& failed_list
		);

	protected:
		virtual void createTestLists() = 0;
		virtual void beforeRunModule();
		virtual void afterRunModule();
		void testAssert(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message);
		void testAssert(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message, const std::string message);
		template<typename T1, typename T2>
		bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected);
		template<typename T1, typename T2, typename TStr>
		bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str);
		template<typename T>
		bool testApproxCompare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected, T epsilon = 0.0001f);
		template<typename T>
		static bool equals(T left, T right, T epsilon = 0.001f);

	private:
		template<typename T1, typename T2, typename TStr>
		void compareFail(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str);

	};

	class TestManager {
	public:
		template<typename T>
		requires std::derived_from<T, TestModule>
		void addModule();
		void runAllModules();

	private:
		std::vector<std::unique_ptr<TestModule>> modules;

	};

	template<typename T1, typename T2>
	inline bool TestModule::testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected) {
		if constexpr (std::same_as<T1, std::string> || std::same_as<T1, const char*>) {
			auto func = [](const T1& val) { return val; };
			return testCompare(test, file, line, name, actual, expected, func);
		} else {
			auto func = [](const T1& val) { return std::to_string(val); };
			return testCompare(test, file, line, name, actual, expected, func);
		}
	}

	template<typename T1, typename T2, typename TStr>
	inline bool TestModule::testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str) {
		if (actual != expected) {
			compareFail(test, file, line, name, actual, expected, to_str);
			return false;
		}
		return true;
	}

	template<typename T>
	inline bool TestModule::testApproxCompare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected, T epsilon) {
		if (!equals(actual, expected, epsilon)) {
			auto func = [](const T& val) { return std::to_string(val); };
			compareFail(test, file, line, name, actual, expected, func);
			return false;
		}
		return true;
	}

	template<typename T>
	inline bool TestModule::equals(T left, T right, T epsilon) {
		return abs(left - right) < epsilon;
	}

	template<typename T1, typename T2, typename TStr>
	inline void TestModule::compareFail(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str) {
		std::string filename = std::filesystem::path(file).filename().string();
		std::string location_str = "[" + filename + ":" + std::to_string(line) + "]";
		Test::Error error(name + " " + location_str);
		error.add(Test::Error("Expected value: " + to_str(expected)));
		error.add(Test::Error("  Actual value: " + to_str(actual)));
		test.errors.push_back(error);
		test.result = false;
	}

	template<typename T>
	requires std::derived_from<T, TestModule>
	inline void TestManager::addModule() {
		std::unique_ptr<T> uptr = std::make_unique<T>();
		modules.push_back(std::move(uptr));
	}

}
