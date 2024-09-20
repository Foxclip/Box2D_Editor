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
	std::function<void(void)> destructor_func = []() { };
	~MyStruct() {
		destructor_func();
	}
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
	std::function<void(void)> func = []() { };
};

template<typename T>
struct AnotherDeleter {
	AnotherDeleter() { }
	template<typename T2>
	AnotherDeleter(const CustomDeleter<T2>& cd) {
		this->func = cd.func;
	}
	AnotherDeleter(const std::function<void(void)>& func) {
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
	test::Test* destructor_test = list->addTest("destructor", { struct_test }, [&](test::Test& test) { destructorTest(test); });
	test::Test* derived_destructor_test = list->addTest("derived_destructor", { destructor_test }, [&](test::Test& test) { derivedDestructorTest(test); });
	test::Test* vector_destructor_test = list->addTest("vector_destructor", { destructor_test }, [&](test::Test& test) { vectorDestructorTest(test); });
	test::Test* custom_deleter_test = list->addTest("custom_deleter", { struct_test }, [&](test::Test& test) { customDeleterTest(test); });
	test::Test* lambda_deleter_test = list->addTest("lambda_deleter", { struct_test }, [&](test::Test& test) { lambdaDeleterTest(test); });
	test::Test* get_test = list->addTest("get", { struct_test }, [&](test::Test& test) { getTest(test); });
	test::Test* release_test = list->addTest("release", { get_test }, [&](test::Test& test) { releaseTest(test); });
	test::Test* reset_test = list->addTest("reset", { get_test }, [&](test::Test& test) { resetTest(test); });
	test::Test* reset_deleter_test = list->addTest("reset_deleter", { reset_test }, [&](test::Test& test) { resetDeleterTest(test); });
	test::Test* release_silent_test = list->addTest("release_silent", { release_test }, [&](test::Test& test) { releaseSilentTest(test); });
	test::Test* reset_silent_test = list->addTest("reset_silent", { reset_test }, [&](test::Test& test) { resetSilentTest(test); });
	test::Test* move_constructor_test = list->addTest("move_contructor", { release_silent_test }, [&](test::Test& test) { moveConstructorTest(test); });
	test::Test* move_constructor_deleter_test = list->addTest("move_contructor_deleter", { move_constructor_test }, [&](test::Test& test) { moveConstructorDeleterTest(test); });
	test::Test* move_constructor_derived_test = list->addTest("move_contructor_derived", { move_constructor_test }, [&](test::Test& test) { moveConstructorDerivedTest(test); });
	test::Test* move_constructor_derived_deleter_test = list->addTest("move_contructor_derived_deleter", { move_constructor_derived_test }, [&](test::Test& test) { moveConstructorDerivedDeleterTest(test); });
	test::Test* move_assignment_test = list->addTest("move_assignment", { release_silent_test }, [&](test::Test& test) { moveAssignmentTest(test); });
	test::Test* move_assignment_deleter_test = list->addTest("move_assignment_deleter", { move_assignment_test }, [&](test::Test& test) { moveAssignmentDeleterTest(test); });
	test::Test* move_assignment_derived_test = list->addTest("move_assignment_derived", { move_assignment_test }, [&](test::Test& test) { moveAssignmentDerivedTest(test); });
	test::Test* move_assignment_derived_deleter_test = list->addTest("move_assignment_derived_deleter", { move_assignment_derived_test }, [&](test::Test& test) { moveAssignmentDerivedDeleterTest(test); });
	test::Test* swap_test = list->addTest("swap", { get_test }, [&](test::Test& test) { swapTest(test); });
	test::Test* dereference_test = list->addTest("dereference", { struct_test }, [&](test::Test& test) { dereferenceTest(test); });
	test::Test* pointer_access_test = list->addTest("pointer_access", { struct_test }, [&](test::Test& test) { pointerAccessTest(test); });
	test::Test* move_test = list->addTest("move", { struct_test }, [&](test::Test& test) { moveTest(test); });
	test::Test* make_data_pointer_test = list->addTest("make_data_pointer", { struct_test }, [&](test::Test& test) { makeDataPointerTest(test); });
	test::Test* make_data_pointer_derived_test = list->addTest("make_data_pointer_derived", { make_data_pointer_test }, [&](test::Test& test) { makeDataPointerDerivedTest(test); });
}

void DataPointerTests::nullTest(test::Test& test) {
	DataPointer<int> dp("Null", nullptr);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::basicTest(test::Test& test) {
	int* x = new int(5);
	DataPointer<int> dp("x", x);

	T_WRAP_CONTAINER(checkDataBlock(test, x, sizeof(int)));
}

void DataPointerTests::structTest(test::Test& test) {
	MyStruct* m = new MyStruct(11);
	DataPointer<MyStruct> dp("MyStruct", m);
	static_assert(sizeof(MyStruct) != sizeof(int));

	T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
}

void DataPointerTests::destructorTest(test::Test& test) {
	bool flag = false;
	MyStruct* m = new MyStruct(11);
	m->destructor_func = [&]() {
		flag = true;
	};
	{
		DataPointer<MyStruct> dp("MyStruct", m);
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::derivedDestructorTest(test::Test& test) {
	bool flag = false;
	{
		DataPointer<MyStruct> dp = make_data_pointer<ChildStruct>("ChildStruct", 11);
		dp->destructor_func = [&]() {
			flag = true;
		};
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::vectorDestructorTest(test::Test& test) {
	bool flag = false;
	MyStruct* m = new MyStruct(11);
	m->destructor_func = [&]() {
		flag = true;
	};
	std::vector<DataPointer<MyStruct>> vec;
	DataPointer<MyStruct> dp("MyStruct", m);
	DataPointer<MyStruct> dp2;
	vec.push_back(std::move(dp));
	vec.push_back(std::move(dp2));
	vec.erase(vec.begin());
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::customDeleterTest(test::Test& test) {
	MyStruct* m = new MyStruct(22);
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<MyStruct> cd(func);
	{
		DataPointer<MyStruct, CustomDeleter<MyStruct>> dp("MyStruct", m, cd);
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::lambdaDeleterTest(test::Test& test) {
	MyStruct* m = new MyStruct(22);
	bool flag = false;
	auto func = [&](MyStruct* ptr) {
		flag = true;
		delete ptr;
	};
	{
		DataPointer<MyStruct, decltype(func)> dp("MyStruct", m, func);
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::getTest(test::Test& test) {
	MyStruct* m = new MyStruct(33);
	DataPointer<MyStruct> dp("MyStruct", m);
	MyStruct* m2 = dp.get();
	T_COMPARE(m2, m, &utils::pointer_to_str);
}

void DataPointerTests::releaseTest(test::Test& test) {
	MyStruct* m = new MyStruct(44);
	DataPointer<MyStruct> dp("MyStruct", m);
	MyStruct* m2 = dp.release();
	T_COMPARE(m2, m, &utils::pointer_to_str);
	MyStruct* m3 = dp.get();
	T_COMPARE(m3, nullptr, &utils::pointer_to_str);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::resetTest(test::Test& test) {
	MyStruct* m = new MyStruct(55);
	MyStruct* m2 = new MyStruct(66);
	DataPointer<MyStruct> dp("MyStruct", m);
	MyStruct* mt1 = dp.get();
	T_COMPARE(mt1, m, &utils::pointer_to_str);
	dp.reset("m2", m2);
	MyStruct* mt2 = dp.get();
	T_COMPARE(mt2, m2, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m2, sizeof(MyStruct)));
}

void DataPointerTests::resetDeleterTest(test::Test& test) {
	bool flag = false;
	MyStruct* del_ptr = nullptr;
	auto deleter = [&](MyStruct* ptr) {
		flag = true;
		del_ptr = ptr;
		delete ptr;
	};
	MyStruct* m = new MyStruct(55);
	MyStruct* m2 = new MyStruct(66);
	DataPointer<MyStruct, decltype(deleter)> dp("MyStruct", m, deleter);
	MyStruct* m3 = dp.get();
	T_COMPARE(m3, m, &utils::pointer_to_str);
	dp.reset("m2", m2);
	MyStruct* m4 = dp.get();
	T_COMPARE(m4, m2, &utils::pointer_to_str);
	T_CHECK(flag);
	T_COMPARE(del_ptr, m, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m2, sizeof(MyStruct)));
}

void DataPointerTests::releaseSilentTest(test::Test& test) {
	MyStruct* m = new MyStruct(66);
	DataPointer<MyStruct> dp("MyStruct", m);
	MyStruct* m2 = dp.releaseSilent();
	T_COMPARE(m2, m, &utils::pointer_to_str);
	MyStruct* m3 = dp.get();
	T_COMPARE(m3, nullptr, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m2, sizeof(MyStruct)));
}

void DataPointerTests::resetSilentTest(test::Test& test) {
	bool flag = false;
	MyStruct* del_ptr = nullptr;
	auto deleter = [&](MyStruct* ptr) {
		flag = true;
		del_ptr = ptr;
		delete ptr;
	};
	MyStruct* m = new MyStruct(77);
	MyStruct* m2 = new MyStruct(88);
	DataPointer<MyStruct, decltype(deleter)> dp("MyStruct", m, deleter);
	MyStruct* mt1 = dp.get();
	T_COMPARE(mt1, m, &utils::pointer_to_str);
	dp.resetSilent(m2);
	MyStruct* mt2 = dp.get();
	T_COMPARE(mt2, m2, &utils::pointer_to_str);
	T_CHECK(flag);
	T_COMPARE(del_ptr, m, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
	T_WRAP_CONTAINER(checkNoDataBlock(test, m2));

	add_to_data_blocks("m2", m2, sizeof(MyStruct)); // to avoid triggering assert in ~DataPointer
	remove_from_data_blocks(m);
}

void DataPointerTests::moveConstructorTest(test::Test& test) {
	MyStruct* m = new MyStruct(11);
	DataPointer<MyStruct> dp("MyStruct", m);
	DataPointer<MyStruct> dp2(std::move(dp));
	MyStruct* m2 = dp.get();
	MyStruct* m3 = dp2.get();
	T_COMPARE(m2, nullptr, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(MyStruct)));
}

void DataPointerTests::moveConstructorDeleterTest(test::Test& test) {
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<MyStruct> cd(func);
	{
		MyStruct* m = new MyStruct(11);
		DataPointer<MyStruct, CustomDeleter<MyStruct>> dp("MyStruct", m, cd);
		DataPointer<MyStruct, AnotherDeleter<MyStruct>> dp2(std::move(dp));
		MyStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, nullptr, &utils::pointer_to_str);

		T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	T_CHECK(flag);
}

void DataPointerTests::moveConstructorDerivedTest(test::Test& test) {
	ChildStruct* m = new ChildStruct(11);
	DataPointer<ChildStruct> dp("ChildStruct", m);
	DataPointer<MyStruct> dp2(std::move(dp));
	MyStruct* m2 = dp.get();
	MyStruct* m3 = dp2.get();
	T_COMPARE(m2, nullptr, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(ChildStruct)));
}

void DataPointerTests::moveConstructorDerivedDeleterTest(test::Test& test) {
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<ChildStruct> cd(func);
	{
		ChildStruct* m = new ChildStruct(11);
		DataPointer<ChildStruct, CustomDeleter<ChildStruct>> dp("ChildStruct", m, cd);
		DataPointer<MyStruct, AnotherDeleter<MyStruct>> dp2(std::move(dp));
		ChildStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, nullptr, &utils::pointer_to_str);

		T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(ChildStruct)));
	}
	T_CHECK(flag);
}

void DataPointerTests::moveAssignmentTest(test::Test& test) {
	bool flag = false;
	MyStruct* m1 = new MyStruct(11);
	m1->destructor_func = [&]() {
		flag = true;
	};
	MyStruct* m2 = new MyStruct(22);
	DataPointer<MyStruct> dp1("MyStruct", m1);
	DataPointer<MyStruct> dp2("MyStruct", m2);
	dp1 = std::move(dp2);
	MyStruct* m3 = dp1.get();
	MyStruct* m4 = dp2.get();
	T_COMPARE(m4, nullptr, &utils::pointer_to_str);
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 1);
	T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(MyStruct)));
}

void DataPointerTests::moveAssignmentDeleterTest(test::Test& test) {
	bool flag1 = false;
	auto func = [&]() {
		flag1 = true;
	};
	CustomDeleter<MyStruct> cd(func);
	{
		bool flag2 = false;
		MyStruct* m1 = new MyStruct(11);
		m1->destructor_func = [&]() {
			flag2 = true;
		};
		MyStruct* m2 = new MyStruct(22);
		DataPointer<MyStruct, AnotherDeleter<MyStruct>> dp1("MyStruct", m1, cd);
		DataPointer<MyStruct, CustomDeleter<MyStruct>> dp2("MyStruct", m2);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);
		T_CHECK(flag2);

		T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	T_CHECK(flag1);
}

void DataPointerTests::moveAssignmentDerivedTest(test::Test& test) {
	bool flag = false;
	MyStruct* m1 = new MyStruct(11);
	m1->destructor_func = [&]() {
		flag = true;
	};
	ChildStruct* m2 = new ChildStruct(22);
	DataPointer<MyStruct> dp1("MyStruct", m1);
	DataPointer<ChildStruct> dp2("ChildStruct", m2);
	dp1 = std::move(dp2);
	MyStruct* m3 = dp1.get();
	ChildStruct* m4 = dp2.get();
	T_COMPARE(m4, nullptr, &utils::pointer_to_str);
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 1);
	T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(ChildStruct)));
}

