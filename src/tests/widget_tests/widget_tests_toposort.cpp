#include "tests/widget_tests/widget_tests.h"
#include "tests/widget_tests/widget_tests_toposort.h"

WidgetTestsToposort::WidgetTestsToposort(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes) : WidgetTest(name, parent, required_nodes) {
    test::Test* empty_test = addTest("empty", [&](test::Test& test) { toposortEmptyTest(test); });
    test::Test* one_node_test = addTest("one_node", { empty_test }, [&](test::Test& test) { toposort1NodeTest(test); });
    test::Test* three_nodes_test = addTest("three_nodes", { one_node_test }, [&](test::Test& test) { toposort3NodesTest(test); });
    test::Test* four_nodes_diamond_test = addTest("four_nodes_diamond", { three_nodes_test }, [&](test::Test& test) { toposort4NodesDiamondTest(test); });
    test::Test* five_nodes_x_test = addTest("five_nodes_x", { three_nodes_test }, [&](test::Test& test) { toposort5NodesXTest(test); });
    test::Test* five_nodes_random_test = addTest("five_nodes_random", { three_nodes_test }, [&](test::Test& test) { toposort5NodesRandomTest(test); });
    test::Test* hair_test = addTest("hair", { three_nodes_test }, [&](test::Test& test) { toposortHairTest(test); });
    test::Test* inverse_hair_test = addTest("inverse_hair", { three_nodes_test }, [&](test::Test& test) { toposortInverseHairTest(test); });
    test::Test* loop_exception_test = addTest("loop_exception", { three_nodes_test }, [&](test::Test& test) { toposortLoopExceptionTest(test); });
    test::Test* loop_event_test = addTest("loop_event", { three_nodes_test }, [&](test::Test& test) { toposortLoopEventTest(test); });
    test::Test* two_loops_test = addTest("two_loops", { loop_event_test }, [&](test::Test& test) { toposortTwoLoopsTest(test); });
    test::Test* forking_loop_test = addTest("forking_loop", { loop_event_test }, [&](test::Test& test) { toposortForkingLoopTest(test); });
    test::Test* triangles_loop_test = addTest("triangles_loop", { forking_loop_test }, [&](test::Test& test) { toposortTrianglesLoopTest(test); });
}

void WidgetTestsToposort::toposortEmptyTest(test::Test& test) {
    NodeList list;
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        list.getNodes(), &Node::getParents
    );
    T_CHECK(sorted.size() == 0);
}

void WidgetTestsToposort::toposort1NodeTest(test::Test& test) {
    NodeList list;
    Node* node = list.createNode("A");
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        list.getNodes(), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 1)) {
        if (T_CHECK(sorted[0].size() == 1)) {
            T_CHECK(layer_contains(sorted[0], "A"));
        }
    }
}

void WidgetTestsToposort::toposort3NodesTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    nodeC->addParent(nodeA);
    nodeB->addParent(nodeC);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 3)) {
        if (T_CHECK(sorted[0].size() == 1)) {
            T_CHECK(layer_contains(sorted[0], "A"));
        }
        if (T_CHECK(sorted[1].size() == 1)) {
            T_CHECK(layer_contains(sorted[1], "C"));
        }
        if (T_CHECK(sorted[2].size() == 1)) {
            T_CHECK(layer_contains(sorted[2], "B"));
        }
    }
}

void WidgetTestsToposort::toposort4NodesDiamondTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    Node* nodeD = list.createNode("D");
    nodeB->addParent(nodeA);
    nodeC->addParent(nodeA);
    nodeD->addParent(nodeB);
    nodeD->addParent(nodeC);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 3)) {
        if (T_CHECK(sorted[0].size() == 1)) {
            T_CHECK(layer_contains(sorted[0], "A"));
        }
        if (T_CHECK(sorted[1].size() == 2)) {
            T_CHECK(layer_contains(sorted[1], "B"));
            T_CHECK(layer_contains(sorted[1], "C"));
        }
        if (T_CHECK(sorted[2].size() == 1)) {
            T_CHECK(layer_contains(sorted[2], "D"));
        }
    }
}

