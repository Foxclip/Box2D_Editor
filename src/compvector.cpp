#ifndef NDEBUG

#include "compvector.h"
#include <cassert>
#include <algorithm>
#include <ranges>

CompVectorTests::CompVectorTests() : TestModule("CompVector") { }

void CompVectorTests::createTestLists() {
	CompVectorTestList compvector_test_list;
	CompVectorUptrTestList compvector_uptr_test_list;
	test_lists.push_back(std::move(compvector_test_list));
	test_lists.push_back(std::move(compvector_uptr_test_list));
}

CompVectorTests::CompVectorTestList::CompVectorTestList() : TestList("CompVector") {
	test::Test* empty_vector_test = addTest("empty_vector", [&](test::Test& test) {
		CompVector<int> vec;
		tCheck(vec.size() == 0);
	});
	test::Test* one_value_test = addTest("one_value", { empty_vector_test }, [&](test::Test& test) {
		CompVector<int> vec = { 5 };
		tAssert(vec.size() == 1);
		tCheck(vec[0] == 5);
		tCheck(vec.front() == 5);
		tCheck(vec.back() == 5);
	});
	test::Test* multiple_values_test = addTest("multiple_values", { one_value_test }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		tAssert(vec.size() == 3);
		tCheck(vec[0] == 1);
		tCheck(vec[1] == 2);
		tCheck(vec[2] == 3);
		tCheck(vec.front() == 1);
		tCheck(vec.back() == 3);
	});

	std::vector<test::Test*> values_tests = { empty_vector_test, one_value_test, multiple_values_test };

	test::Test* convert_to_vector_test = addTest("convert_to_vector", { values_tests }, [&](test::Test& test) {
		CompVector<int> cvec = { 1, 2, 3 };
		std::vector<int> vec = cvec;
		tAssert(vec.size() == 3);
		tCheck(vec[0] == 1);
		tCheck(vec[1] == 2);
		tCheck(vec[2] == 3);
	});
	test::Test* compare_to_vector_test = addTest("compare_to_vector", { values_tests }, [&](test::Test& test) {
		CompVector<int> cvec = { 1, 2, 3 };
		std::vector<int> vec = { 1, 2, 3 };
		tCheck(cvec == vec);
		tCheck(vec == cvec);
	});
	test::Test* convert_from_vector_test = addTest("convert_from_vector", { compare_to_vector_test }, [&](test::Test& test) {
		std::vector<int> vec = { 1, 2, 3 };
		CompVector<int> cvec(vec);
		tCheck(vec == std::vector<int>({ 1, 2, 3 }));
	});
	test::Test* custom_compare_test = addTest("custom_compare", { values_tests }, [&](test::Test& test) {
		auto cmp = [](int left, int right) {
			return left > right;
		};
		CompVector<int, decltype(cmp)> vec = { 1, 2, 3 };
		std::vector<int> set_vec = std::vector<int>(vec.getSet().begin(), vec.getSet().end());
		tCheck(set_vec == std::vector<int>({ 3, 2, 1 }));
	});

	std::vector<test::Test*> basic_tests = getTestList();

	test::Test* add_test = addTest("add", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec;
		vec.add(1);
		vec.add(2);
		vec.add(3);
		tCheck(vec == std::vector<int>({ 1, 2, 3 }));
	});
	test::Test* insert_value_test = addTest("insert_value", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.insert(vec.begin() + 2, 5);
		tCheck(vec == std::vector<int>({ 1, 2, 5, 3 }));
		vec.insert(vec.begin() + 2, 5);
		tCheck(vec == std::vector<int>({ 1, 2, 5, 3 }));
	});
	test::Test* insert_range_test = addTest("insert_range", { insert_value_test }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		std::set<int> set1 = { 5, 6, 7 };
		vec.insert(vec.begin() + 2, set1.begin(), set1.end());
		tCheck(vec == std::vector<int>({ 1, 2, 5, 6, 7, 3 }));
		vec.insert(vec.begin() + 2, set1.begin(), set1.end());
		tCheck(vec == std::vector<int>({ 1, 2, 5, 6, 7, 3 }));
		std::set<int> set2 = { 7, 8, 9 };
		vec.insert(vec.begin(), set2.begin(), set2.end());
		tCheck(vec == std::vector<int>({ 8, 9, 1, 2, 5, 6, 7, 3 }));
	});
	test::Test* remove_test = addTest("remove", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.remove(2);
		tCheck(vec == std::vector<int>({ 1, 3 }));
	});
	test::Test* remove_at_test = addTest("remove_at", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.removeAt(1);
		tCheck(vec == std::vector<int>({ 1, 3 }));
	});
	test::Test* reverse_test = addTest("reverse", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.reverse();
		tCheck(vec == std::vector<int>({ 3, 2, 1 }));
	});
	test::Test* iterators_test = addTest("iterators", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		{
			auto it = vec.begin();
			tCheck(*it == 1);
			tCheck(*++it == 2);
			tCheck(*++it == 3);
			tCheck(++it == vec.end());
		}
		{
			auto it = vec.end();
			tCheck(*--it == 3);
			tCheck(*--it == 2);
			tCheck(*--it == 1);
			tCheck(it == vec.begin());
		}
		{
			auto it = vec.rbegin();
			tCheck(*it == 3);
			tCheck(*++it == 2);
			tCheck(*++it == 1);
			tCheck(++it == vec.rend());
		}
		{
			auto it = vec.rend();
			tCheck(*--it == 1);
			tCheck(*--it == 2);
			tCheck(*--it == 3);
			tCheck(it == vec.rbegin());
		}
		{
			for (int value : vec) {
				tCheck(value > 0);
			}
			for (int value : vec | std::views::reverse) {
				tCheck(value > 0);
			}
		}
	});
	test::Test* at_test = addTest("at", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		tCheck(vec.at(1) == 2);
	});
	test::Test* get_index_test = addTest("get_index", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		tCheck(vec.getIndex(2) == 1);
	});
	test::Test* get_vector_test = addTest("get_vector", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		tCheck(vec.getVector() == std::vector<int>({ 1, 2, 3 }));
	});
	test::Test* get_set_test = addTest("get_set", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		tCheck(vec.getSet() == std::set<int>({ 1, 2, 3 }));
	});
	test::Test* square_brackets_test = addTest("square_brackets", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		tCheck(vec[1] == 2);
	});
	test::Test* find_test = addTest("find", { get_set_test }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		tCheck(*vec.find(2) == 2);
		tCheck(vec.find(5) == vec.getSet().end());
	});
	test::Test* contains_test = addTest("contains", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		tCheck(vec.contains(2));
		tCheck(!vec.contains(5));
	});
	test::Test* clear_test = addTest("clear", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.clear();
		tCheck(vec.size() == 0);
	});
	test::Test* equals_test = addTest("equals", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		tCheck(vec == CompVector<int>({ 1, 2, 3 }));
		tCheck(vec != CompVector<int>({ 1, 2 }));
		tCheck(vec != CompVector<int>({ 1, 2, 3, 4 }));
		tCheck(vec != CompVector<int>({ 1, 5, 3 }));
	});
	test::Test* duplicates_test = addTest("duplicates", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 2, 3 };
		tCheck(vec == CompVector<int>({ 1, 2, 3 }));
	});
	test::Test* remove_missing_test = addTest("remove_missing", { remove_test }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec.remove(5);
		tCheck(vec == CompVector<int>({ 1, 2, 3 }));
	});
	test::Test* modify_test = addTest("modify", { basic_tests }, [&](test::Test& test) {
		CompVector<int> vec = { 1, 2, 3 };
		vec[1] = 5;
		tCheck(vec == CompVector<int>({ 1, 5, 3 }));
	});
}

