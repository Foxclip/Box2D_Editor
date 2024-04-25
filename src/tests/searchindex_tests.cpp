#include "tests/searchindex_tests.h"

SearchIndexTests::SearchIndexTests(test::TestManager& manager) : TestModule("SearchIndex", manager) { }

class MyClass { };

void SearchIndexTests::createTestLists() {
	test::TestList* test_list_unique = createTestList("SearchIndexUnique");
	test::TestList* test_list_multiple = createTestList("SearchIndexMultiple");
	createTestListUnique(test_list_unique);
	createTestListMultiple(test_list_multiple);
}

void SearchIndexTests::createTestListUnique(test::TestList* list) {
	test::Test* basic_test = list->addTest("basic", [&](test::Test& test) {
		SearchIndexUnique<int, MyClass*> sindex;
	});
	test::Test* add_test = list->addTest("add", { basic_test }, [&](test::Test& test) {
		SearchIndexUnique<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc2);
		sindex.add(7, &mc3);
		T_ASSERT(T_COMPARE(sindex.size(), 3));
	});
	test::Test* contains_test = list->addTest("contains", { add_test }, [&](test::Test& test) {
		SearchIndexUnique<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc2);
		sindex.add(7, &mc3);
		T_ASSERT(T_COMPARE(sindex.size(), 3));
		T_CHECK(sindex.contains(5));
		T_CHECK(sindex.contains(6));
		T_CHECK(sindex.contains(7));
		T_CHECK(!sindex.contains(4));
		T_CHECK(!sindex.contains(8));
	});
	test::Test* min_max_test = list->addTest("min_max", { add_test }, [&](test::Test& test) {
		SearchIndexUnique<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc2);
		sindex.add(7, &mc3);
		T_ASSERT(T_COMPARE(sindex.size(), 3));
		T_COMPARE(sindex.min(), 5);
		T_COMPARE(sindex.max(), 7);
	});
	test::Test* find_test = list->addTest("find", { add_test }, [&](test::Test& test) {
		SearchIndexUnique<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc2);
		sindex.add(7, &mc3);
		T_ASSERT(T_COMPARE(sindex.size(), 3));
		T_CHECK(sindex.find(5) == &mc1);
		T_CHECK(sindex.find(6) == &mc2);
		T_CHECK(sindex.find(7) == &mc3);
		T_CHECK(sindex.find(4) == nullptr);
		T_CHECK(sindex.find(8) == nullptr);
	});
	test::Test* remove_test = list->addTest("remove", { find_test }, [&](test::Test& test) {
		SearchIndexUnique<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc2);
		sindex.add(7, &mc3);
		sindex.remove(6);
		T_ASSERT(T_COMPARE(sindex.size(), 2));
		T_CHECK(sindex.find(5) == &mc1);
		T_CHECK(sindex.find(6) == nullptr);
		T_CHECK(sindex.find(7) == &mc3);
		sindex.remove(5);
		T_ASSERT(T_COMPARE(sindex.size(), 1));
		T_CHECK(sindex.find(5) == nullptr);
		T_CHECK(sindex.find(6) == nullptr);
		T_CHECK(sindex.find(7) == &mc3);
	});
	test::Test* clear_test = list->addTest("clear", { find_test }, [&](test::Test& test) {
		SearchIndexUnique<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc2);
		sindex.add(7, &mc3);
		sindex.clear();
		T_ASSERT(T_COMPARE(sindex.size(), 0));
		T_CHECK(sindex.find(5) == nullptr);
		T_CHECK(sindex.find(6) == nullptr);
		T_CHECK(sindex.find(7) == nullptr);
		T_CHECK(sindex.find(4) == nullptr);
		T_CHECK(sindex.find(8) == nullptr);
	});
}

void SearchIndexTests::createTestListMultiple(test::TestList* list) {
	test::Test* basic_test = list->addTest("basic", [&](test::Test& test) {
		SearchIndexMultiple<int, MyClass*> sindex;
	});
	test::Test* add_test = list->addTest("add", { basic_test }, [&](test::Test& test) {
		SearchIndexMultiple<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		MyClass mc4;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc4);
		sindex.add(7, &mc3);
		T_ASSERT(T_COMPARE(sindex.size(), 4));
	});
	test::Test* contains_test = list->addTest("contains", { add_test }, [&](test::Test& test) {
		SearchIndexMultiple<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		MyClass mc4;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc4);
		sindex.add(7, &mc3);
		T_ASSERT(T_COMPARE(sindex.size(), 4));
		T_CHECK(sindex.contains(5));
		T_CHECK(sindex.contains(6));
		T_CHECK(sindex.contains(7));
		T_CHECK(!sindex.contains(4));
		T_CHECK(!sindex.contains(8));
	});
	test::Test* min_max_test = list->addTest("min_max", { add_test }, [&](test::Test& test) {
		SearchIndexMultiple<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		MyClass mc4;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc4);
		sindex.add(7, &mc3);
		T_ASSERT(T_COMPARE(sindex.size(), 4));
		T_COMPARE(sindex.min(), 5);
		T_COMPARE(sindex.max(), 7);
	});
	test::Test* find_test = list->addTest("find", { add_test }, [&](test::Test& test) {
		SearchIndexMultiple<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		MyClass mc4;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc4);
		sindex.add(7, &mc3);
		T_ASSERT(T_COMPARE(sindex.size(), 4));
		T_CHECK(sindex.find(5) == &mc1);
		T_CHECK(sindex.find(6) == &mc2);
		T_CHECK(sindex.find(7) == &mc3);
		T_CHECK(sindex.find(4) == nullptr);
		T_CHECK(sindex.find(8) == nullptr);
	});
	test::Test* remove_test = list->addTest("remove", { find_test }, [&](test::Test& test) {
		SearchIndexMultiple<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		MyClass mc4;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc4);
		sindex.add(7, &mc3);
		T_ASSERT(T_COMPARE(sindex.size(), 4));
		sindex.remove(6, &mc2);
		T_ASSERT(T_COMPARE(sindex.size(), 3));
		T_CHECK(sindex.find(5) == &mc1);
		T_CHECK(sindex.find(6) == &mc4);
		T_CHECK(sindex.find(7) == &mc3);
		sindex.remove(5, &mc1);
		T_ASSERT(T_COMPARE(sindex.size(), 2));
		T_CHECK(sindex.find(5) == nullptr);
		T_CHECK(sindex.find(6) == &mc4);
		T_CHECK(sindex.find(7) == &mc3);
	});
	test::Test* clear_test = list->addTest("clear", { find_test }, [&](test::Test& test) {
		SearchIndexMultiple<int, MyClass*> sindex;
		MyClass mc1;
		MyClass mc2;
		MyClass mc3;
		MyClass mc4;
		sindex.add(5, &mc1);
		sindex.add(6, &mc2);
		sindex.add(6, &mc4);
		sindex.add(7, &mc3);
		T_ASSERT(T_COMPARE(sindex.size(), 4));
		sindex.clear();
		T_ASSERT(T_COMPARE(sindex.size(), 0));
		T_CHECK(sindex.find(5) == nullptr);
		T_CHECK(sindex.find(6) == nullptr);
		T_CHECK(sindex.find(7) == nullptr);
		T_CHECK(sindex.find(4) == nullptr);
		T_CHECK(sindex.find(8) == nullptr);
	});
}
