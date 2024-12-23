#pragma once

#include "widgets/widget.h"
#include "test_lib/test.h"
#include "tests/widget_tests/widget_test.h"
#include <random>

struct Node;

class WidgetTestsToposort : public WidgetTest {
public:
	WidgetTestsToposort(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });
	void toposortEmptyTest(test::Test& test);
	void toposort1NodeTest(test::Test& test);
	void toposort3NodesTest(test::Test& test);
	void toposort4NodesDiamondTest(test::Test& test);
	void toposort5NodesXTest(test::Test& test);
	void toposort5NodesRandomTest(test::Test& test);
	void toposortHairTest(test::Test& test);
	void toposortInverseHairTest(test::Test& test);
	void toposortLoopExceptionTest(test::Test& test);
	void toposortLoopEventTest(test::Test& test);
	void toposortTwoLoopsTest(test::Test& test);
	void toposortForkingLoopTest(test::Test& test);
	void toposortTrianglesLoopTest(test::Test& test);

private:
	std::mt19937 rng;
	bool layer_contains(const std::vector<Node*>& layer, const std::string& name);
	template <typename TVec>
	TVec get_shuffled(const TVec& vec) {
		TVec copy(vec);
		std::shuffle(copy.begin(), copy.end(), rng);
		return copy;
	}

};

struct Node {
	std::string str;
	std::vector<Node*> parents;
	Node(const std::string& str);
	void addParent(Node* parent);
	static const std::vector<Node*>& getParents(Node* node);
};

class NodeList {
public:
	Node* createNode(const std::string& name);
	const std::vector<Node*>& getNodes() const;
private:
	CompVectorUptr<Node> nodes;
};
