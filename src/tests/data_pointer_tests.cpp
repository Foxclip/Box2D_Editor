#include "tests/data_pointer_tests.h"
#include "common/data_pointer.h"
#include <functional>

struct MyStruct {
	MyStruct(int val) {
		this->val = val;
	}
	int val = 0;
	int hidden = 0;
};

template<typename T>
struct CustomDeleter {
	CustomDeleter() { }
	CustomDeleter(const std::function<void(void)>& func) {
		this->func = func;
	}
	void operator()(T* ptr) {
		func();
		delete ptr;
	}
	std::function<void(void)> func;
};

DataPointerTests::DataPointerTests(
	test::TestManager& manager, const std::vector<TestModule*>& required_modules
) : TestModule("DataPointer", manager, required_modules) { }

void DataPointerTests::createTestLists() {
	test::TestList* list = createTestList("DataPointer");
	test::Test* null_test = list->addTest("null", [&](test::Test& test) { nullTest(test); });
	test::Test* basic_test = list->addTest("basic", [&](test::Test& test) { basicTest(test); });
	test::Test* struct_test = list->addTest("struct", { basic_test }, [&](test::Test& test) { structTest(test); });
	test::Test* custom_deleter_test = list->addTest("custom_deleter", { struct_test }, [&](test::Test& test) { customDeleterTest(test); });
	test::Test* get_test = list->addTest("get", { struct_test }, [&](test::Test& test) { getTest(test); });
	test::Test* release_test = list->addTest("release", { get_test }, [&](test::Test& test) { releaseTest(test); });
	test::Test* reset_test = list->addTest("reset", { get_test }, [&](test::Test& test) { resetTest(test); });
	test::Test* dereference_test = list->addTest("dereference", { struct_test }, [&](test::Test& test) { dereferenceTest(test); });
	test::Test* pointer_access_test = list->addTest("pointer_access", { struct_test }, [&](test::Test& test) { pointerAccessTest(test); });
	test::Test* move_test = list->addTest("move", { struct_test }, [&](test::Test& test) { moveTest(test); });
}

void DataPointerTests::nullTest(test::Test& test) {
	DataPointer<int> dp(nullptr);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::basicTest(test::Test& test) {
	int* x = new int(5);
	DataPointer<int> dp(x);

	if (T_COMPARE(data_blocks.size(), 1)) {
		auto it = data_blocks.find(x);
		if (T_CHECK(it != data_blocks.end())) {
			void* ptr = it->second.ptr;
			size_t size = it->second.size;
			T_COMPARE(ptr, x, &pointer_to_str);
			T_COMPARE(size, sizeof(int));
		}
	}
}

void DataPointerTests::structTest(test::Test& test) {
	MyStruct* m = new MyStruct(11);
	DataPointer<MyStruct> dp(m);
	static_assert(sizeof(MyStruct) != sizeof(int));

	if (T_COMPARE(data_blocks.size(), 1)) {
		auto it = data_blocks.find(m);
		if (T_CHECK(it != data_blocks.end())) {
			void* ptr = it->second.ptr;
			size_t size = it->second.size;
			T_COMPARE(ptr, m, &pointer_to_str);
			T_COMPARE(size, sizeof(MyStruct));
		}
	}
}

void DataPointerTests::customDeleterTest(test::Test& test) {
	MyStruct* m = new MyStruct(22);
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<MyStruct> cd(func);
	{
		DataPointer<MyStruct, CustomDeleter<MyStruct>> dp(m, cd);
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::getTest(test::Test& test) {
	MyStruct* m = new MyStruct(33);
	DataPointer<MyStruct> dp(m);
	MyStruct* m2 = dp.get();
	T_COMPARE(m2, m, &pointer_to_str);
}

void DataPointerTests::releaseTest(test::Test& test) {
	MyStruct* m = new MyStruct(44);
	DataPointer<MyStruct> dp(m);
	MyStruct* m2 = dp.release();
	T_COMPARE(m2, m, &pointer_to_str);
	MyStruct* m3 = dp.get();
	T_COMPARE(m3, nullptr, &pointer_to_str);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::resetTest(test::Test& test) {
	MyStruct* m = new MyStruct(55);
	MyStruct* m2 = new MyStruct(66);
	DataPointer<MyStruct> dp(m);
	MyStruct* mt1 = dp.get();
	T_COMPARE(mt1, m, &pointer_to_str);
	dp.reset(m2);
	MyStruct* mt2 = dp.get();
	T_COMPARE(mt2, m2, &pointer_to_str);

	if (T_COMPARE(data_blocks.size(), 1)) {
		auto it = data_blocks.find(m2);
		if (T_CHECK(it != data_blocks.end())) {
			void* ptr = it->second.ptr;
			size_t size = it->second.size;
			T_COMPARE(ptr, m2, &pointer_to_str);
			T_COMPARE(size, sizeof(MyStruct));
		}
	}
}

void DataPointerTests::dereferenceTest(test::Test& test) {
	MyStruct* m = new MyStruct(77);
	DataPointer<MyStruct> dp(m);
	MyStruct mystruct = *dp;
	T_COMPARE(mystruct.val, 77);
}

void DataPointerTests::pointerAccessTest(test::Test& test) {
	MyStruct* m = new MyStruct(88);
	DataPointer<MyStruct> dp(m);
	int val = dp->val;
	T_COMPARE(val, 88);
}

void DataPointerTests::moveTest(test::Test& test) {
	std::vector<DataPointer<MyStruct>> vec;
	MyStruct* m = new MyStruct(99);
	{
		DataPointer<MyStruct> dp(m);
		vec.push_back(std::move(dp));
	}
	T_COMPARE(vec[0]->val, 99);

	if (T_COMPARE(data_blocks.size(), 1)) {
		auto it = data_blocks.find(m);
		if (T_CHECK(it != data_blocks.end())) {
			void* ptr = it->second.ptr;
			size_t size = it->second.size;
			T_COMPARE(ptr, m, &pointer_to_str);
			T_COMPARE(size, sizeof(MyStruct));
		}
	}
}
