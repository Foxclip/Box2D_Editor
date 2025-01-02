#include "tests/data_pointer_shared_tests.h"
#include "common/data_pointer_shared.h"
#include <functional>
#include "common/utils.h"
#include "tests/data_pointer_tests.h"

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

DataPointerSharedTests::DataPointerSharedTests(
	const std::string& name,
	test::TestModule* parent,
	const std::vector<TestNode*>& required_nodes
) : TestModule(name, parent, required_nodes) {
	OnAfterRunTest = [&]() {
		dp::data_blocks.clear();
	};
	test::Test* null_test = addTest("null", [&](test::Test& test) { nullTest(test); });
	test::Test* basic_test = addTest("basic", [&](test::Test& test) { basicTest(test); });
	test::Test* struct_test = addTest("struct", { basic_test }, [&](test::Test& test) { structTest(test); });
	test::Test* destructor_test = addTest("destructor", { struct_test }, [&](test::Test& test) { destructorTest(test); });
	test::Test* derived_destructor_test = addTest("derived_destructor", { destructor_test }, [&](test::Test& test) { derivedDestructorTest(test); });
	test::Test* vector_destructor_test = addTest("vector_destructor", { destructor_test }, [&](test::Test& test) { vectorDestructorTest(test); });
	test::Test* custom_deleter_test = addTest("custom_deleter", { struct_test }, [&](test::Test& test) { customDeleterTest(test); });
	test::Test* lambda_deleter_test = addTest("lambda_deleter", { struct_test }, [&](test::Test& test) { lambdaDeleterTest(test); });
	test::Test* get_test = addTest("get", { struct_test }, [&](test::Test& test) { getTest(test); });
	test::Test* reset_test = addTest("reset", { get_test }, [&](test::Test& test) { resetTest(test); });
	test::Test* reset_deleter_test = addTest("reset_deleter", { reset_test }, [&](test::Test& test) { resetDeleterTest(test); });
	test::Test* move_constructor_test = addTest("move_contructor", { get_test }, [&](test::Test& test) { moveConstructorTest(test); });
	test::Test* move_constructor_deleter_test = addTest("move_contructor_deleter", { move_constructor_test }, [&](test::Test& test) { moveConstructorDeleterTest(test); });
	test::Test* move_constructor_derived_test = addTest("move_contructor_derived", { move_constructor_test }, [&](test::Test& test) { moveConstructorDerivedTest(test); });
	test::Test* move_constructor_derived_deleter_test = addTest("move_contructor_derived_deleter", { move_constructor_derived_test }, [&](test::Test& test) { moveConstructorDerivedDeleterTest(test); });
	test::Test* move_assignment_test = addTest("move_assignment", { get_test }, [&](test::Test& test) { moveAssignmentTest(test); });
	test::Test* move_assignment_deleter_test = addTest("move_assignment_deleter", { move_assignment_test }, [&](test::Test& test) { moveAssignmentDeleterTest(test); });
	test::Test* move_assignment_derived_test = addTest("move_assignment_derived", { move_assignment_test }, [&](test::Test& test) { moveAssignmentDerivedTest(test); });
	test::Test* move_assignment_derived_deleter_test = addTest("move_assignment_derived_deleter", { move_assignment_derived_test }, [&](test::Test& test) { moveAssignmentDerivedDeleterTest(test); });
	test::Test* copy_constructor_test = addTest("copy_constructor", { get_test }, [&](test::Test& test) { copyConstructorTest(test); });
	test::Test* copy_constructor_deleter_test = addTest("copy_constructor_deleter", { copy_constructor_test }, [&](test::Test& test) { copyConstructorDeleterTest(test); });
	test::Test* copy_constructor_derived_test = addTest("copy_constructor_derived", { copy_constructor_test }, [&](test::Test& test) { copyConstructorDerivedTest(test); });
	test::Test* copy_constructor_derived_deleter_test = addTest("copy_constructor_derived_deleter", { copy_constructor_derived_test }, [&](test::Test& test) { copyConstructorDerivedDeleterTest(test); });
	test::Test* copy_assignment_test = addTest("copy_assignment", { get_test }, [&](test::Test& test) { copyAssignmentTest(test); });
	test::Test* copy_assignment_deleter_test = addTest("copy_assignment_deleter", { copy_assignment_test }, [&](test::Test& test) { copyAssignmentDeleterTest(test); });
	test::Test* copy_assignment_derived_test = addTest("copy_assignment_derived", { copy_assignment_test }, [&](test::Test& test) { copyAssignmentDerivedTest(test); });
	test::Test* copy_assignment_derived_deleter_test = addTest("copy_assignment_derived_deleter", { copy_assignment_derived_test }, [&](test::Test& test) { copyAssignmentDerivedDeleterTest(test); });
	test::Test* copy_test = addTest("copy", { get_test }, [&](test::Test& test) { copyTest(test); });
	test::Test* swap_test = addTest("swap", { get_test }, [&](test::Test& test) { swapTest(test); });
	test::Test* dereference_test = addTest("dereference", { get_test }, [&](test::Test& test) { dereferenceTest(test); });
	test::Test* pointer_access_test = addTest("pointer_access", { get_test }, [&](test::Test& test) { pointerAccessTest(test); });
	test::Test* move_test = addTest("move", { get_test }, [&](test::Test& test) { moveTest(test); });
	test::Test* make_data_pointer_test = addTest("make_data_pointer", { get_test }, [&](test::Test& test) { makeDataPointerTest(test); });
	test::Test* make_data_pointer_derived_test = addTest("make_data_pointer_derived", { get_test }, [&](test::Test& test) { makeDataPointerDerivedTest(test); });
	test::Test* block_name_test = addTest("block_name", { make_data_pointer_test }, [&](test::Test& test) { blockNameTest(test); });

}