void DataPointerTests::moveAssignmentDerivedDeleterTest(test::Test& test) {
	bool flag1 = false;
	auto func = [&]() {
		flag1 = true;
	};
	CustomDeleter<ChildStruct> cd(func);
	{
		bool flag2 = false;
		MyStruct* m1 = new MyStruct(11);
		m1->destructor_func = [&]() {
			flag2 = true;
		};
		ChildStruct* m2 = new ChildStruct(22);
		DataPointer<MyStruct, AnotherDeleter<MyStruct>> dp1("MyStruct", m1, cd);
		DataPointer<ChildStruct, CustomDeleter<ChildStruct>> dp2("ChildStruct", m2);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		ChildStruct* m4 = dp2.get();
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);
		T_CHECK(flag2);

		T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(ChildStruct)));
	}
	T_CHECK(flag1);
}

void DataPointerTests::swapTest(test::Test& test) {
	MyStruct* m1 = new MyStruct(123);
	MyStruct* m2 = new MyStruct(567);
	DataPointer<MyStruct> dp1("MyStruct", m1);
	DataPointer<MyStruct> dp2("MyStruct", m2);
	dp1.swap(dp2);
	T_COMPARE(dp1.get(), m2, &utils::pointer_to_str);
	T_COMPARE(dp2.get(), m1, &utils::pointer_to_str);
}

