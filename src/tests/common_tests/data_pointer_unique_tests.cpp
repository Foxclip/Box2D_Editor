#include "tests/data_pointer_unique_tests.h"
#include "common/data_pointer_unique.h"
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

DataPointerUniqueTests::DataPointerUniqueTests(
	const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes
) : TestModule(name, parent, required_nodes) {
	OnAfterRunTest = [&]() {
		data_blocks.clear();
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
	test::Test* release_test = addTest("release", { get_test }, [&](test::Test& test) { releaseTest(test); });
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
	test::Test* swap_test = addTest("swap", { get_test }, [&](test::Test& test) { swapTest(test); });
	test::Test* dereference_test = addTest("dereference", { struct_test }, [&](test::Test& test) { dereferenceTest(test); });
	test::Test* pointer_access_test = addTest("pointer_access", { struct_test }, [&](test::Test& test) { pointerAccessTest(test); });
	test::Test* move_test = addTest("move", { struct_test }, [&](test::Test& test) { moveTest(test); });
	test::Test* make_data_pointer_test = addTest("make_data_pointer", { struct_test }, [&](test::Test& test) { makeDataPointerTest(test); });
	test::Test* make_data_pointer_derived_test = addTest("make_data_pointer_derived", { make_data_pointer_test }, [&](test::Test& test) { makeDataPointerDerivedTest(test); });
	test::Test* block_name_test = addTest("block_name", { make_data_pointer_test }, [&](test::Test& test) { blockNameTest(test); });
}

void DataPointerUniqueTests::nullTest(test::Test& test) {
	DataPointerUnique<int> dp("Null", nullptr);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerUniqueTests::basicTest(test::Test& test) {
	int* x = new int(5);
	DataPointerUnique<int> dp("x", x);

	T_WRAP_CONTAINER(checkDataBlock(test, x, sizeof(int)));
}

void DataPointerUniqueTests::structTest(test::Test& test) {
	MyStruct* m = new MyStruct(11);
	DataPointerUnique<MyStruct> dp("MyStruct", m);
	static_assert(sizeof(MyStruct) != sizeof(int));

	T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
}

void DataPointerUniqueTests::destructorTest(test::Test& test) {
	bool flag = false;
	MyStruct* m = new MyStruct(11);
	m->destructor_func = [&]() {
		flag = true;
	};
	{
		DataPointerUnique<MyStruct> dp("MyStruct", m);
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerUniqueTests::derivedDestructorTest(test::Test& test) {
	bool flag = false;
	{
		DataPointerUnique<MyStruct> dp = make_data_pointer<ChildStruct>("ChildStruct", 11);
		dp->destructor_func = [&]() {
			flag = true;
		};
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerUniqueTests::vectorDestructorTest(test::Test& test) {
	bool flag = false;
	MyStruct* m = new MyStruct(11);
	m->destructor_func = [&]() {
		flag = true;
	};
	std::vector<DataPointerUnique<MyStruct>> vec;
	DataPointerUnique<MyStruct> dp("MyStruct", m);
	DataPointerUnique<MyStruct> dp2;
	vec.push_back(std::move(dp));
	vec.push_back(std::move(dp2));
	vec.erase(vec.begin());
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerUniqueTests::customDeleterTest(test::Test& test) {
	MyStruct* m = new MyStruct(22);
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<MyStruct> cd(func);
	{
		DataPointerUnique<MyStruct, CustomDeleter<MyStruct>> dp("MyStruct", m, cd);
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerUniqueTests::lambdaDeleterTest(test::Test& test) {
	MyStruct* m = new MyStruct(22);
	bool flag = false;
	auto func = [&](MyStruct* ptr) {
		flag = true;
		delete ptr;
	};
	{
		DataPointerUnique<MyStruct, decltype(func)> dp("MyStruct", m, func);
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerUniqueTests::getTest(test::Test& test) {
	MyStruct* m = new MyStruct(33);
	DataPointerUnique<MyStruct> dp("MyStruct", m);
	MyStruct* m2 = dp.get();
	T_COMPARE(m2, m, &utils::pointer_to_str);
}

void DataPointerUniqueTests::releaseTest(test::Test& test) {
	MyStruct* m = new MyStruct(44);
	DataPointerUnique<MyStruct> dp("MyStruct", m);
	MyStruct* m2 = dp.release();
	T_COMPARE(m2, m, &utils::pointer_to_str);
	MyStruct* m3 = dp.get();
	T_COMPARE(m3, nullptr, &utils::pointer_to_str);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerUniqueTests::resetTest(test::Test& test) {
	MyStruct* m = new MyStruct(55);
	MyStruct* m2 = new MyStruct(66);
	DataPointerUnique<MyStruct> dp("MyStruct", m);
	MyStruct* mt1 = dp.get();
	T_COMPARE(mt1, m, &utils::pointer_to_str);
	dp.reset("m2", m2);
	MyStruct* mt2 = dp.get();
	T_COMPARE(mt2, m2, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m2, sizeof(MyStruct)));
}

void DataPointerUniqueTests::resetDeleterTest(test::Test& test) {
	bool flag = false;
	MyStruct* del_ptr = nullptr;
	auto deleter = [&](MyStruct* ptr) {
		flag = true;
		del_ptr = ptr;
		delete ptr;
	};
	MyStruct* m = new MyStruct(55);
	MyStruct* m2 = new MyStruct(66);
	DataPointerUnique<MyStruct, decltype(deleter)> dp("MyStruct", m, deleter);
	MyStruct* m3 = dp.get();
	T_COMPARE(m3, m, &utils::pointer_to_str);
	dp.reset("m2", m2);
	MyStruct* m4 = dp.get();
	T_COMPARE(m4, m2, &utils::pointer_to_str);
	T_CHECK(flag);
	T_COMPARE(del_ptr, m, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m2, sizeof(MyStruct)));
}

void DataPointerUniqueTests::moveConstructorTest(test::Test& test) {
	{
		// from ptr
		MyStruct* m = new MyStruct(11);
		DataPointerUnique<MyStruct> dp("MyStruct", m);
		DataPointerUnique<MyStruct> dp2(std::move(dp));
		MyStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, nullptr, &utils::pointer_to_str);
		T_COMPARE(m3, m, &utils::pointer_to_str);

		T_COMPARE(data_blocks.size(), 1);
		T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	{
		// from nullptr
		DataPointerUnique<MyStruct> dp("null", nullptr);
		DataPointerUnique<MyStruct> dp2(std::move(dp));
		MyStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, nullptr, &utils::pointer_to_str);
		T_COMPARE(m3, nullptr, &utils::pointer_to_str);

		T_COMPARE(data_blocks.size(), 0);
	}
}

void DataPointerUniqueTests::moveConstructorDeleterTest(test::Test& test) {
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<MyStruct> cd(func);
	{
		MyStruct* m = new MyStruct(11);
		DataPointerUnique<MyStruct, CustomDeleter<MyStruct>> dp("MyStruct", m, cd);
		DataPointerUnique<MyStruct, AnotherDeleter<MyStruct>> dp2(std::move(dp));
		MyStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, nullptr, &utils::pointer_to_str);

		T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	T_CHECK(flag);
}

void DataPointerUniqueTests::moveConstructorDerivedTest(test::Test& test) {
	ChildStruct* m = new ChildStruct(11);
	DataPointerUnique<ChildStruct> dp("ChildStruct", m);
	DataPointerUnique<MyStruct> dp2(std::move(dp));
	MyStruct* m2 = dp.get();
	MyStruct* m3 = dp2.get();
	T_COMPARE(m2, nullptr, &utils::pointer_to_str);

	T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(ChildStruct)));
}

void DataPointerUniqueTests::moveConstructorDerivedDeleterTest(test::Test& test) {
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<ChildStruct> cd(func);
	{
		ChildStruct* m = new ChildStruct(11);
		DataPointerUnique<ChildStruct, CustomDeleter<ChildStruct>> dp("ChildStruct", m, cd);
		DataPointerUnique<MyStruct, AnotherDeleter<MyStruct>> dp2(std::move(dp));
		ChildStruct* m2 = dp.get();
		MyStruct* m3 = dp2.get();
		T_COMPARE(m2, nullptr, &utils::pointer_to_str);

		T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(ChildStruct)));
	}
	T_CHECK(flag);
}

void DataPointerUniqueTests::moveAssignmentTest(test::Test& test) {
	{
		// ptr to ptr
		bool flag = false;
		MyStruct* m1 = new MyStruct(11);
		m1->destructor_func = [&]() {
			flag = true;
		};
		MyStruct* m2 = new MyStruct(22);
		DataPointerUnique<MyStruct> dp1("MyStruct", m1);
		DataPointerUnique<MyStruct> dp2("MyStruct", m2);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m3, m2, &utils::pointer_to_str);
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);
		T_CHECK(flag);

		T_COMPARE(data_blocks.size(), 1);
		T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	{
		// self to self
		bool flag = false;
		MyStruct* m1 = new MyStruct(11);
		m1->destructor_func = [&]() {
			flag = true;
		};
		DataPointerUnique<MyStruct> dp1("MyStruct", m1);
		dp1 = std::move(dp1);
		MyStruct* m2 = dp1.get();
		T_COMPARE(m2, m1, &utils::pointer_to_str);
		T_CHECK(!flag);

		T_COMPARE(data_blocks.size(), 1);
		T_WRAP_CONTAINER(checkDataBlock(test, m2, sizeof(MyStruct)));
	}
	{
		// nullptr to ptr
		bool flag = false;
		MyStruct* m1 = new MyStruct(11);
		m1->destructor_func = [&]() {
			flag = true;
		};
		DataPointerUnique<MyStruct> dp1("MyStruct", m1);
		DataPointerUnique<MyStruct> dp2("null", nullptr);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m3, nullptr, &utils::pointer_to_str);
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);
		T_CHECK(flag);

		T_COMPARE(data_blocks.size(), 0);
	}
	{
		// ptr to nullptr
		MyStruct* m2 = new MyStruct(22);
		DataPointerUnique<MyStruct> dp1("null", nullptr);
		DataPointerUnique<MyStruct> dp2("MyStruct", m2);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m3, m2, &utils::pointer_to_str);
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);

		T_COMPARE(data_blocks.size(), 1);
		T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(MyStruct)));
	}
}