void DataPointerSharedTests::nullTest(test::Test& test) {
	dp::DataPointerShared<int> dp("Null", nullptr);
	T_COMPARE(dp.use_count(), 0);

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::basicTest(test::Test& test) {
	int* x = new int(5);
	dp::DataPointerShared<int> dp("x", x);
	T_COMPARE(dp.use_count(), 1);

	T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, x, sizeof(int)));
}

void DataPointerSharedTests::structTest(test::Test& test) {
	MyStruct* m = new MyStruct(11);
	dp::DataPointerShared<MyStruct> dp("MyStruct", m);
	static_assert(sizeof(MyStruct) != sizeof(int));

	T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m, sizeof(MyStruct)));
}

void DataPointerSharedTests::destructorTest(test::Test& test) {
	bool flag = false;
	MyStruct* m = new MyStruct(11);
	m->destructor_func = [&]() {
		flag = true;
	};
	{
		dp::DataPointerShared<MyStruct> dp("MyStruct", m);
	}
	T_CHECK(flag);

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::derivedDestructorTest(test::Test& test) {
	bool flag = false;
	{
		dp::DataPointerShared<MyStruct> dp = dp::make_shared_data_pointer<ChildStruct>("ChildStruct", 11);
		dp->destructor_func = [&]() {
			flag = true;
		};
	}
	T_CHECK(flag);

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::vectorDestructorTest(test::Test& test) {
	bool flag = false;
	MyStruct* m = new MyStruct(11);
	m->destructor_func = [&]() {
		flag = true;
	};
	std::vector<dp::DataPointerShared<MyStruct>> vec;
	dp::DataPointerShared<MyStruct> dp("MyStruct", m);
	dp::DataPointerShared<MyStruct> dp2;
	vec.push_back(std::move(dp));
	vec.push_back(std::move(dp2));
	vec.erase(vec.begin());
	T_CHECK(flag);

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::customDeleterTest(test::Test& test) {
	MyStruct* m = new MyStruct(22);
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<MyStruct> cd(func);
	{
		dp::DataPointerShared<MyStruct, CustomDeleter<MyStruct>> dp("MyStruct", m, cd);
	}
	T_CHECK(flag);

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::lambdaDeleterTest(test::Test& test) {
	MyStruct* m = new MyStruct(22);
	bool flag = false;
	auto func = [&](MyStruct* ptr) {
		flag = true;
		delete ptr;
	};
	{
		dp::DataPointerShared<MyStruct, decltype(func)> dp("MyStruct", m, func);
	}
	T_CHECK(flag);

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::getTest(test::Test& test) {
	MyStruct* m = new MyStruct(33);
	dp::DataPointerShared<MyStruct> dp("MyStruct", m);
	MyStruct* m2 = dp.get();
	T_COMPARE(m2, m, &utils::pointer_to_str);
}

void DataPointerSharedTests::resetTest(test::Test& test) {
	MyStruct* m = new MyStruct(55);
	MyStruct* m2 = new MyStruct(66);
	dp::DataPointerShared<MyStruct> dp("MyStruct", m);
	MyStruct* mt1 = dp.get();
	T_COMPARE(mt1, m, &utils::pointer_to_str);
	dp.reset("m2", m2);
	MyStruct* mt2 = dp.get();
	T_COMPARE(mt2, m2, &utils::pointer_to_str);

	T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m2, sizeof(MyStruct)));
}

void DataPointerSharedTests::resetDeleterTest(test::Test& test) {
	bool flag = false;
	MyStruct* del_ptr = nullptr;
	auto deleter = [&](MyStruct* ptr) {
		flag = true;
		del_ptr = ptr;
		delete ptr;
	};
	MyStruct* m = new MyStruct(55);
	MyStruct* m2 = new MyStruct(66);
	dp::DataPointerShared<MyStruct, decltype(deleter)> dp("MyStruct", m, deleter);
	MyStruct* m3 = dp.get();
	T_COMPARE(m3, m, &utils::pointer_to_str);
	dp.reset("m2", m2);
	MyStruct* m4 = dp.get();
	T_COMPARE(m4, m2, &utils::pointer_to_str);
	T_CHECK(flag);
	T_COMPARE(del_ptr, m, &utils::pointer_to_str);

	T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m2, sizeof(MyStruct)));
}

void DataPointerSharedTests::moveConstructorTest(test::Test& test) {
	{
		// from ptr
		MyStruct* m = new MyStruct(11);
		dp::DataPointerShared<MyStruct> dp("MyStruct", m);
		dp::DataPointerShared<MyStruct> dp2(std::move(dp));
		MyStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, nullptr, &utils::pointer_to_str);

		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	{
		// from nullptr
		dp::DataPointerShared<MyStruct> dp("null", nullptr);
		dp::DataPointerShared<MyStruct> dp2(std::move(dp));
		MyStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, nullptr, &utils::pointer_to_str);
		T_COMPARE(m3, nullptr, &utils::pointer_to_str);

		T_COMPARE(dp::data_blocks.size(), 0);
	}
}

