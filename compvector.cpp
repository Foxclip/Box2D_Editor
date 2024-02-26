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
		// add method
		CompoundVector<int> vec;
		vec.add(1);
		vec.add(2);
		vec.add(3);
		assert(vec.size() == 3);
		assert(vec[0] == 1);
		assert(vec[1] == 2);
		assert(vec[2] == 3);
		assert(vec.front() == 1);
		assert(vec.back() == 3);
	}
	{
		// remove method
		CompoundVector<int> vec = { 1, 2, 3 };
		vec.remove(2);
		assert(vec.size() == 2);
		assert(vec[0] == 1);
		assert(vec[1] == 3);
		assert(vec.front() == 1);
		assert(vec.back() == 3);
	}
	{
		// removeByIndex method
		CompoundVector<int> vec = { 1, 2, 3 };
		vec.removeByIndex(1);
		assert(vec.size() == 2);
		assert(vec[0] == 1);
		assert(vec[1] == 3);
		assert(vec.front() == 1);
		assert(vec.back() == 3);
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
		// get method
		CompoundVector<int> vec = { 1, 2, 3 };
		assert(vec.get(1) == 2);
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
		assert(vec.size() == 3);
		assert(vec[0] == 1);
		assert(vec[1] == 2);
		assert(vec[2] == 3);
		assert(vec.front() == 1);
		assert(vec.back() == 3);
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
		// remove method
		int* ptr1 = new int(1);
		int* ptr2 = new int(2);
		int* ptr3 = new int(3);
		CompoundVectorUptr<int> vec({ ptr1, ptr2, ptr3 });
		vec.remove(ptr2);
		assert(vec.size() == 2);
		assert(*vec[0] == 1);
		assert(*vec[1] == 3);
		assert(*vec.front() == 1);
		assert(*vec.back() == 3);
	}
	{
		// removeByIndex method
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
		// get method
		CompoundVectorUptr<int> vec = { 1, 2, 3 };
		assert(*vec.get(1) == 2);
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
		assert(*vec.front() == 1);
		assert(*vec.back() == 3);
	}
}

CompoundVectorTest::CompoundVectorTest() {
	test_CompoundVector();
	test_CompoundVectorUptr();
}

#endif // NDEBUG
