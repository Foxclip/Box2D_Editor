#ifndef NDEBUG

#include "compvector.h"
#include <cassert>
#include <algorithm>
#include <ranges>

CompoundVectorTest compound_vector_test;

void test_CompoundVector() {
	{
		// empty vector
		CompoundVector<int> vec;
		assert(vec.size() == 0);
	}
	{
		// one value
		CompoundVector<int> vec = { 5 };
		assert(vec.size() == 1);
		assert(vec[0] == 5);
		assert(vec.front() == 5);
		assert(vec.back() == 5);
	}
	{
		// multiple values
		CompoundVector<int> vec = { 1, 2, 3 };
		assert(vec.size() == 3);
		assert(vec[0] == 1);
		assert(vec[1] == 2);
		assert(vec[2] == 3);
		assert(vec.front() == 1);
		assert(vec.back() == 3);
	}
	{
		// convert to vector
		CompoundVector<int> cvec = { 1, 2, 3 };
		std::vector<int> vec = cvec;
		assert(vec == std::vector<int>({ 1, 2, 3 }));
	}
	{
		// compare to vector
		CompoundVector<int> cvec = { 1, 2, 3 };
		std::vector<int> vec = { 1, 2, 3 };
		assert(cvec == vec);
		assert(vec == cvec);
	}
	{
		// from vector
		std::vector<int> vec = { 1, 2, 3 };
		CompoundVector<int> cvec(vec);
		assert(vec == std::vector<int>({ 1, 2, 3 }));
	}
	{
		// add method
		CompoundVector<int> vec;
		vec.add(1);
		vec.add(2);
		vec.add(3);
		assert(vec == std::vector<int>({ 1, 2, 3 }));
	}
	{
		// insert method (value)
		CompoundVector<int> vec = { 1, 2, 3 };
		vec.insert(vec.begin() + 2, 5);
		assert(vec == std::vector<int>({ 1, 2, 5, 3 }));
		vec.insert(vec.begin() + 2, 5);
		assert(vec == std::vector<int>({ 1, 2, 5, 3 }));
	}
	{
		// insert method (range)
		CompoundVector<int> vec = { 1, 2, 3 };
		std::set<int> set1 = { 5, 6, 7 };
		vec.insert(vec.begin() + 2, set1.begin(), set1.end());
		assert(vec == std::vector<int>({ 1, 2, 5, 6, 7, 3 }));
		vec.insert(vec.begin() + 2, set1.begin(), set1.end());
		assert(vec == std::vector<int>({ 1, 2, 5, 6, 7, 3 }));
		std::set<int> set2 = { 7, 8, 9 };
		vec.insert(vec.begin(), set2.begin(), set2.end());
		assert(vec == std::vector<int>({ 8, 9, 1, 2, 5, 6, 7, 3 }));
	}
	{
		// remove method
		CompoundVector<int> vec = { 1, 2, 3 };
		vec.remove(2);
		assert(vec == std::vector<int>({ 1, 3 }));
	}
	{
		// removeAt method
		CompoundVector<int> vec = { 1, 2, 3 };
		vec.removeAt(1);
		assert(vec == std::vector<int>({ 1, 3 }));
	}
	{
		// iterators
		CompoundVector<int> vec = { 1, 2, 3 };
		{
			auto it = vec.begin();
			assert(*it == 1);
			assert(*++it == 2);
			assert(*++it == 3);
			assert(++it == vec.end());
		}
		{
			auto it = vec.end();
			assert(*--it == 3);
			assert(*--it == 2);
			assert(*--it == 1);
			assert(it == vec.begin());
		}
		{
			auto it = vec.rbegin();
			assert(*it == 3);
			assert(*++it == 2);
			assert(*++it == 1);
			assert(++it == vec.rend());
		}
		{
			auto it = vec.rend();
			assert(*--it == 1);
			assert(*--it == 2);
			assert(*--it == 3);
			assert(it == vec.rbegin());
		}
		{
			for (int value : vec) {
				assert(value > 0);
			}
			for (int value : vec | std::views::reverse) {
				assert(value > 0);
			}
		}
	}
	{
		// at method
		CompoundVector<int> vec = { 1, 2, 3 };
		assert(vec.at(1) == 2);
	}
	{
		// getIndex method
		CompoundVector<int> vec = { 1, 2, 3 };
		assert(vec.getIndex(2) == 1);
	}
	{
		// getVector method
		CompoundVector<int> vec = { 1, 2, 3 };
		assert(vec.getVector() == std::vector<int>({ 1, 2, 3 }));
	}
	{
		// getSet method
		CompoundVector<int> vec = { 1, 2, 3 };
		assert(vec.getSet() == std::set<int>({ 1, 2, 3 }));
	}
	{
		// operator []
		CompoundVector<int> vec = { 1, 2, 3 };
		assert(vec[1] == 2);
	}
	{
		// find method
		CompoundVector<int> vec = { 1, 2, 3 };
		assert(*vec.find(2) == 2);
		assert(vec.find(5) == vec.getSet().end());
	}
	{
		// contains method
		CompoundVector<int> vec = { 1, 2, 3 };
		assert(vec.contains(2));
		assert(!vec.contains(5));
	}
	{
		// clear method
		CompoundVector<int> vec = { 1, 2, 3 };
		vec.clear();
		assert(vec.size() == 0);
	}
	{
		// operator ==
		CompoundVector<int> vec = { 1, 2, 3 };
		assert(vec == CompoundVector<int>({ 1, 2, 3 }));
		assert(vec != CompoundVector<int>({ 1, 2 }));
		assert(vec != CompoundVector<int>({ 1, 2, 3, 4 }));
		assert(vec != CompoundVector<int>({ 1, 5, 3 }));
	}
	{
		// no duplicates
		CompoundVector<int> vec = { 1, 2, 2, 3 };
		assert(vec == CompoundVector<int>({ 1, 2, 3 }));
	}
	{
		// removing missing
		CompoundVector<int> vec = { 1, 2, 3 };
		vec.remove(5);
		assert(vec == CompoundVector<int>({ 1, 2, 3 }));
	}
	{
		// value modification
		CompoundVector<int> vec = { 1, 2, 3 };
		vec[1] = 5;
		assert(vec == CompoundVector<int>({ 1, 5, 3 }));
	}
}