void DataPointerUniqueTests::moveAssignmentDeleterTest(test::Test& test) {
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
		DataPointerUnique<MyStruct, AnotherDeleter<MyStruct>> dp1("MyStruct", m1, cd);
		DataPointerUnique<MyStruct, CustomDeleter<MyStruct>> dp2("MyStruct", m2);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		MyStruct* m4 = dp2.get();
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);
		T_CHECK(flag2);

		T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(MyStruct)));
	}
	T_CHECK(flag1);
}

void DataPointerUniqueTests::moveAssignmentDerivedTest(test::Test& test) {
	bool flag = false;
	MyStruct* m1 = new MyStruct(11);
	m1->destructor_func = [&]() {
		flag = true;
	};
	ChildStruct* m2 = new ChildStruct(22);
	DataPointerUnique<MyStruct> dp1("MyStruct", m1);
	DataPointerUnique<ChildStruct> dp2("ChildStruct", m2);
	dp1 = std::move(dp2);
	MyStruct* m3 = dp1.get();
	ChildStruct* m4 = dp2.get();
	T_COMPARE(m4, nullptr, &utils::pointer_to_str);
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 1);
	T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(ChildStruct)));
}

void DataPointerUniqueTests::moveAssignmentDerivedDeleterTest(test::Test& test) {
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
		DataPointerUnique<MyStruct, AnotherDeleter<MyStruct>> dp1("MyStruct", m1, cd);
		DataPointerUnique<ChildStruct, CustomDeleter<ChildStruct>> dp2("ChildStruct", m2);
		dp1 = std::move(dp2);
		MyStruct* m3 = dp1.get();
		ChildStruct* m4 = dp2.get();
		T_COMPARE(m4, nullptr, &utils::pointer_to_str);
		T_CHECK(flag2);

		T_WRAP_CONTAINER(checkDataBlock(test, m3, sizeof(ChildStruct)));
	}
	T_CHECK(flag1);
}