void DataPointerSharedTests::moveConstructorDeleterTest(test::Test& test) {
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<MyStruct> cd(func);
	{
		MyStruct* m = new MyStruct(11);
		dp::DataPointerShared<MyStruct, CustomDeleter<MyStruct>> dp("MyStruct", m, cd);
		dp::DataPointerShared<MyStruct, AnotherDeleter<MyStruct>> dp2(std::move(dp));
		MyStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, nullptr, &utils::pointer_to_str);

		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	T_CHECK(flag);
}

void DataPointerSharedTests::moveConstructorDerivedTest(test::Test& test) {
	ChildStruct* m = new ChildStruct(11);
	dp::DataPointerShared<ChildStruct> dp("ChildStruct", m);
	dp::DataPointerShared<MyStruct> dp2(std::move(dp));
	MyStruct* m2 = dp.get();
	MyStruct* m3 = dp2.get();
	T_COMPARE(m2, nullptr, &utils::pointer_to_str);

	T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(ChildStruct)));
}

void DataPointerSharedTests::moveConstructorDerivedDeleterTest(test::Test& test) {
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<ChildStruct> cd(func);
	{
		ChildStruct* m = new ChildStruct(11);
		dp::DataPointerShared<ChildStruct, CustomDeleter<ChildStruct>> dp("ChildStruct", m, cd);
		dp::DataPointerShared<MyStruct, AnotherDeleter<MyStruct>> dp2(std::move(dp));
		ChildStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, nullptr, &utils::pointer_to_str);

		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(ChildStruct)));
	}
	T_CHECK(flag);
}