void DataPointerTests::dereferenceTest(test::Test& test) {
	MyStruct* m = new MyStruct(99);
	DataPointer<MyStruct> dp("MyStruct", m);
	MyStruct mystruct = *dp;
	T_COMPARE(mystruct.val, 99);
}

void DataPointerTests::pointerAccessTest(test::Test& test) {
	MyStruct* m = new MyStruct(111);
	DataPointer<MyStruct> dp("MyStruct", m);
	int val = dp->val;
	T_COMPARE(val, 111);
}

void DataPointerTests::moveTest(test::Test& test) {
	std::vector<DataPointer<MyStruct>> vec;
	MyStruct* m = new MyStruct(222);
	{
		DataPointer<MyStruct> dp("MyStruct", m);
		vec.push_back(std::move(dp));
	}
	T_COMPARE(vec[0]->val, 222);

	T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
}

void DataPointerTests::makeDataPointerTest(test::Test& test) {
	{
		DataPointer<MyStruct> dp = make_data_pointer<MyStruct>("MyStruct", 333);
		T_COMPARE(dp->val, 333);

		MyStruct* m = dp.get();
		T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
	}

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::makeDataPointerDerivedTest(test::Test& test) {
	{
		DataPointer<MyStruct> dp = make_data_pointer<ChildStruct>("ChildStruct", 444);
		T_COMPARE(dp->val, 444);

		MyStruct* m = dp.get();
		T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(ChildStruct)));
	}

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerTests::checkDataBlock(test::Test& test, void* p_block, size_t p_size) {
	if (T_CHECK(data_blocks.size() > 0)) {
		auto it = data_blocks.find(p_block);
		if (T_CHECK(it != data_blocks.end())) {
			void* ptr = it->second.ptr;
			size_t size = it->second.size;
			T_COMPARE(ptr, p_block, &utils::pointer_to_str);
			T_COMPARE(size, p_size);
		}
	}
}

void DataPointerTests::checkNoDataBlock(test::Test& test, void* p_block) {
	auto it = data_blocks.find(p_block);
	T_CHECK(it == data_blocks.end());
}