void DataPointerUniqueTests::swapTest(test::Test& test) {
	{
		// ptr with ptr
		MyStruct* m1 = new MyStruct(123);
		MyStruct* m2 = new MyStruct(567);
		DataPointerUnique<MyStruct> dp1("MyStruct", m1);
		DataPointerUnique<MyStruct> dp2("MyStruct", m2);
		dp1.swap(dp2);
		T_COMPARE(dp1.get(), m2, &utils::pointer_to_str);
		T_COMPARE(dp2.get(), m1, &utils::pointer_to_str);

		T_COMPARE(data_blocks.size(), 2);
		T_WRAP_CONTAINER(checkDataBlock(test, m1, sizeof(MyStruct)));
		T_WRAP_CONTAINER(checkDataBlock(test, m2, sizeof(MyStruct)));
	}
	{
		// self with self
		MyStruct* m1 = new MyStruct(123);
		DataPointerUnique<MyStruct> dp1("MyStruct", m1);
		dp1.swap(dp1);
		T_COMPARE(dp1.get(), m1, &utils::pointer_to_str);

		T_COMPARE(data_blocks.size(), 1);
		T_WRAP_CONTAINER(checkDataBlock(test, m1, sizeof(MyStruct)));
	}
	{
		// ptr with nullptr
		MyStruct* m1 = new MyStruct(123);
		DataPointerUnique<MyStruct> dp1("MyStruct", m1);
		DataPointerUnique<MyStruct> dp2("null", nullptr);
		dp1.swap(dp2);
		T_COMPARE(dp1.get(), nullptr, &utils::pointer_to_str);
		T_COMPARE(dp2.get(), m1, &utils::pointer_to_str);

		T_COMPARE(data_blocks.size(), 1);
		T_WRAP_CONTAINER(checkDataBlock(test, m1, sizeof(MyStruct)));
	}
}

