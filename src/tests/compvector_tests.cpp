#include "tests/compvector_tests.h"
#include <algorithm>
#include <ranges>

CompVectorTests::CompVectorTests(test::TestManager& manager) : TestModule("CompVector", manager) { }

void CompVectorTests::createTestLists() {
	test::TestList* compvector_test_list = createTestList("CompVector");
	test::TestList* compvector_uptr_test_list = createTestList("CompVectorUptr");
	createCompVectorList(compvector_test_list);
	createCompVectorUptrList(compvector_uptr_test_list);
}

void CompVectorTests::createCompVectorList(test::TestList* list) {
	test::Test* empty_vector_test = list->addTest("empty_vector", [&](test::Test& test) {
		CompVector<int> vec;
		T_COMPARE(vec.size(), 0);
	});
	test::Test* one_value_test = list->addTest("one_value", { empty_vector_test }, [&](test::Test& test) {
		CompVector<int> vec = { 5 };
		T_ASSERT(T_COMPARE(vec.size(), 1));
		T_COMPARE(vec[0], 5);
		T_COMPARE(vec.front(), 5);
		T_COMPARE(vec.back(), 5);
	});
	test::Test* multiple_values_test = list->addTest("multiple_values", { one_value_test }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		T_ASSERT(T_COMPARE(vec.size(), 3));
		T_COMPARE(vec[0], 1);
		T_COMPARE(vec[1], 2);
		T_COMPARE(vec[2], 3);
		T_COMPARE(vec.front(), 1);
		T_COMPARE(vec.back(), 3);
	});

	std::vector<test::Test*> values_tests = { empty_vector_test, one_value_test, multiple_values_test };

	test::Test* convert_to_vector_test = list->addTest("convert_to_vector", { values_tests }, [&](test::Test& test) {
		CompVector<int> cvec = { 1, 2, 3 };
		std::vector<int> vec = cvec;
		T_ASSERT(T_COMPARE(vec.size(), 3));
		T_COMPARE(vec[0], 1);
		T_COMPARE(vec[1], 2);
		T_COMPARE(vec[2], 3);
	});
	test::Test* compare_to_vector_test = list->addTest("compare_to_vector", { values_tests }, [&](test::Test& test) {
		CompVector<int> cvec = { 1, 2, 3 };
		std::vector<int> vec = { 1, 2, 3 };
		T_CHECK(cvec == vec);
		T_CHECK(vec == cvec);
	});
	test::Test* convert_from_vector_test = list->addTest("convert_from_vector", { compare_to_vector_test }, [&](test::Test& test) {
		std::vector<int> vec = { 1, 2, 3 };
		CompVector<int> cvec(vec);
		T_CHECK(vec == std::vector<int>({ 1, 2, 3 }));
	});
	test::Test* custom_compare_test = list->addTest("custom_compare", { values_tests }, [&](test::Test& test) {
		auto cmp = [](int left, int right) {
			return left > right;
		};
		CompVector<int, decltype(cmp)> vec = { 1, 2, 3 };
		std::vector<int> set_vec = std::vector<int>(vec.getSet().begin(), vec.getSet().end());
		T_CHECK(set_vec == std::vector<int>({ 3, 2, 1 }));
	});

	std::vector<test::Test*> basic_tests = list->getTestList();

	test::Test* add_test = list->addTest("add", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec;
		vec.add(1);
		vec.add(2);
		vec.add(3);
		T_CHECK(vec == std::vector<int>({ 1, 2, 3 }));
	});
	test::Test* insert_value_test = list->addTest("insert_value", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.insert(vec.begin() + 2, 5);
		T_CHECK(vec == std::vector<int>({ 1, 2, 5, 3 }));
		vec.insert(vec.begin() + 2, 5);
		T_CHECK(vec == std::vector<int>({ 1, 2, 5, 3 }));
	});
	test::Test* insert_range_test = list->addTest("insert_range", { insert_value_test }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		std::set<int> set1 = { 5, 6, 7 };
		vec.insert(vec.begin() + 2, set1.begin(), set1.end());
		T_CHECK(vec == std::vector<int>({ 1, 2, 5, 6, 7, 3 }));
		vec.insert(vec.begin() + 2, set1.begin(), set1.end());
		T_CHECK(vec == std::vector<int>({ 1, 2, 5, 6, 7, 3 }));
		std::set<int> set2 = { 7, 8, 9 };
		vec.insert(vec.begin(), set2.begin(), set2.end());
		T_CHECK(vec == std::vector<int>({ 8, 9, 1, 2, 5, 6, 7, 3 }));
	});
	test::Test* remove_test = list->addTest("remove", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.remove(2);
		T_CHECK(vec == std::vector<int>({ 1, 3 }));
	});
	test::Test* remove_at_test = list->addTest("remove_at", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.removeAt(1);
		T_CHECK(vec == std::vector<int>({ 1, 3 }));
	});
	test::Test* reverse_test = list->addTest("reverse", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.reverse();
		T_CHECK(vec == std::vector<int>({ 3, 2, 1 }));
	});
	test::Test* iterators_test = list->addTest("iterators", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		{
			auto it = vec.begin();
			T_COMPARE(*it, 1);
			T_COMPARE(*++it, 2);
			T_COMPARE(*++it, 3);
			T_CHECK(++it == vec.end());
		}
		{
			auto it = vec.end();
			T_COMPARE(*--it, 3);
			T_COMPARE(*--it, 2);
			T_COMPARE(*--it, 1);
			T_CHECK(it == vec.begin());
		}
		{
			auto it = vec.rbegin();
			T_COMPARE(*it, 3);
			T_COMPARE(*++it, 2);
			T_COMPARE(*++it, 1);
			T_CHECK(++it == vec.rend());
		}
		{
			auto it = vec.rend();
			T_COMPARE(*--it, 1);
			T_COMPARE(*--it, 2);
			T_COMPARE(*--it, 3);
			T_CHECK(it == vec.rbegin());
		}
		{
			for (int value : vec) {
				T_CHECK(value > 0);
			}
			for (int value : vec | std::views::reverse) {
				T_CHECK(value > 0);
			}
		}
	});
	test::Test* at_test = list->addTest("at", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		T_COMPARE(vec.at(1), 2);
	});
	test::Test* get_index_test = list->addTest("get_index", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		T_COMPARE(vec.getIndex(2), 1);
	});
	test::Test* get_vector_test = list->addTest("get_vector", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		T_CHECK(vec.getVector() == std::vector<int>({ 1, 2, 3 }));
	});
	test::Test* get_set_test = list->addTest("get_set", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		T_CHECK(vec.getSet() == std::set<int>({ 1, 2, 3 }));
	});
	test::Test* square_brackets_test = list->addTest("square_brackets", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		T_COMPARE(vec[1], 2);
	});
	test::Test* find_test = list->addTest("find", { get_set_test }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		auto it2 = vec.find(2);
		auto it5 = vec.find(5);
		T_CHECK(it5 == vec.getSet().end());
		T_COMPARE(*it2, 2);
	});
	test::Test* contains_test = list->addTest("contains", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		T_CHECK(vec.contains(2));
		T_CHECK(!vec.contains(5));
	});
	test::Test* clear_test = list->addTest("clear", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.clear();
		T_ASSERT(T_COMPARE(vec.size(), 0));
	});
	test::Test* equals_test = list->addTest("equals", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		T_CHECK(vec == CompVector<int>({ 1, 2, 3 }));
		T_CHECK(vec != CompVector<int>({ 1, 2 }));
		T_CHECK(vec != CompVector<int>({ 1, 2, 3, 4 }));
		T_CHECK(vec != CompVector<int>({ 1, 5, 3 }));
	});
	test::Test* duplicates_test = list->addTest("duplicates", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 2, 3 };
		T_CHECK(vec == CompVector<int>({ 1, 2, 3 }));
	});
	test::Test* remove_missing_test = list->addTest("remove_missing", { remove_test }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.remove(5);
		T_CHECK(vec == CompVector<int>({ 1, 2, 3 }));
	});
	test::Test* modify_test = list->addTest("modify", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec[1] = 5;
		T_CHECK(vec == CompVector<int>({ 1, 5, 3 }));
	});
	test::Test* modify_cycle_test = list->addTest("modify_cycle", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		for (int& n : vec) {
			n++;
		}
		T_CHECK(vec == CompVector<int>({ 2, 3, 4 }));
	});
}

