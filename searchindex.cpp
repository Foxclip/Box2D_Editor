#ifndef NDEBUG

#include "searchindex.h"
#include <vector>
#include <cassert>

SearchIndexTest search_index_test;

class MyClass { };

void test_SearchIndexUnique() {
	MyClass mc1;
	MyClass mc2;
	MyClass mc3;
	SearchIndexUnique<int, MyClass> sindex;
	sindex.add(5, &mc1);
	sindex.add(6, &mc2);
	sindex.add(6, &mc2);
	sindex.add(7, &mc3);
	assert(sindex.size() == 3);
	assert(sindex.contains(5));
	assert(sindex.contains(6));
	assert(sindex.contains(7));
	assert(!sindex.contains(4));
	assert(!sindex.contains(8));
	assert(sindex.min() == 5);
	assert(sindex.max() == 7);
	assert(sindex.find(5) == &mc1);
	assert(sindex.find(6) == &mc2);
	assert(sindex.find(7) == &mc3);
	assert(sindex.find(4) == nullptr);
	assert(sindex.find(8) == nullptr);
	sindex.remove(6);
	assert(sindex.size() == 2);
	assert(sindex.contains(5));
	assert(!sindex.contains(6));
	assert(sindex.contains(7));
	assert(!sindex.contains(4));
	assert(!sindex.contains(8));
	assert(sindex.min() == 5);
	assert(sindex.max() == 7);
	assert(sindex.find(5) == &mc1);
	assert(sindex.find(6) == nullptr);
	assert(sindex.find(7) == &mc3);
	sindex.remove(5);
	assert(sindex.size() == 1);
	assert(!sindex.contains(5));
	assert(!sindex.contains(6));
	assert(sindex.contains(7));
	assert(!sindex.contains(4));
	assert(!sindex.contains(8));
	assert(sindex.min() == 7);
	assert(sindex.max() == 7);
	assert(sindex.find(5) == nullptr);
	assert(sindex.find(6) == nullptr);
	assert(sindex.find(7) == &mc3);
	assert(sindex.find(4) == nullptr);
	assert(sindex.find(8) == nullptr);
	sindex.clear();
	assert(sindex.size() == 0);
	assert(!sindex.contains(5));
	assert(!sindex.contains(6));
	assert(!sindex.contains(7));
	assert(!sindex.contains(4));
	assert(!sindex.contains(8));
	assert(sindex.find(5) == nullptr);
	assert(sindex.find(6) == nullptr);
	assert(sindex.find(7) == nullptr);
	assert(sindex.find(4) == nullptr);
	assert(sindex.find(8) == nullptr);
}

void test_SearchIndexMultiple() {
	MyClass mc1;
	MyClass mc2;
	MyClass mc3;
	MyClass mc4;
	SearchIndexMultiple<int, MyClass> sindex;
	sindex.add(5, &mc1);
	sindex.add(6, &mc2);
	sindex.add(6, &mc4);
	sindex.add(7, &mc3);
	assert(sindex.size() == 4); 
	assert(sindex.contains(5));
	assert(sindex.contains(6));
	assert(sindex.contains(7));
	assert(!sindex.contains(4));
	assert(!sindex.contains(8));
	assert(sindex.min() == 5);
	assert(sindex.max() == 7);
	assert(sindex.find(5) == &mc1);
	assert(sindex.find(6) == &mc2);
	assert(sindex.find(7) == &mc3);
	assert(sindex.find(4) == nullptr);
	assert(sindex.find(8) == nullptr);
	sindex.remove(6, &mc2);
	assert(sindex.size() == 3);
	assert(sindex.contains(5));
	assert(sindex.contains(6));
	assert(sindex.contains(7));
	assert(!sindex.contains(4));
	assert(!sindex.contains(8));
	assert(sindex.min() == 5);
	assert(sindex.max() == 7);
	assert(sindex.find(5) == &mc1);
	assert(sindex.find(6) == &mc4);
	assert(sindex.find(7) == &mc3);
	assert(sindex.find(4) == nullptr);
	assert(sindex.find(8) == nullptr);
	sindex.remove(5, &mc1);
	assert(sindex.size() == 2);
	assert(!sindex.contains(5));
	assert(sindex.contains(6));
	assert(sindex.contains(7));
	assert(!sindex.contains(4));
	assert(!sindex.contains(8));
	assert(sindex.min() == 6);
	assert(sindex.max() == 7);
	assert(sindex.find(5) == nullptr);
	assert(sindex.find(6) == &mc4);
	assert(sindex.find(7) == &mc3);
	assert(sindex.find(4) == nullptr);
	assert(sindex.find(8) == nullptr);
	sindex.clear();
	assert(sindex.size() == 0);
	assert(!sindex.contains(5));
	assert(!sindex.contains(6));
	assert(!sindex.contains(7));
	assert(!sindex.contains(4));
	assert(!sindex.contains(8));
	assert(sindex.find(5) == nullptr);
	assert(sindex.find(6) == nullptr);
	assert(sindex.find(7) == nullptr);
	assert(sindex.find(4) == nullptr);
	assert(sindex.find(8) == nullptr);
}

SearchIndexTest::SearchIndexTest() {
	test_SearchIndexUnique();
	test_SearchIndexMultiple();
}

#endif // NDEBUG