void WidgetTestsToposort::toposort5NodesXTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    Node* nodeD = list.createNode("D");
    Node* nodeE = list.createNode("E");
    nodeC->addParent(nodeA);
    nodeC->addParent(nodeB);
    nodeD->addParent(nodeC);
    nodeE->addParent(nodeC);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 3)) {
        if (T_CHECK(sorted[0].size() == 2)) {
            T_CHECK(layer_contains(sorted[0], "A"));
            T_CHECK(layer_contains(sorted[0], "B"));
        }
        if (T_CHECK(sorted[1].size() == 1)) {
            T_CHECK(layer_contains(sorted[1], "C"));
        }
        if (sorted[2].size() == 2) {
            T_CHECK(layer_contains(sorted[2], "D"));
            T_CHECK(layer_contains(sorted[2], "E"));
        }
    }
}

void WidgetTestsToposort::toposort5NodesRandomTest(test::Test& test) {
    NodeList list;
    Node* node0 = list.createNode("N0");
    Node* node1 = list.createNode("N1");
    Node* node2 = list.createNode("N2");
    Node* node3 = list.createNode("N3");
    Node* node4 = list.createNode("N4");
    Node* node5 = list.createNode("N5");
    node0->addParent(node2);
    node2->addParent(node3);
    node4->addParent(node0);
    node4->addParent(node5);
    node5->addParent(node2);
    node5->addParent(node1);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 4)) {
        if (T_CHECK(sorted[0].size() == 2)) {
            T_CHECK(layer_contains(sorted[0], "N1"));
            T_CHECK(layer_contains(sorted[0], "N3"));
        }
        if (T_CHECK(sorted[1].size() == 1)) {
            T_CHECK(layer_contains(sorted[1], "N2"));
        }
        if (T_CHECK(sorted[2].size() == 2)) {
            T_CHECK(layer_contains(sorted[2], "N0"));
            T_CHECK(layer_contains(sorted[2], "N5"));
        }
        if (T_CHECK(sorted[3].size() == 1)) {
            T_CHECK(layer_contains(sorted[3], "N4"));
        }
    }
}

void WidgetTestsToposort::toposortHairTest(test::Test& test) {
    NodeList list;
    Node* center = list.createNode("center");
    Node* short0 = list.createNode("short0");
    Node* short1 = list.createNode("short1");
    Node* short2 = list.createNode("short2");
    Node* long0 = list.createNode("long0");
    Node* long1 = list.createNode("long1");
    Node* long2 = list.createNode("long2");
    short0->addParent(center);
    short1->addParent(center);
    short2->addParent(center);
    long0->addParent(center);
    long1->addParent(long0);
    long2->addParent(long1);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 4)) {
        if (T_CHECK(sorted[0].size() == 1)) {
            T_CHECK(layer_contains(sorted[0], "center"));
        }
        if (T_CHECK(sorted[1].size() == 4)) {
            T_CHECK(layer_contains(sorted[1], "short0"));
            T_CHECK(layer_contains(sorted[1], "short1"));
            T_CHECK(layer_contains(sorted[1], "short2"));
            T_CHECK(layer_contains(sorted[1], "long0"));
        }
        if (T_CHECK(sorted[2].size() == 1)) {
            T_CHECK(layer_contains(sorted[2], "long1"));
        }
        if (T_CHECK(sorted[3].size() == 1)) {
            T_CHECK(layer_contains(sorted[3], "long2"));
        }
    }
}

