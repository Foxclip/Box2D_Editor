#pragma once

#include <vector>
#include <functional>
#include <string>
#include <memory>
#include <filesystem>
#include <stack>

namespace test {

	class Test;
	using TestFuncType = std::function<void(Test& test)>;

#define T_MESSAGE(message) \
	testMessage(test, __FILE__, __LINE__, message)

#define T_CHECK(value, ...) \
	testCheck(test, __FILE__, __LINE__, value, #value, __VA_ARGS__)

#define T_COMPARE(actual, expected, ...) \
	testCompare(test, __FILE__, __LINE__, #actual, actual, expected, __VA_ARGS__)

#define T_APPROX_COMPARE(actual, expected, ...) \
	testApproxCompare(test, __FILE__, __LINE__, #actual, actual, expected, __VA_ARGS__)

#define T_VEC2_APPROX_COMPARE(actual, expected, ...) \
	testVec2ApproxCompare(test, __FILE__, __LINE__, #actual, actual, expected, __VA_ARGS__)

#define T_ASSERT(expr) \
	if (!expr) { \
		return; \
	} \

#define T_ASSERT_NO_ERRORS() \
	if (!test.result) { \
		return; \
	} \

#define T_CONTAINER(message) \
	test::ErrorContainer error_container(test, __FILE__, __LINE__, message);

#define T_WRAP_CONTAINER(expr, ...) \
	{ \
		T_CONTAINER(#expr); \
		expr; \
	}

	class Test {
	public:
		struct Error {
			enum class Type {
				Root,
				Container,
				Normal,
			};
			Type type;
			std::string str;
			std::vector<std::unique_ptr<Error>> subentries;
			explicit Error(const std::string& str, Type type);
			Error* add(const std::string& message, Type type = Type::Normal);
			void log() const;
		};
		std::string name = "<unnamed>";
		std::vector<Test*> required;
		bool result = false;
		bool cancelled = false;
		std::unique_ptr<Error> root_error;

		Test(std::string name, TestFuncType func);
		Test(std::string name, std::vector<Test*> required, TestFuncType func);
		bool run();
		Error* getCurrentError() const;

	private:
		friend class ErrorContainer;
		TestFuncType func;
		std::stack<Error*> error_stack;

		static std::string char_to_str(char c);
		static std::string char_to_esc(std::string str, bool convert_quotes = true);
	};

	class ErrorContainer {
	public:
		ErrorContainer(Test& test, const std::string& file, size_t line, const std::string& message = "");
		~ErrorContainer();

	private:
		Test& test;

		void close();
	};

	class TestModule;

	class TestList {
	public:
		std::string name;
		std::vector<TestList*> required_lists;
		std::vector<std::string> passed_list;
		std::vector<std::string> cancelled_list;
		std::vector<std::string> failed_list;
		bool is_run = false;
		std::function<void(void)> OnBeforeRunAllTests = []() { };
		std::function<void(void)> OnAfterRunAllTests = []() { };
		std::function<void(void)> OnBeforeRunTest = []() { };
		std::function<void(void)> OnAfterRunTest = []() { };

		TestList(const std::string& name, TestModule& module, const std::vector<TestList*>& required_lists = { });
		Test* addTest(std::string name, TestFuncType func);
		Test* addTest(std::string name, std::vector<Test*> required, TestFuncType func);
		std::vector<Test*> getTestList() const;
		void runTests();

	protected:
		TestModule& module;
		std::vector<std::unique_ptr<Test>> test_list;

	private:

	};

	class TestManager;

	class TestModule {
	public:
		std::string name;
		std::vector<TestModule*> required_modules;
		std::vector<std::unique_ptr<TestList>> test_lists;
		std::vector<std::string> passed_list;
		std::vector<std::string> cancelled_list;
		std::vector<std::string> failed_list;
		bool is_run = false;

		TestModule(const std::string& name, TestManager& manager, const std::vector<TestModule*>& required_modules = { });
		virtual void createTestLists() = 0;
		TestList* createTestList(const std::string& name, const std::vector<TestList*>& required_lists = { });
		std::vector<Test*> getTestList() const;
		void runTests();
		static void printSummary(
			const std::vector<std::string>& passed_list,
			const std::vector<std::string>& cancelled_list,
			const std::vector<std::string>& failed_list
		);

	protected:
		friend class TestList;
		TestManager& manager;

		virtual void beforeRunModule();
		virtual void afterRunModule();
		static void testMessage(Test& test, const std::string& file, size_t line, const std::string& message);
		static bool testCheck(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message);
		static bool testCheck(Test& test, const std::string& file, size_t line, bool value, const std::string& value_message, const std::string message);
		template<typename T1, typename T2>
		static bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected);
		template<typename T1, typename T2, typename TStr>
		static bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str);
		template<typename T1, typename T2, typename TStr, typename TCmp>
		static bool testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str, TCmp cmp);
		template<typename T>
		static bool testApproxCompare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected, T epsilon = 0.0001f);
		template<typename T>
		static bool testVec2ApproxCompare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected, double epsilon = 0.0001);
		template<typename T, typename TEps>
		static bool equals(T left, T right, TEps epsilon = 0.0001f);

	private:
		template<typename T1, typename T2, typename TStr>
		static void compareFail(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str);

	};

	class TestManager {
	public:
		bool print_module_summary = false;
		bool print_list_summary = false;

		template<typename T>
		requires std::derived_from<T, TestModule>
		TestModule* addModule(const std::vector<TestModule*>& required_modules = { });
		void runAllModules();

	private:
		friend class TestList;
		friend class Test;
		std::vector<std::unique_ptr<TestModule>> modules;
		size_t max_test_name = 0;

	};

	template<typename T1, typename T2>
	inline bool TestModule::testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected) {
		if constexpr (std::convertible_to<T1, std::string> || std::same_as<T1, const char*>) {
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

	template<typename T1, typename T2, typename TStr, typename TCmp>
	inline bool TestModule::testCompare(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str, TCmp cmp) {
		if (!cmp(actual, expected)) {
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
	inline bool TestModule::testVec2ApproxCompare(Test& test, const std::string& file, size_t line, const std::string& name, T actual, T expected, double epsilon) {
		if (!equals(actual.x, expected.x, epsilon) || !equals(actual.y, expected.y, epsilon)) {
			auto to_str = [](const T& vec) {
				return "(" + std::to_string(vec.x) + " " + std::to_string(vec.y) + ")";
			};
			compareFail(test, file, line, name, actual, expected, to_str);
			return false;
		}
		return true;
	}

	template<typename T, typename TEps>
	inline bool TestModule::equals(T left, T right, TEps epsilon) {
		return abs(left - right) < epsilon;
	}

	template<typename T1, typename T2, typename TStr>
	inline void TestModule::compareFail(Test& test, const std::string& file, size_t line, const std::string& name, T1 actual, T2 expected, TStr to_str) {
		std::string filename = std::filesystem::path(file).filename().string();
		std::string location_str = "[" + filename + ":" + std::to_string(line) + "]";
		Test::Error* error = test.getCurrentError()->add(name + " " + location_str);
		error->add("Expected value: " + to_str(expected));
		error->add("Actual value:   " + to_str(actual));
		test.result = false;
	}

	template<typename T>
	requires std::derived_from<T, TestModule>
	inline TestModule* TestManager::addModule(const std::vector<TestModule*>& required_modules) {
		std::unique_ptr<T> uptr = std::make_unique<T>(*this, required_modules);
		T* ptr = uptr.get();
		modules.push_back(std::move(uptr));
		return ptr;
	}

}