void DataPointerSharedTests::moveAssignmentTest(test::Test& test) {
	{
		bool flag = false;
		MyStruct* m1 = new MyStruct(11);
		m1->destructor_func = [&]() {
			flag = true;
		};
		MyStruct* m2 = new MyStruct(22);
		dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
		dp::DataPointerShared<MyStruct> dp2("MyStruct", m2);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);
		T_CHECK(flag);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	{
		// self to self
		bool flag = false;
		MyStruct* m1 = new MyStruct(11);
		m1->destructor_func = [&]() {
			flag = true;
		};
		dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
		dp1 = std::move(dp1);
		MyStruct* m2 = dp1.get();
		T_COMPARE(m2, m1, &utils::pointer_to_str);
		T_CHECK(!flag);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m2, sizeof(MyStruct)));
	}
	{
		// nullptr to ptr
		bool flag = false;
		MyStruct* m1 = new MyStruct(11);
		m1->destructor_func = [&]() {
			flag = true;
		};
		dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
		dp::DataPointerShared<MyStruct> dp2("null", nullptr);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m3, nullptr, &utils::pointer_to_str);
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);
		T_CHECK(flag);

		T_COMPARE(dp::data_blocks.size(), 0);
	}
	{
		// ptr to nullptr
		MyStruct* m2 = new MyStruct(22);
		dp::DataPointerShared<MyStruct> dp1("null", nullptr);
		dp::DataPointerShared<MyStruct> dp2("MyStruct", m2);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m3, m2, &utils::pointer_to_str);
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(MyStruct)));
	}
}

void DataPointerSharedTests::moveAssignmentDeleterTest(test::Test& test) {
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
		dp::DataPointerShared<MyStruct, AnotherDeleter<MyStruct>> dp1("MyStruct", m1, cd);
		dp::DataPointerShared<MyStruct, CustomDeleter<MyStruct>> dp2("MyStruct", m2);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);
		T_CHECK(flag2);

		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	T_CHECK(flag1);
}

void DataPointerSharedTests::moveAssignmentDerivedTest(test::Test& test) {
	bool flag = false;
	MyStruct* m1 = new MyStruct(11);
	m1->destructor_func = [&]() {
		flag = true;
	};
	ChildStruct* m2 = new ChildStruct(22);
	dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
	dp::DataPointerShared<ChildStruct> dp2("ChildStruct", m2);
	dp1 = std::move(dp2);
	MyStruct* m3 = dp1.get();
	ChildStruct* m4 = dp2.get();
	T_COMPARE(m4, nullptr, &utils::pointer_to_str);
	T_CHECK(flag);

	T_COMPARE(dp::data_blocks.size(), 1);
	T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(ChildStruct)));
}

void DataPointerSharedTests::moveAssignmentDerivedDeleterTest(test::Test& test) {
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
		dp::DataPointerShared<MyStruct, AnotherDeleter<MyStruct>> dp1("MyStruct", m1, cd);
		dp::DataPointerShared<ChildStruct, CustomDeleter<ChildStruct>> dp2("ChildStruct", m2);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		ChildStruct* m4 = dp2.get();
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);
		T_CHECK(flag2);

		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(ChildStruct)));
	}
	T_CHECK(flag1);
}

void DataPointerSharedTests::copyConstructorTest(test::Test& test) {
	{
		// ptr to ptr
		MyStruct* m = new MyStruct(11);
		dp::DataPointerShared<MyStruct> dp("MyStruct", m);
		dp::DataPointerShared<MyStruct> dp2(dp);
		MyStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, m, &utils::pointer_to_str);
		T_COMPARE(m3, m, &utils::pointer_to_str);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	{
		// nullptr to ptr
		dp::DataPointerShared<MyStruct> dp("null", nullptr);
		dp::DataPointerShared<MyStruct> dp2(dp);
		MyStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, nullptr, &utils::pointer_to_str);
		T_COMPARE(m3, nullptr, &utils::pointer_to_str);

		T_COMPARE(dp::data_blocks.size(), 0);
	}
}

