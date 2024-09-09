#include "tests/data_pointer_tests.h"
#include "common/data_pointer.h"
#include <functional>
#include "common/utils.h"

struct MyStruct {
	MyStruct(int val) {
		this->val = val;
	}
	int val = 0;
	int hidden = 0;
};

struct ChildStruct : public MyStruct {
	ChildStruct(int val) : MyStruct(val) { }
	int child_hidden = 0;
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
	list->OnAfterRunTest = [&]() {
		data_blocks.clear();
	};
	test::Test* null_test = list->addTest("null", [&](test::Test& test) { nullTest(test); });
	test::Test* basic_test = list->addTest("basic", [&](test::Test& test) { basicTest(test); });
	test::Test* struct_test = list->addTest("struct", { basic_test }, [&](test::Test& test) { structTest(test); });
	test::Test* custom_deleter_test = list->addTest("custom_deleter", { struct_test }, [&](test::Test& test) { customDeleterTest(test); });
	test::Test* get_test = list->addTest("get", { struct_test }, [&](test::Test& test) { getTest(test); });
	test::Test* release_test = list->addTest("release", { get_test }, [&](test::Test& test) { releaseTest(test); });
	test::Test* reset_test = list->addTest("reset", { get_test }, [&](test::Test& test) { resetTest(test); });
	test::Test* release_silent_test = list->addTest("release_silent", { release_test }, [&](test::Test& test) { releaseSilentTest(test); });
	test::Test* reset_silent_test = list->addTest("reset_silent", { reset_test }, [&](test::Test& test) { resetSilentTest(test); });
	test::Test* dereference_test = list->addTest("dereference", { struct_test }, [&](test::Test& test) { dereferenceTest(test); });
	test::Test* pointer_access_test = list->addTest("pointer_access", { struct_test }, [&](test::Test& test) { pointerAccessTest(test); });
	test::Test* move_test = list->addTest("move", { struct_test }, [&](test::Test& test) { moveTest(test); });
	test::Test* make_data_pointer_test = list->addTest("make_data_pointer", { struct_test }, [&](test::Test& test) { makeDataPointerTest(test); });
	test::Test* make_data_pointer_derived_test = list->addTest("make_data_pointer_derived", { make_data_pointer_test }, [&](test::Test& test) { makeDataPointerDerivedTest(test); });
}

void DataPointerTests::nullTest(test::Test& test) {
	DataPointer<int> dp(nullptr);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::basicTest(test::Test& test) {
	int* x = new int(5);
	DataPointer<int> dp(x);

	T_WRAP_CONTAINER(checkDataBlock(test, x, sizeof(int)));
}

void DataPointerTests::structTest(test::Test& test) {
	MyStruct* m = new MyStruct(11);
	DataPointer<MyStruct> dp(m);
	static_assert(sizeof(MyStruct) != sizeof(int));

	T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
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
	T_COMPARE(m2, m, &utils::pointer_to_str);
}

void DataPointerTests::releaseTest(test::Test& test) {
	MyStruct* m = new MyStruct(44);
	DataPointer<MyStruct> dp(m);
	MyStruct* m2 = dp.release();
	T_COMPARE(m2, m, &utils::pointer_to_str);
	MyStruct* m3 = dp.get();
	T_COMPARE(m3, nullptr, &utils::pointer_to_str);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::resetTest(test::Test& test) {
	MyStruct* m = new MyStruct(55);
	MyStruct* m2 = new MyStruct(66);
	DataPointer<MyStruct> dp(m);
	MyStruct* mt1 = dp.get();
	T_COMPARE(mt1, m, &utils::pointer_to_str);
	dp.reset(m2);
	MyStruct* mt2 = dp.get();
	T_COMPARE(mt2, m2, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m2, sizeof(MyStruct)));
}

void DataPointerTests::releaseSilentTest(test::Test& test) {
	MyStruct* m = new MyStruct(66);
	DataPointer<MyStruct> dp(m);
	MyStruct* m2 = dp.releaseSilent();
	T_COMPARE(m2, m, &utils::pointer_to_str);
	MyStruct* m3 = dp.get();
	T_COMPARE(m3, nullptr, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m2, sizeof(MyStruct)));
}

void DataPointerTests::resetSilentTest(test::Test& test) {
	MyStruct* m = new MyStruct(77);
	MyStruct* m2 = new MyStruct(88);
	DataPointer<MyStruct> dp(m);
	MyStruct* mt1 = dp.get();
	T_COMPARE(mt1, m, &utils::pointer_to_str);
	dp.resetSilent(m2);
	MyStruct* mt2 = dp.get();
	T_COMPARE(mt2, m2, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
}

void DataPointerTests::dereferenceTest(test::Test& test) {
	MyStruct* m = new MyStruct(99);
	DataPointer<MyStruct> dp(m);
	MyStruct mystruct = *dp;
	T_COMPARE(mystruct.val, 99);
}

void DataPointerTests::pointerAccessTest(test::Test& test) {
	MyStruct* m = new MyStruct(111);
	DataPointer<MyStruct> dp(m);
	int val = dp->val;
	T_COMPARE(val, 111);
}

void DataPointerTests::moveTest(test::Test& test) {
	std::vector<DataPointer<MyStruct>> vec;
	MyStruct* m = new MyStruct(222);
	{
		DataPointer<MyStruct> dp(m);
		vec.push_back(std::move(dp));
	}
	T_COMPARE(vec[0]->val, 222);

	T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
}

void DataPointerTests::makeDataPointerTest(test::Test& test) {
	{
		DataPointer<MyStruct> dp = make_data_pointer<MyStruct>(333);
		T_COMPARE(dp->val, 333);

		MyStruct* m = dp.get();
		T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
	}

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::makeDataPointerDerivedTest(test::Test& test) {
	{
		DataPointer<MyStruct> dp = make_data_pointer<ChildStruct>(444);
		T_COMPARE(dp->val, 444);

		MyStruct* m = dp.get();
		T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(ChildStruct)));
	}

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::checkDataBlock(test::Test& test, void* p_block, size_t p_size) {
	if (T_COMPARE(data_blocks.size(), 1)) {
		auto it = data_blocks.find(p_block);
		if (T_CHECK(it != data_blocks.end())) {
			void* ptr = it->second.ptr;
			size_t size = it->second.size;
			T_COMPARE(ptr, p_block, &utils::pointer_to_str);
			T_COMPARE(size, p_size);
		}
	}
}