void WidgetTestsToposort::toposortInverseHairTest(test::Test& test) {
    NodeList list;
    Node* center = list.createNode("center");
    Node* short0 = list.createNode("short0");
    Node* short1 = list.createNode("short1");
    Node* short2 = list.createNode("short2");
    Node* long0 = list.createNode("long0");
    Node* long1 = list.createNode("long1");
    Node* long2 = list.createNode("long2");
    center->addParent(short0);
    center->addParent(short1);
    center->addParent(short2);
    center->addParent(long0);
    long0->addParent(long1);
    long1->addParent(long2);
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents
    );
    if (T_CHECK(sorted.size() == 4)) {
        if (T_CHECK(sorted[0].size() == 4)) {
            T_CHECK(layer_contains(sorted[0], "long2"));
            T_CHECK(layer_contains(sorted[0], "short0"));
            T_CHECK(layer_contains(sorted[0], "short1"));
            T_CHECK(layer_contains(sorted[0], "short2"));
        }
        if (T_CHECK(sorted[1].size() == 1)) {
            T_CHECK(layer_contains(sorted[1], "long1"));
        }
        if (T_CHECK(sorted[2].size() == 1)) {
            T_CHECK(layer_contains(sorted[2], "long0"));
        }
        if (T_CHECK(sorted[3].size() == 1)) {
            T_CHECK(layer_contains(sorted[3], "center"));
        }
    }
}

void WidgetTestsToposort::toposortLoopExceptionTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    nodeC->addParent(nodeA);
    nodeB->addParent(nodeC);
    nodeA->addParent(nodeB);
    bool exception = false;
    try {
        std::vector<std::vector<Node*>> sorted = fw::toposort(
            get_shuffled(list.getNodes()), &Node::getParents
        );
    } catch (std::exception exc) {
        exception = true;
    }
    T_CHECK(exception);
}

void WidgetTestsToposort::toposortLoopEventTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    nodeC->addParent(nodeA);
    nodeB->addParent(nodeC);
    nodeA->addParent(nodeB);
    std::vector<Node*> loop;
    std::function<void(const std::vector<Node*>&)> on_loop_detected = [&](const std::vector<Node*>& p_loop) {
        loop = p_loop;
    };
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents, &on_loop_detected
    );
    T_CHECK(sorted.empty());
    if (T_COMPARE(loop.size(), 3)) {
        T_CHECK(loop[0] == nodeB);
        T_CHECK(loop[1] == nodeC);
        T_CHECK(loop[2] == nodeA);
    }
}

void WidgetTestsToposort::toposortTwoLoopsTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    Node* nodeD = list.createNode("D");
    Node* nodeE = list.createNode("E");
    Node* nodeF = list.createNode("F");
    nodeA->addParent(nodeB);
    nodeB->addParent(nodeC);
    nodeC->addParent(nodeA);
    nodeD->addParent(nodeE);
    nodeE->addParent(nodeF);
    nodeF->addParent(nodeD);
    std::vector<std::vector<Node*>> loops;
    std::function<void(const std::vector<Node*>&)> on_loop_detected = [&](const std::vector<Node*>& p_loop) {
        loops.push_back(p_loop);
    };
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents, &on_loop_detected
    );
    T_CHECK(sorted.empty());
    if (T_COMPARE(loops.size(), 2)) {
        if (T_COMPARE(loops[0].size(), 3)) {
            T_CHECK(loops[0][0] == nodeF);
            T_CHECK(loops[0][1] == nodeD);
            T_CHECK(loops[0][2] == nodeE);
        }
        if (T_COMPARE(loops[1].size(), 3)) {
            T_CHECK(loops[1][0] == nodeB);
            T_CHECK(loops[1][1] == nodeC);
            T_CHECK(loops[1][2] == nodeA);
        }
    }
}

void WidgetTestsToposort::toposortForkingLoopTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeL1 = list.createNode("L1");
    Node* nodeL2 = list.createNode("L2");
    Node* nodeR1 = list.createNode("R1");
    Node* nodeR2 = list.createNode("R2");
    nodeA->addParent(nodeB);
    nodeB->addParent(nodeL1);
    nodeB->addParent(nodeR1);
    nodeL1->addParent(nodeL2);
    nodeR1->addParent(nodeR2);
    nodeL2->addParent(nodeB);
    nodeR2->addParent(nodeA);
    std::vector<std::vector<Node*>> loops;
    std::function<void(const std::vector<Node*>&)> on_loop_detected = [&](const std::vector<Node*>& p_loop) {
        loops.push_back(p_loop);
    };
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents, &on_loop_detected
    );
    T_CHECK(sorted.empty());
    if (T_COMPARE(loops.size(), 2)) {
        if (T_COMPARE(loops[0].size(), 3)) {
            T_CHECK(loops[0][0] == nodeB);
            T_CHECK(loops[0][1] == nodeL1);
            T_CHECK(loops[0][2] == nodeL2);
        }
        if (T_COMPARE(loops[1].size(), 4)) {
            T_CHECK(loops[1][0] == nodeR2);
            T_CHECK(loops[1][1] == nodeA);
            T_CHECK(loops[1][2] == nodeB);
            T_CHECK(loops[1][3] == nodeR1);
        }
    }
}