void DataPointerSharedTests::copyConstructorDeleterTest(test::Test& test) {
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<MyStruct> cd(func);
	{
		MyStruct* m = new MyStruct(11);
		dp::DataPointerShared<MyStruct, CustomDeleter<MyStruct>> dp("MyStruct", m, cd);
		dp::DataPointerShared<MyStruct, AnotherDeleter<MyStruct>> dp2(dp);
		MyStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, m, &utils::pointer_to_str);
		T_COMPARE(m3, m, &utils::pointer_to_str);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	T_CHECK(flag);

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::copyConstructorDerivedTest(test::Test& test) {
	ChildStruct* m = new ChildStruct(11);
	dp::DataPointerShared<ChildStruct> dp("ChildStruct", m);
	dp::DataPointerShared<MyStruct> dp2(dp);
	MyStruct* m2 = dp.get();
	MyStruct* m3 = dp2.get();
	T_COMPARE(m2, m, &utils::pointer_to_str);
	T_COMPARE(m3, m, &utils::pointer_to_str);

	T_COMPARE(dp::data_blocks.size(), 1);
	T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(ChildStruct)));
}

void DataPointerSharedTests::copyConstructorDerivedDeleterTest(test::Test& test) {
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<ChildStruct> cd(func);
	{
		ChildStruct* m = new ChildStruct(11);
		dp::DataPointerShared<ChildStruct, CustomDeleter<ChildStruct>> dp("ChildStruct", m, cd);
		dp::DataPointerShared<MyStruct, AnotherDeleter<MyStruct>> dp2(dp);
		ChildStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, m, &utils::pointer_to_str);
		T_COMPARE(m3, m, &utils::pointer_to_str);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(ChildStruct)));
	}
	T_CHECK(flag);

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::copyAssignmentTest(test::Test& test) {
	{
		// ptr to ptr
		bool flag = false;
		MyStruct* m1 = new MyStruct(11);
		m1->destructor_func = [&]() {
			flag = true;
		};
		MyStruct* m2 = new MyStruct(22);
		dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
		dp::DataPointerShared<MyStruct> dp2("MyStruct", m2);
		dp1 = dp2;
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m3, m2, &utils::pointer_to_str);
		T_COMPARE(m4, m2, &utils::pointer_to_str);
		T_CHECK(flag);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	{
		// self to self
		bool flag = false;
		MyStruct* m1 = new MyStruct(11);
		m1->destructor_func = [&]() {
			flag = true;
		};
		dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
		dp1 = dp1;
		MyStruct* m2 = dp1.get();
		T_COMPARE(m2, m1, &utils::pointer_to_str);
		T_CHECK(!flag);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m2, sizeof(MyStruct)));
	}
	{
		// nullptr to ptr
		bool flag = false;
		MyStruct* m1 = new MyStruct(11);
		m1->destructor_func = [&]() {
			flag = true;
		};
		dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
		dp::DataPointerShared<MyStruct> dp2("null", nullptr);
		dp1 = dp2;
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m3, nullptr, &utils::pointer_to_str);
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);
		T_CHECK(flag);

		T_COMPARE(dp::data_blocks.size(), 0);
	}
	{
		// ptr to nullptr
		MyStruct* m2 = new MyStruct(22);
		dp::DataPointerShared<MyStruct> dp1("null", nullptr);
		dp::DataPointerShared<MyStruct> dp2("MyStruct", m2);
		dp1 = dp2;
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m3, m2, &utils::pointer_to_str);
		T_COMPARE(m4, m2, &utils::pointer_to_str);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(MyStruct)));
	}
}

void DataPointerSharedTests::copyAssignmentDeleterTest(test::Test& test) {
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
		dp::DataPointerShared<MyStruct, AnotherDeleter<MyStruct>> dp1("MyStruct", m1, cd);
		dp::DataPointerShared<MyStruct, CustomDeleter<MyStruct>> dp2("MyStruct", m2);
		dp1 = dp2;
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m3, m2, &utils::pointer_to_str);
		T_COMPARE(m4, m2, &utils::pointer_to_str);
		T_CHECK(flag2);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(MyStruct)));
	}

	T_CHECK(flag1);

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::copyAssignmentDerivedTest(test::Test& test) {
	bool flag = false;
	MyStruct* m1 = new MyStruct(11);
	m1->destructor_func = [&]() {
		flag = true;
	};
	ChildStruct* m2 = new ChildStruct(22);
	dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
	dp::DataPointerShared<ChildStruct> dp2("ChildStruct", m2);
	dp1 = dp2;
	MyStruct* m3 = dp1.get();
	ChildStruct* m4 = dp2.get();
	T_COMPARE(m3, m2, &utils::pointer_to_str);
	T_COMPARE(m4, m2, &utils::pointer_to_str);
	T_CHECK(flag);

	T_COMPARE(dp::data_blocks.size(), 1);
	T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(ChildStruct)));
}