void DataPointerUniqueTests::dereferenceTest(test::Test& test) {
	MyStruct* m = new MyStruct(99);
	DataPointerUnique<MyStruct> dp("MyStruct", m);
	MyStruct mystruct = *dp;
	T_COMPARE(mystruct.val, 99);
}

void DataPointerUniqueTests::pointerAccessTest(test::Test& test) {
	MyStruct* m = new MyStruct(111);
	DataPointerUnique<MyStruct> dp("MyStruct", m);
	int val = dp->val;
	T_COMPARE(val, 111);
}

void DataPointerUniqueTests::moveTest(test::Test& test) {
	std::vector<DataPointerUnique<MyStruct>> vec;
	MyStruct* m = new MyStruct(222);
	{
		DataPointerUnique<MyStruct> dp("MyStruct", m);
		vec.push_back(std::move(dp));
	}
	T_COMPARE(vec[0]->val, 222);

	T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
}

void DataPointerUniqueTests::makeDataPointerTest(test::Test& test) {
	{
		DataPointerUnique<MyStruct> dp = make_data_pointer<MyStruct>("MyStruct", 333);
		T_COMPARE(dp->val, 333);

		MyStruct* m = dp.get();
		T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
	}

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerUniqueTests::makeDataPointerDerivedTest(test::Test& test) {
	{
		DataPointerUnique<MyStruct> dp = make_data_pointer<ChildStruct>("ChildStruct", 444);
		T_COMPARE(dp->val, 444);

		MyStruct* m = dp.get();
		T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(ChildStruct)));
	}

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerUniqueTests::blockNameTest(test::Test& test) {

	{
		MyStruct* m1 = new MyStruct(11);
		CustomDeleter<MyStruct> cd([]() { });
		DataPointerUnique<MyStruct, CustomDeleter<MyStruct>> dp1("m1", m1, cd);
		DataPointerUnique<MyStruct> dp2 = make_data_pointer<MyStruct>("m2", 22);
		MyStruct* m2 = dp2.get();
		{
			auto it1 = data_blocks.find(m1);
			auto it2 = data_blocks.find(m2);
			T_COMPARE(it1->second.name, "m1");
			T_COMPARE(it2->second.name, "m2");
		}

		dp1.setName("dp1");
		{
			auto it = data_blocks.find(m1);
			T_COMPARE(it->second.name, "dp1");
		}

		MyStruct* m3 = new MyStruct(33);
		dp1.reset("m3", m3);
		{
			auto it = data_blocks.find(m3);
			T_COMPARE(it->second.name, "m3");
		}
	}

	{
		MyStruct* m4 = new MyStruct(44);
		MyStruct* m5 = new MyStruct(55);
		DataPointerUnique<MyStruct> dp4("m4", m4);
		DataPointerUnique<MyStruct> dp5("m5", m5);
		dp4 = std::move(dp5);
		{
			auto it = data_blocks.find(m5);
			T_COMPARE(it->second.name, "m5");
		}
	}

	{
		MyStruct* m6 = new MyStruct(66);
		DataPointerUnique<MyStruct> dp6("m6", m6);
		DataPointerUnique<MyStruct> dp7(std::move(dp6));
		{
			auto it = data_blocks.find(m6);
			T_COMPARE(it->second.name, "m6");
		}
	}

	{
		MyStruct* m8 = new MyStruct(88);
		MyStruct* m9 = new MyStruct(99);
		DataPointerUnique<MyStruct> dp8("m8", m8);
		DataPointerUnique<MyStruct> dp9("m9", m9);
		dp8.swap(dp9);
		{
			auto it1 = data_blocks.find(m8);
			auto it2 = data_blocks.find(m9);
			T_COMPARE(it1->second.name, "m8");
			T_COMPARE(it2->second.name, "m9");
		}
	}

}

void DataPointerUniqueTests::checkDataBlock(test::Test& test, void* p_block, size_t p_size) {
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

void DataPointerUniqueTests::checkNoDataBlock(test::Test& test, void* p_block) {
	auto it = data_blocks.find(p_block);
	T_CHECK(it == data_blocks.end());
}