void WidgetTestsToposort::toposortTrianglesLoopTest(test::Test& test) {
    NodeList list;
    Node* nodeA = list.createNode("A");
    Node* nodeB = list.createNode("B");
    Node* nodeC = list.createNode("C");
    Node* nodeD = list.createNode("D");
    Node* nodeE = list.createNode("E");
    Node* nodeF = list.createNode("F");
    nodeA->addParent(nodeC);
    nodeB->addParent(nodeA);
    nodeB->addParent(nodeE);
    nodeC->addParent(nodeB);
    nodeC->addParent(nodeF);
    nodeD->addParent(nodeB);
    nodeE->addParent(nodeD);
    nodeE->addParent(nodeC);
    nodeF->addParent(nodeE);
    std::vector<std::vector<Node*>> loops;
    std::function<void(const std::vector<Node*>&)> on_loop_detected = [&](const std::vector<Node*>& p_loop) {
        loops.push_back(p_loop);
    };
    std::vector<std::vector<Node*>> sorted = fw::toposort(
        get_shuffled(list.getNodes()), &Node::getParents, &on_loop_detected
    );
    T_CHECK(sorted.empty());
    if (T_COMPARE(loops.size(), 5)) {
        if (T_COMPARE(loops[0].size(), 3)) {
            T_CHECK(loops[0][0] == nodeB);
            T_CHECK(loops[0][1] == nodeA);
            T_CHECK(loops[0][2] == nodeC);
        }
        if (T_COMPARE(loops[1].size(), 6)) {
            T_CHECK(loops[1][0] == nodeE);
            T_CHECK(loops[1][1] == nodeD);
            T_CHECK(loops[1][2] == nodeB);
            T_CHECK(loops[1][3] == nodeA);
            T_CHECK(loops[1][4] == nodeC);
            T_CHECK(loops[1][5] == nodeF);
        }
        if (T_COMPARE(loops[2].size(), 3)) {
            T_CHECK(loops[2][0] == nodeE);
            T_CHECK(loops[2][1] == nodeD);
            T_CHECK(loops[2][2] == nodeB);
        }
        if (T_COMPARE(loops[3].size(), 3)) {
            T_CHECK(loops[3][0] == nodeE);
            T_CHECK(loops[3][1] == nodeC);
            T_CHECK(loops[3][2] == nodeB);
        }
        if (T_COMPARE(loops[4].size(), 3)) {
            T_CHECK(loops[4][0] == nodeE);
            T_CHECK(loops[4][1] == nodeC);
            T_CHECK(loops[4][2] == nodeF);
        }
    }
}

bool WidgetTestsToposort::layer_contains(const std::vector<Node*>& layer, const std::string& name) {
    for (Node* node : layer) {
        if (node->str == name) {
            return true;
        }
    }
    return false;
}

Node::Node(const std::string& str) {
    this->str = str;
}

void Node::addParent(Node* parent) {
    parents.push_back(parent);
}

const std::vector<Node*>& Node::getParents(Node* node) {
    return node->parents;
}

Node* NodeList::createNode(const std::string& name) {
    DataPointerUnique<Node> uptr = make_data_pointer<Node>("Node " + name, name);
    Node* ptr = uptr.get();
    nodes.add(std::move(uptr));
    return ptr;
}

const std::vector<Node*>& NodeList::getNodes() const {
    return nodes.getVector();
}