void DataPointerSharedTests::copyAssignmentDerivedDeleterTest(test::Test& test) {
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
		dp::DataPointerShared<MyStruct, AnotherDeleter<MyStruct>> dp1("MyStruct", m1, cd);
		dp::DataPointerShared<ChildStruct, CustomDeleter<ChildStruct>> dp2("ChildStruct", m2);
		dp1 = dp2;
		MyStruct* m3 = dp1.get();
		ChildStruct* m4 = dp2.get();
		T_COMPARE(m3, m2, &utils::pointer_to_str);
		T_COMPARE(m4, m2, &utils::pointer_to_str);
		T_CHECK(flag2);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m3, sizeof(ChildStruct)));
	}
	T_CHECK(flag1);

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::copyTest(test::Test& test) {
	MyStruct* m1 = new MyStruct(44);
	{
		dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m1, sizeof(MyStruct)));
		T_COMPARE(dp1.use_count(), 1);
		{
			dp::DataPointerShared<MyStruct> dp2(dp1);
			T_COMPARE(dp1.use_count(), 2);
			T_COMPARE(dp2.use_count(), 2);
			T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m1, sizeof(MyStruct)));
		}
		T_COMPARE(dp1.use_count(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m1, sizeof(MyStruct)));
	}
	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::swapTest(test::Test& test) {
	{
		MyStruct* m1 = new MyStruct(123);
		MyStruct* m2 = new MyStruct(567);
		dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
		dp::DataPointerShared<MyStruct> dp2("MyStruct", m2);
		dp1.swap(dp2);
		T_COMPARE(dp1.get(), m2, &utils::pointer_to_str);
		T_COMPARE(dp2.get(), m1, &utils::pointer_to_str);

		T_COMPARE(dp::data_blocks.size(), 2);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m1, sizeof(MyStruct)));
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m2, sizeof(MyStruct)));
	}
	{
		// self with self
		MyStruct* m1 = new MyStruct(123);
		dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
		dp1.swap(dp1);
		T_COMPARE(dp1.get(), m1, &utils::pointer_to_str);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m1, sizeof(MyStruct)));
	}
	{
		// ptr with nullptr
		MyStruct* m1 = new MyStruct(123);
		dp::DataPointerShared<MyStruct> dp1("MyStruct", m1);
		dp::DataPointerShared<MyStruct> dp2("null", nullptr);
		dp1.swap(dp2);
		T_COMPARE(dp1.get(), nullptr, &utils::pointer_to_str);
		T_COMPARE(dp2.get(), m1, &utils::pointer_to_str);

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m1, sizeof(MyStruct)));
	}
}

void DataPointerSharedTests::dereferenceTest(test::Test& test) {
	MyStruct* m = new MyStruct(99);
	dp::DataPointerShared<MyStruct> dp("MyStruct", m);
	MyStruct mystruct = *dp;
	T_COMPARE(mystruct.val, 99);
}

void DataPointerSharedTests::pointerAccessTest(test::Test& test) {
	MyStruct* m = new MyStruct(111);
	dp::DataPointerShared<MyStruct> dp("MyStruct", m);
	int val = dp->val;
	T_COMPARE(val, 111);
}

void DataPointerSharedTests::moveTest(test::Test& test) {
	std::vector<dp::DataPointerShared<MyStruct>> vec;
	MyStruct* m = new MyStruct(222);
	{
		dp::DataPointerShared<MyStruct> dp("MyStruct", m);
		T_COMPARE(dp.use_count(), 1);
		vec.push_back(std::move(dp));
		MyStruct* m = dp.get();
		T_COMPARE(m, nullptr, &utils::pointer_to_str);
	}
	T_COMPARE(vec[0]->val, 222);

	T_COMPARE(dp::data_blocks.size(), 1);
	T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m, sizeof(MyStruct)));
}

