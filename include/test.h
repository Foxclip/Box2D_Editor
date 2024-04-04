#include <vector>
#include <functional>
#include <string>

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
		std::string name = "<unnamed>";
		std::vector<std::string> errors;
		bool result = true;

		Test(std::string name, TestFuncType func);
		bool run();

	private:
		TestFuncType func;
	};

	class TestList {
	public:
		std::string name;

		TestList(const std::string& name);
		void runTests();

	protected:
		std::vector<Test> test_list;

		void addTest(std::string name, TestFuncType func);
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
		std::string message = name + "\n";
		std::string actual_value_str;
		std::string expected_value_str;
		if constexpr (std::is_same_v<T, std::string>) {
			actual_value_str = actual;
			expected_value_str = expected;
		} else {
			expected_value_str = std::to_string(expected);
			actual_value_str = std::to_string(actual);
		}
		message += "    Expected value: " + expected_value_str + "\n";
		message += "      Actual value: " + actual_value_str;
		test.errors.push_back(message);
		test.result = false;
	}

}
