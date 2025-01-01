#include "tests/data_pointer_shared_tests.h"
#include "common/data_pointer_shared.h"
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

DataPointerSharedTests::DataPointerSharedTests(
	const std::string& name,
	test::TestModule* parent,
	const std::vector<TestNode*>& required_nodes
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
	test::Test* copy_test = addTest("copy", { get_test }, [&](test::Test& test) { copyTest(test); });
}

void DataPointerSharedTests::nullTest(test::Test& test) {
	DataPointerShared<int> dp("Null", nullptr);
	T_COMPARE(dp.use_count(), 0);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerSharedTests::basicTest(test::Test& test) {
	int* x = new int(5);
	DataPointerShared<int> dp("x", x);
	T_COMPARE(dp.use_count(), 1);

	T_WRAP_CONTAINER(checkDataBlock(test, x, sizeof(int)));
}

void DataPointerSharedTests::structTest(test::Test& test) {
	MyStruct* m = new MyStruct(11);
	DataPointerShared<MyStruct> dp("MyStruct", m);
	static_assert(sizeof(MyStruct) != sizeof(int));

	T_WRAP_CONTAINER(checkDataBlock(test, m, sizeof(MyStruct)));
}

void DataPointerSharedTests::destructorTest(test::Test& test) {
	bool flag = false;
	MyStruct* m = new MyStruct(11);
	m->destructor_func = [&]() {
		flag = true;
	};
	{
		DataPointerShared<MyStruct> dp("MyStruct", m);
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerSharedTests::derivedDestructorTest(test::Test& test) {
	bool flag = false;
	{
		DataPointerShared<MyStruct> dp = make_shared_data_pointer<ChildStruct>("ChildStruct", 11);
		dp->destructor_func = [&]() {
			flag = true;
		};
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerSharedTests::vectorDestructorTest(test::Test& test) {
	bool flag = false;
	MyStruct* m = new MyStruct(11);
	m->destructor_func = [&]() {
		flag = true;
	};
	std::vector<DataPointerShared<MyStruct>> vec;
	DataPointerShared<MyStruct> dp("MyStruct", m);
	DataPointerShared<MyStruct> dp2;
	vec.push_back(std::move(dp));
	vec.push_back(std::move(dp2));
	vec.erase(vec.begin());
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerSharedTests::customDeleterTest(test::Test& test) {
	MyStruct* m = new MyStruct(22);
	bool flag = false;
	auto func = [&]() {
		flag = true;
	};
	CustomDeleter<MyStruct> cd(func);
	{
		DataPointerShared<MyStruct, CustomDeleter<MyStruct>> dp("MyStruct", m, cd);
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerSharedTests::lambdaDeleterTest(test::Test& test) {
	MyStruct* m = new MyStruct(22);
	bool flag = false;
	auto func = [&](MyStruct* ptr) {
		flag = true;
		delete ptr;
	};
	{
		DataPointerShared<MyStruct, decltype(func)> dp("MyStruct", m, func);
	}
	T_CHECK(flag);

	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerSharedTests::getTest(test::Test& test) {
	MyStruct* m = new MyStruct(33);
	DataPointerShared<MyStruct> dp("MyStruct", m);
	MyStruct* m2 = dp.get();
	T_COMPARE(m2, m, &utils::pointer_to_str);
}

void DataPointerSharedTests::copyTest(test::Test& test) {
	MyStruct* m1 = new MyStruct(44);
	{
		DataPointerShared<MyStruct> dp1("MyStruct", m1);
		T_WRAP_CONTAINER(checkDataBlock(test, m1, sizeof(MyStruct)));
		T_COMPARE(dp1.use_count(), 1);
		{
			DataPointerShared<MyStruct> dp2(dp1);
			T_COMPARE(dp1.use_count(), 2);
			T_COMPARE(dp2.use_count(), 2);
			T_WRAP_CONTAINER(checkDataBlock(test, m1, sizeof(MyStruct)));
		}
		T_COMPARE(dp1.use_count(), 1);
		T_WRAP_CONTAINER(checkDataBlock(test, m1, sizeof(MyStruct)));
	}
	T_COMPARE(data_blocks.size(), 0);
}

void DataPointerSharedTests::checkDataBlock(test::Test& test, void* p_block, size_t p_size) {
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

void DataPointerSharedTests::checkNoDataBlock(test::Test& test, void* p_block) {
	auto it = data_blocks.find(p_block);
	T_CHECK(it == data_blocks.end());
}