void test_CompoundVectorUptr() {
	{
		// empty vector
		CompoundVectorUptr<int> vec;
		assert(vec.size() == 0);
	}
	{
		// one value
		CompoundVectorUptr<int> vec = { 5 };
		assert(vec.size() == 1);
		assert(*vec[0] == 5);
		assert(*vec.front() == 5);
		assert(*vec.back() == 5);
	}
	{
		// multiple values
		CompoundVectorUptr<int> vec = { 1, 2, 3 };
		assert(vec.size() == 3);
		assert(*vec[0] == 1);
		assert(*vec[1] == 2);
		assert(*vec[2] == 3);
		assert(*vec.front() == 1);
		assert(*vec.back() == 3);
	}
	{
		// convert to vector
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompoundVectorUptr<int> cvec = { ptr1, ptr2, ptr3 };
		std::vector<int*> vec = cvec;
		assert(vec == std::vector<int*>({ ptr1, ptr2, ptr3 }));
	}
	{
		// compare to vector
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompoundVectorUptr<int> cvec = { ptr1, ptr2, ptr3 };
		std::vector<int*> vec = { ptr1, ptr2, ptr3 };
		assert(cvec == vec);
		assert(vec == cvec);
	}
	{
		// add method
		CompoundVectorUptr<int> vec;
		vec.add(1);
		vec.add(2);
		vec.add(3);
		assert(vec.size() == 3);
		assert(*vec[0] == 1);
		assert(*vec[1] == 2);
		assert(*vec[2] == 3);
		assert(*vec.front() == 1);
		assert(*vec.back() == 3);
	}
	{
		// insert method (value)
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompoundVectorUptr<int> vec = { ptr1, ptr2, ptr3 };
		std::unique_ptr<int> uptr5(ptr5);
		std::unique_ptr<int> uptr5_another(ptr5);
		vec.insert(vec.begin() + 2, std::move(uptr5));
		assert(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr3 }));
		vec.insert(vec.begin() + 2, std::move(uptr5_another));
		assert(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr3 }));
		uptr5_another.release();
	}
	{
		// insert method (range)
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr4 = new int(4);
		int* ptr5 = new int(5);
		int* ptr6 = new int(6);
		int* ptr7 = new int(7);
		int* ptr8 = new int(8);
		int* ptr9 = new int(9);
		CompoundVectorUptr<int> vec = { ptr1, ptr2, ptr3 };
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
		assert(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr6, ptr7, ptr3 }));
		vec.insert(vec.begin() + 2, vec1_another.begin(), vec1_another.end());
		assert(vec == std::vector<int*>({ ptr1, ptr2, ptr5, ptr6, ptr7, ptr3 }));
		std::unique_ptr<int> uptr7_2(ptr7);
		std::unique_ptr<int> uptr8(ptr8);
		std::unique_ptr<int> uptr9(ptr9);
		std::vector<std::unique_ptr<int>> vec2;
		vec2.push_back(std::move(uptr7_2));
		vec2.push_back(std::move(uptr8));
		vec2.push_back(std::move(uptr9));
		vec.insert(vec.begin(), vec2.begin(), vec2.end());
		assert(vec == std::vector<int*>({ ptr8, ptr9, ptr1, ptr2, ptr5, ptr6, ptr7, ptr3 }));
		uptr5_another.release();
		uptr6_another.release();
		uptr7_another.release();
		uptr7_2.release();
	}
	{
		// remove method
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompoundVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		vec.remove(ptr2);
		assert(vec.size() == 2);
		assert(*vec[0] == 1);
		assert(*vec[1] == 3);
		assert(*vec.front() == 1);
		assert(*vec.back() == 3);
	}
	{
		// removeAt method
		CompoundVectorUptr<int> vec = { 1, 2, 3 };
		vec.removeByIndex(1);
		assert(vec.size() == 2);
		assert(*vec[0] == 1);
		assert(*vec[1] == 3);
		assert(*vec.front() == 1);
		assert(*vec.back() == 3);
	}
	{
		// iterators
		CompoundVectorUptr<int> vec = { 1, 2, 3 };
		{
			auto it = vec.begin();
			assert(**it == 1);
			assert(**++it == 2);
			assert(**++it == 3);
			assert(++it == vec.end());
		}
		{
			auto it = vec.end();
			assert(**--it == 3);
			assert(**--it == 2);
			assert(**--it == 1);
			assert(it == vec.begin());
		}
		{
			auto it = vec.rbegin();
			assert(**it == 3);
			assert(**++it == 2);
			assert(**++it == 1);
			assert(++it == vec.rend());
		}
		{
			auto it = vec.rend();
			assert(**--it == 1);
			assert(**--it == 2);
			assert(**--it == 3);
			assert(it == vec.rbegin());
		}
		{
			for (int* value : vec) {
				assert(value);
			}
			for (int* value : vec | std::views::reverse) {
				assert(value);
			}
		}
	}
	{
		// at method
		CompoundVectorUptr<int> vec = { 1, 2, 3 };
		assert(*vec.at(1) == 2);
	}
	{
		// getIndex method
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompoundVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		assert(vec.getIndex(ptr2) == 1);
	}
	{
		// getVector method
		CompoundVectorUptr<int> vec = { 1, 2, 3 };
		std::vector<int*> get_vec = vec.getVector();
		std::vector<int> value_vec;
		for (size_t i = 0; i < get_vec.size(); i++) {
			value_vec.push_back(*get_vec[i]);
		}
		assert(value_vec == std::vector<int>({ 1, 2, 3 }));
	}
	{
		// getSet method
		CompoundVectorUptr<int> vec = { 1, 2, 3 };
		std::set<int*> get_set = vec.getSet();
		std::vector<int> value_vec;
		for (int* ptr : get_set) {
			value_vec.push_back(*ptr);
		}
		std::sort(value_vec.begin(), value_vec.end());
		assert(value_vec == std::vector<int>({ 1, 2, 3 }));
	}
	{
		// operator []
		CompoundVectorUptr<int> vec = { 1, 2, 3 };
		assert(*vec[1] == 2);
	}
	{
		// find method
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompoundVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		assert(vec.find(ptr2) != vec.getSet().end());
		assert(vec.find(ptr5) == vec.getSet().end());
	}
	{
		// contains method
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompoundVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		assert(vec.contains(ptr2));
		assert(!vec.contains(ptr5));
	}
	{
		// clear method
		CompoundVectorUptr<int> vec = { 1, 2, 3 };
		vec.clear();
		assert(vec.size() == 0);
	}
	{
		// no duplicates
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompoundVectorUptr<int> vec({ ptr1, ptr2, ptr2, ptr3 });
		assert(vec.size() == 3);
		assert(*vec[0] == 1);
		assert(*vec[1] == 2);
		assert(*vec[2] == 3);
	}
	{
		// removing missing
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		int* ptr5 = new int(5);
		CompoundVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		vec.remove(ptr5);
		assert(vec.size() == 3);
		assert(*vec[0] == 1);
		assert(*vec[1] == 2);
		assert(*vec[2] == 3);
	}
}

CompoundVectorTest::CompoundVectorTest() {
	test_CompoundVector();
	test_CompoundVectorUptr();
}

#endif // NDEBUG