void CompVectorTests::createCompVectorUptrList(test::TestList* list) {
	test::Test* empty_vector_test = list->addTest("empty_vector", [&](test::Test& test) {
		CompVectorUptr<int> vec;
		T_COMPARE(vec.size(), 0);
	});
	test::Test* one_value_test = list->addTest("one_value", [&](test::Test& test) {
		CompVectorUptr<int> vec = { 5 };
		T_ASSERT(T_COMPARE(vec.size(), 1));
		T_COMPARE(*vec[0], 5);
		T_COMPARE(*vec.front(), 5);
		T_COMPARE(*vec.back(), 5);
	});
	test::Test* multiple_values_test = list->addTest("multiple_values", [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		T_ASSERT(T_COMPARE(vec.size(), 3));
		T_COMPARE(*vec[0], 1);
		T_COMPARE(*vec[1], 2);
		T_COMPARE(*vec[2], 3);
		T_COMPARE(*vec.front(), 1);
		T_COMPARE(*vec.back(), 3);
	});

	std::vector<test::Test*> values_tests = { empty_vector_test, one_value_test, multiple_values_test };

	test::Test* convert_to_vector_test = list->addTest("convert_to_vector", { values_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompVectorUptr<int> cvec = { ptr1, ptr2, ptr3 };
		std::vector<int*> vec = cvec;
		T_CHECK(vec == std::vector<int*>({ ptr1, ptr2, ptr3 }));
	});
	test::Test* compare_to_vector_test = list->addTest("compare_to_vector", { values_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompVectorUptr<int> cvec = { ptr1, ptr2, ptr3 };
		std::vector<int*> vec = { ptr1, ptr2, ptr3 };
		T_CHECK(cvec == vec);
		T_CHECK(vec == cvec);
	});
	test::Test* custom_compare_test = list->addTest("custom_compare", { compare_to_vector_test }, [&](test::Test& test) {
		auto cmp = [](const int* left, const int* right) {
			return *left > *right;
		};
		CompVectorUptr<int, decltype(cmp)> vec = { 1, 2, 3 };
		std::vector<int*> set_vec = std::vector<int*>(vec.getSet().begin(), vec.getSet().end());
		std::vector<int> set_values;
		for (int* ptr : set_vec) {
			set_values.push_back(*ptr);
		}
		T_CHECK(set_values == std::vector<int>({ 3, 2, 1 }));
	});

	std::vector<test::Test*> basic_tests = list->getTestList();

	test::Test* add_test = list->addTest("add", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec;
		int* ptr1 = vec.add(1);
		int* ptr2 = vec.add(2);
		int* ptr3 = vec.add(3);
		T_ASSERT(T_COMPARE(vec.size(), 3));
		T_COMPARE(*vec[0], 1);
		T_COMPARE(*vec[1], 2);
		T_COMPARE(*vec[2], 3);
		T_COMPARE(*vec.front(), 1);
		T_COMPARE(*vec.back(), 3);
		T_COMPARE(*ptr1, 1);
		T_COMPARE(*ptr2, 2);
		T_COMPARE(*ptr3, 3);
	});
	test::Test* insert_value_test = list->addTest("insert_value", { basic_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompVectorUptr<int> vec = { ptr1, ptr2, ptr3 };
		std::unique_ptr<int> uptr5(ptr5);
		std::unique_ptr<int> uptr5_another(ptr5);
		vec.insert(vec.begin() + 2, std::move(uptr5));
		T_CHECK(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr3 }));
		vec.insert(vec.begin() + 2, std::move(uptr5_another));
		T_CHECK(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr3 }));
		uptr5_another.release();
	});
	test::Test* insert_range_test = list->addTest("insert_range", { insert_value_test }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr4 = new int(4);
		int* ptr5 = new int(5);
		int* ptr6 = new int(6);
		int* ptr7 = new int(7);
		int* ptr8 = new int(8);
		int* ptr9 = new int(9);
		CompVectorUptr<int> vec = { ptr1, ptr2, ptr3 };
		std::unique_ptr<int> uptr5(ptr5);
		std::unique_ptr<int> uptr6(ptr6);
		std::unique_ptr<int> uptr7(ptr7);
		std::unique_ptr<int> uptr5_another(ptr5);
		std::unique_ptr<int> uptr6_another(ptr6);
		std::unique_ptr<int> uptr7_another(ptr7);
		std::vector<std::unique_ptr<int>> vec1;
		std::vector<std::unique_ptr<int>> vec1_another;
		vec1.push_back(std::move(uptr5));
		vec1.push_back(std::move(uptr6));
		vec1.push_back(std::move(uptr7));
		vec1_another.push_back(std::move(uptr5_another));
		vec1_another.push_back(std::move(uptr6_another));
		vec1_another.push_back(std::move(uptr7_another));
		vec.insert(vec.begin() + 2, vec1.begin(), vec1.end());
		T_CHECK(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr6, ptr7, ptr3 }));
		vec.insert(vec.begin() + 2, vec1_another.begin(), vec1_another.end());
		T_CHECK(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr6, ptr7, ptr3 }));
		std::unique_ptr<int> uptr7_2(ptr7);
		std::unique_ptr<int> uptr8(ptr8);
		std::unique_ptr<int> uptr9(ptr9);
		std::vector<std::unique_ptr<int>> vec2;
		vec2.push_back(std::move(uptr7_2));
		vec2.push_back(std::move(uptr8));
		vec2.push_back(std::move(uptr9));
		vec.insert(vec.begin(), vec2.begin(), vec2.end());
		T_CHECK(vec == std::vector<int*>({ ptr8, ptr9, ptr1, ptr2, ptr5, ptr6, ptr7, ptr3 }));
		uptr5_another.release();
		uptr6_another.release();
		uptr7_another.release();
		uptr7_2.release();
	});
	test::Test* remove_test = list->addTest("remove", { basic_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		vec.remove(ptr2);
		T_ASSERT(T_COMPARE(vec.size(), 2));
		T_COMPARE(*vec[0], 1);
		T_COMPARE(*vec[1], 3);
		T_COMPARE(*vec.front(), 1);
		T_COMPARE(*vec.back(), 3);
	});
	test::Test* remove_at_test = list->addTest("remove_at", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		vec.removeAt(1);
		T_ASSERT(T_COMPARE(vec.size(), 2));
		T_COMPARE(*vec[0], 1);
		T_COMPARE(*vec[1], 3);
		T_COMPARE(*vec.front(), 1);
		T_COMPARE(*vec.back(), 3);
	});
	test::Test* reverse_test = list->addTest("reverse", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		vec.reverse();
		T_ASSERT(T_COMPARE(vec.size(), 3));
		T_COMPARE(*vec[0], 3);
		T_COMPARE(*vec[1], 2);
		T_COMPARE(*vec[2], 1);
		T_COMPARE(*vec.front(), 3);
		T_COMPARE(*vec.back(), 1);
	});
	test::Test* iterators_test = list->addTest("iterators", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		{
			auto it = vec.begin();
			T_COMPARE(**it, 1);
			T_COMPARE(**++it, 2);
			T_COMPARE(**++it, 3);
			T_CHECK(++it == vec.end());
		}
		{
			auto it = vec.end();
			T_COMPARE(**--it, 3);
			T_COMPARE(**--it, 2);
			T_COMPARE(**--it, 1);
			T_CHECK(it == vec.begin());
		}
		{
			auto it = vec.rbegin();
			T_COMPARE(**it, 3);
			T_COMPARE(**++it, 2);
			T_COMPARE(**++it, 1);
			T_CHECK(++it == vec.rend());
		}
		{
			auto it = vec.rend();
			T_COMPARE(**--it, 1);
			T_COMPARE(**--it, 2);
			T_COMPARE(**--it, 3);
			T_CHECK(it == vec.rbegin());
		}
		{
			for (int* value : vec) {
				T_CHECK(value);
			}
			for (int* value : vec | std::views::reverse) {
				T_CHECK(value);
			}
		}
	});
	test::Test* at_test = list->addTest("at", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		T_COMPARE(*vec.at(1), 2);
	});
	test::Test* get_index_test = list->addTest("get_index", { basic_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		T_COMPARE(vec.getIndex(ptr2), 1);
	});
	test::Test* get_vector_test = list->addTest("get_vector", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		std::vector<int*> get_vec = vec.getVector();
		std::vector<int> value_vec;
		for (size_t i = 0; i < get_vec.size(); i++) {
			value_vec.push_back(*get_vec[i]);
		}
		T_CHECK(value_vec == std::vector<int>({ 1, 2, 3 }));
	});
	test::Test* get_set_test = list->addTest("get_set", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		std::set<int*> get_set = vec.getSet();
		std::vector<int> value_vec;
		for (int* ptr : get_set) {
			value_vec.push_back(*ptr);
		}
		std::sort(value_vec.begin(), value_vec.end());
		T_CHECK(value_vec == std::vector<int>({ 1, 2, 3 }));
	});
	test::Test* square_brackets_test = list->addTest("square_brackets", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		T_COMPARE(*vec[1], 2);
	});
	test::Test* find_test = list->addTest("find", { get_set_test }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		T_CHECK(vec.find(ptr2) != vec.getSet().end());
		T_CHECK(vec.find(ptr5) == vec.getSet().end());
	});
	test::Test* contains_test = list->addTest("contains", { basic_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		T_CHECK(vec.contains(ptr2));
		T_CHECK(!vec.contains(ptr5));
	});
	test::Test* clear_test = list->addTest("clear", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		vec.clear();
		T_ASSERT(T_COMPARE(vec.size(), 0));
	});
	test::Test* no_duplicates_test = list->addTest("duplicates", { basic_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr2, ptr3 });
		T_ASSERT(T_COMPARE(vec.size(), 3));
		T_COMPARE(*vec[0], 1);
		T_COMPARE(*vec[1], 2);
		T_COMPARE(*vec[2], 3);
	});
	test::Test* remove_missing_test = list->addTest("remove_missing", { remove_test }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		vec.remove(ptr5);
		T_ASSERT(T_COMPARE(vec.size(), 3));
		T_COMPARE(*vec[0], 1);
		T_COMPARE(*vec[1], 2);
		T_COMPARE(*vec[2], 3);
	});
	test::Test* modify_test = list->addTest("modify", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		*vec[1] = 5;
		T_COMPARE(*vec[0], 1);
		T_COMPARE(*vec[1], 5);
		T_COMPARE(*vec[2], 3);
	});
	test::Test* modify_cycle_test = list->addTest("modify_cycle", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		for (int* n : vec) {
			(*n)++;
		}
		T_COMPARE(*vec[0], 2);
		T_COMPARE(*vec[1], 3);
		T_COMPARE(*vec[2], 4);
	});
}