void DataPointerSharedTests::makeDataPointerTest(test::Test& test) {
	{
		dp::DataPointerShared<MyStruct> dp = dp::make_shared_data_pointer<MyStruct>("MyStruct", 333);
		T_COMPARE(dp->val, 333);

		MyStruct* m = dp.get();

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m, sizeof(MyStruct)));
	}

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::makeDataPointerDerivedTest(test::Test& test) {
	{
		dp::DataPointerShared<MyStruct> dp = dp::make_shared_data_pointer<ChildStruct>("ChildStruct", 444);
		T_COMPARE(dp->val, 444);

		MyStruct* m = dp.get();

		T_COMPARE(dp::data_blocks.size(), 1);
		T_WRAP_CONTAINER(DataPointerTests::checkDataBlock(test, m, sizeof(ChildStruct)));
	}

	T_COMPARE(dp::data_blocks.size(), 0);
}

void DataPointerSharedTests::blockNameTest(test::Test& test) {
	{
		MyStruct* m1 = new MyStruct(11);
		CustomDeleter<MyStruct> cd([]() { });
		dp::DataPointerShared<MyStruct, CustomDeleter<MyStruct>> dp1("m1", m1, cd);
		dp::DataPointerShared<MyStruct> dp2 = dp::make_shared_data_pointer<MyStruct>("m2", 22);
		MyStruct* m2 = dp2.get();
		{
			auto it1 = dp::data_blocks.find(m1);
			auto it2 = dp::data_blocks.find(m2);
			T_COMPARE(it1->second.name, "m1");
			T_COMPARE(it2->second.name, "m2");
		}

		dp1.setName("dp1");
		{
			auto it = dp::data_blocks.find(m1);
			T_COMPARE(it->second.name, "dp1");
		}

		MyStruct* m3 = new MyStruct(33);
		dp1.reset("m3", m3);
		{
			auto it = dp::data_blocks.find(m3);
			T_COMPARE(it->second.name, "m3");
		}
	}

	{
		MyStruct* m4 = new MyStruct(44);
		MyStruct* m5 = new MyStruct(55);
		dp::DataPointerShared<MyStruct> dp4("m4", m4);
		dp::DataPointerShared<MyStruct> dp5("m5", m5);
		dp4 = std::move(dp5);
		{
			auto it = dp::data_blocks.find(m5);
			T_COMPARE(it->second.name, "m5");
		}
	}

	{
		MyStruct* m6 = new MyStruct(66);
		dp::DataPointerShared<MyStruct> dp6("m6", m6);
		dp::DataPointerShared<MyStruct> dp7(std::move(dp6));
		{
			auto it = dp::data_blocks.find(m6);
			T_COMPARE(it->second.name, "m6");
		}
	}

	{
		MyStruct* m4 = new MyStruct(44);
		MyStruct* m5 = new MyStruct(55);
		dp::DataPointerShared<MyStruct> dp4("m4", m4);
		dp::DataPointerShared<MyStruct> dp5("m5", m5);
		dp4 = dp5;
		{
			auto it = dp::data_blocks.find(m5);
			T_COMPARE(it->second.name, "m5");
		}
	}

	{
		MyStruct* m6 = new MyStruct(66);
		dp::DataPointerShared<MyStruct> dp6("m6", m6);
		dp::DataPointerShared<MyStruct> dp7(dp6);
		{
			auto it = dp::data_blocks.find(m6);
			T_COMPARE(it->second.name, "m6");
		}
	}

	{
		MyStruct* m8 = new MyStruct(88);
		MyStruct* m9 = new MyStruct(99);
		dp::DataPointerShared<MyStruct> dp8("m8", m8);
		dp::DataPointerShared<MyStruct> dp9("m9", m9);
		dp8.swap(dp9);
		{
			auto it1 = dp::data_blocks.find(m8);
			auto it2 = dp::data_blocks.find(m9);
			T_COMPARE(it1->second.name, "m8");
			T_COMPARE(it2->second.name, "m9");
		}
	}
}