CompVectorTests::CompVectorUptrTestList::CompVectorUptrTestList() : TestList("CompVectorUptr") {
	test::Test* empty_vector_test = addTest("empty_vector", [&](test::Test& test) {
		CompVectorUptr<int> vec;
		tAssert(vec.size() == 0);
	});
	test::Test* one_value_test = addTest("one_value", [&](test::Test& test) {
		CompVectorUptr<int> vec = { 5 };
		tAssert(vec.size() == 1);
		tCheck(*vec[0] == 5);
		tCheck(*vec.front() == 5);
		tCheck(*vec.back() == 5);
	});
	test::Test* multiple_values_test = addTest("multiple_values", [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		tAssert(vec.size() == 3);
		tCheck(*vec[0] == 1);
		tCheck(*vec[1] == 2);
		tCheck(*vec[2] == 3);
		tCheck(*vec.front() == 1);
		tCheck(*vec.back() == 3);
	});

	std::vector<test::Test*> values_tests = { empty_vector_test, one_value_test, multiple_values_test };

	test::Test* convert_to_vector_test = addTest("convert_to_vector", { values_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompVectorUptr<int> cvec = { ptr1, ptr2, ptr3 };
		std::vector<int*> vec = cvec;
		tCheck(vec == std::vector<int*>({ ptr1, ptr2, ptr3 }));
	});
	test::Test* compare_to_vector_test = addTest("compare_to_vector", { values_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompVectorUptr<int> cvec = { ptr1, ptr2, ptr3 };
		std::vector<int*> vec = { ptr1, ptr2, ptr3 };
		tCheck(cvec == vec);
		tCheck(vec == cvec);
	});
	test::Test* custom_compare_test = addTest("custom_compare", { compare_to_vector_test }, [&](test::Test& test) {
		auto cmp = [](const int* left, const int* right) {
			return *left > *right;
		};
		CompVectorUptr<int, decltype(cmp)> vec = { 1, 2, 3 };
		std::vector<int*> set_vec = std::vector<int*>(vec.getSet().begin(), vec.getSet().end());
		std::vector<int> set_values;
		for (int* ptr : set_vec) {
			set_values.push_back(*ptr);
		}
		tCheck(set_values == std::vector<int>({ 3, 2, 1 }));
	});

	std::vector<test::Test*> basic_tests = getTestList();

	test::Test* add_test = addTest("add", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec;
		int* ptr1 = vec.add(1);
		int* ptr2 = vec.add(2);
		int* ptr3 = vec.add(3);
		tAssert(vec.size() == 3);
		tCheck(*vec[0] == 1);
		tCheck(*vec[1] == 2);
		tCheck(*vec[2] == 3);
		tCheck(*vec.front() == 1);
		tCheck(*vec.back() == 3);
		tCheck(*ptr1 == 1);
		tCheck(*ptr2 == 2);
		tCheck(*ptr3 == 3);
	});
	test::Test* insert_value_test = addTest("insert_value", { basic_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompVectorUptr<int> vec = { ptr1, ptr2, ptr3 };
		std::unique_ptr<int> uptr5(ptr5);
		std::unique_ptr<int> uptr5_another(ptr5);
		vec.insert(vec.begin() + 2, std::move(uptr5));
		tCheck(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr3 }));
		vec.insert(vec.begin() + 2, std::move(uptr5_another));
		tCheck(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr3 }));
		uptr5_another.release();
	});
	test::Test* insert_range_test = addTest("insert_range", { insert_value_test }, [&](test::Test& test) {
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
		tCheck(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr6, ptr7, ptr3 }));
		vec.insert(vec.begin() + 2, vec1_another.begin(), vec1_another.end());
		tCheck(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr6, ptr7, ptr3 }));
		std::unique_ptr<int> uptr7_2(ptr7);
		std::unique_ptr<int> uptr8(ptr8);
		std::unique_ptr<int> uptr9(ptr9);
		std::vector<std::unique_ptr<int>> vec2;
		vec2.push_back(std::move(uptr7_2));
		vec2.push_back(std::move(uptr8));
		vec2.push_back(std::move(uptr9));
		vec.insert(vec.begin(), vec2.begin(), vec2.end());
		tCheck(vec == std::vector<int*>({ ptr8, ptr9, ptr1, ptr2, ptr5, ptr6, ptr7, ptr3 }));
		uptr5_another.release();
		uptr6_another.release();
		uptr7_another.release();
		uptr7_2.release();
	});
	test::Test* remove_test = addTest("remove", { basic_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		vec.remove(ptr2);
		tAssert(vec.size() == 2);
		tCheck(*vec[0] == 1);
		tCheck(*vec[1] == 3);
		tCheck(*vec.front() == 1);
		tCheck(*vec.back() == 3);
	});
	test::Test* remove_at_test = addTest("remove_at", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		vec.removeAt(1);
		tAssert(vec.size() == 2);
		tCheck(*vec[0] == 1);
		tCheck(*vec[1] == 3);
		tCheck(*vec.front() == 1);
		tCheck(*vec.back() == 3);
	});
	test::Test* reverse_test = addTest("reverse", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		vec.reverse();
		tAssert(vec.size() == 3);
		tCheck(*vec[0] == 3);
		tCheck(*vec[1] == 2);
		tCheck(*vec[2] == 1);
		tCheck(*vec.front() == 3);
		tCheck(*vec.back() == 1);
	});
	test::Test* iterators_test = addTest("iterators", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		{
			auto it = vec.begin();
			tCheck(**it == 1);
			tCheck(**++it == 2);
			tCheck(**++it == 3);
			tCheck(++it == vec.end());
		}
		{
			auto it = vec.end();
			tCheck(**--it == 3);
			tCheck(**--it == 2);
			tCheck(**--it == 1);
			tCheck(it == vec.begin());
		}
		{
			auto it = vec.rbegin();
			tCheck(**it == 3);
			tCheck(**++it == 2);
			tCheck(**++it == 1);
			tCheck(++it == vec.rend());
		}
		{
			auto it = vec.rend();
			tCheck(**--it == 1);
			tCheck(**--it == 2);
			tCheck(**--it == 3);
			tCheck(it == vec.rbegin());
		}
		{
			for (int* value : vec) {
				tCheck(value);
			}
			for (int* value : vec | std::views::reverse) {
				tCheck(value);
			}
		}
	});
	test::Test* at_test = addTest("at", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		tCheck(*vec.at(1) == 2);
	});
	test::Test* get_index_test = addTest("get_index", { basic_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		tCheck(vec.getIndex(ptr2) == 1);
	});
	test::Test* get_vector_test = addTest("get_vector", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		std::vector<int*> get_vec = vec.getVector();
		std::vector<int> value_vec;
		for (size_t i = 0; i < get_vec.size(); i++) {
			value_vec.push_back(*get_vec[i]);
		}
		tCheck(value_vec == std::vector<int>({ 1, 2, 3 }));
	});
	test::Test* get_set_test = addTest("get_set", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		std::set<int*> get_set = vec.getSet();
		std::vector<int> value_vec;
		for (int* ptr : get_set) {
			value_vec.push_back(*ptr);
		}
		std::sort(value_vec.begin(), value_vec.end());
		tCheck(value_vec == std::vector<int>({ 1, 2, 3 }));
	});
	test::Test* square_brackets_test = addTest("square_brackets", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		tCheck(*vec[1] == 2);
	});
	test::Test* find_test = addTest("find", { get_set_test }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		tCheck(vec.find(ptr2) != vec.getSet().end());
		tCheck(vec.find(ptr5) == vec.getSet().end());
	});
	test::Test* contains_test = addTest("contains", { basic_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		tCheck(vec.contains(ptr2));
		tCheck(!vec.contains(ptr5));
	});
	test::Test* clear_test = addTest("clear", { basic_tests }, [&](test::Test& test) {
		CompVectorUptr<int> vec = { 1, 2, 3 };
		vec.clear();
		tAssert(vec.size() == 0);
	});
	test::Test* no_duplicates_test = addTest("duplicates", { basic_tests }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr2, ptr3 });
		tAssert(vec.size() == 3);
		tCheck(*vec[0] == 1);
		tCheck(*vec[1] == 2);
		tCheck(*vec[2] == 3);
	});
	test::Test* remove_missing_test = addTest("remove_missing", { remove_test }, [&](test::Test& test) {
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		vec.remove(ptr5);
		tAssert(vec.size() == 3);
		tCheck(*vec[0] == 1);
		tCheck(*vec[1] == 2);
		tCheck(*vec[2] == 3);
	});
}

#endif // NDEBUG
