#include "tests/simulation_tests.h"

SimulationTests::SimulationTests(test::TestManager& manager) : TestModule("Simulation", manager) { }

void SimulationTests::createTestLists() {
    test::TestList* simulation_list = createTestList("Simulation");
    test::Test* basic_test = simulation_list->addTest("basic", [&](test::Test& test) { basicTest(test); });
    test::Test* box_test = simulation_list->addTest("box", { basic_test }, [&](test::Test& test) { boxTest(test); });
    test::Test* ball_test = simulation_list->addTest("ball", { basic_test }, [&](test::Test& test) { ballTest(test); });
    test::Test* polygon_test = simulation_list->addTest("polygon", { basic_test }, [&](test::Test& test) { polygonTest(test); });
    test::Test* chain_test = simulation_list->addTest("chain", { basic_test }, [&](test::Test& test) { chainTest(test); });
    test::Test* revolute_joint_test = simulation_list->addTest("revolute_joint", { box_test }, [&](test::Test& test) { revoluteJointTest(test); });
    test::Test* car_test = simulation_list->addTest("car", { ball_test, polygon_test, revolute_joint_test }, [&](test::Test& test) { carTest(test); });
    test::Test* box_serialize_test = simulation_list->addTest("box_serialize", { box_test }, [&](test::Test& test) { boxSerializeTest(test); });
    test::Test* ball_serialize_test = simulation_list->addTest("ball_serialize", { ball_test }, [&](test::Test& test) { ballSerializeTest(test); });
    test::Test* polygon_serialize_test = simulation_list->addTest("polygon_serialize", { polygon_test }, [&](test::Test& test) { polygonSerializeTest(test); });
    test::Test* chain_serialize_test = simulation_list->addTest("chain_serialize", { chain_test }, [&](test::Test& test) { chainSerializeTest(test); });
    test::Test* revolute_joint_serialize_test = simulation_list->addTest("revolute_joint_serialize", { revolute_joint_test }, [&](test::Test& test) { revoluteJointSerializeTest(test); });
    std::vector<test::Test*> serialize_tests = { car_test, ball_serialize_test, polygon_serialize_test, revolute_joint_serialize_test };
    test::Test* car_serialize_test = simulation_list->addTest("car_serialize", serialize_tests, [&](test::Test& test) { carSerializeTest(test); });
    test::Test* advance_test = simulation_list->addTest("advance", { box_test }, [&](test::Test& test) { advanceTest(test); });
    test::Test* saveload_test = simulation_list->addTest("saveload", { box_test, box_serialize_test }, [&](test::Test& test) { saveloadTest(test); });
    test::Test* box_stack_test = simulation_list->addTest("box_stack", { advance_test, saveload_test }, [&](test::Test& test) { boxStackTest(test); });
    test::Test* moving_car_test = simulation_list->addTest("moving_car", { advance_test, saveload_test, car_serialize_test }, [&](test::Test& test) { movingCarTest(test); });

    test::TestList* gameobject_list = createTestList("GameObject", { simulation_list });
    test::Test* set_parent_two_test = gameobject_list->addTest("set_parent_two", [&](test::Test& test) { setParentTwoTest(test); });
    test::Test* set_parent_three_test = gameobject_list->addTest("set_parent_three", { set_parent_two_test }, [&](test::Test& test) { setParentThreeTest(test); });
    test::Test* parent_loop_test = gameobject_list->addTest("parent_loop", { set_parent_three_test }, [&](test::Test& test) { parentLoopTest(test); });
    test::Test* set_position_two_test = gameobject_list->addTest("set_position_two", { set_parent_two_test }, [&](test::Test& test) { setPositionTwoTest(test); });
    test::Test* set_position_three_test = gameobject_list->addTest("set_position_three", { set_parent_two_test, set_position_two_test }, [&](test::Test& test) { setPositionThreeTest(test); });
    test::Test* set_angle_test = gameobject_list->addTest("set_angle", { set_parent_three_test, }, [&](test::Test& test) { setAngleTest(test); });
    test::Test* set_vertex_pos_test = gameobject_list->addTest("set_vertex_pos", [&](test::Test& test) { setVertexPosTest(test); });
    test::Test* add_vertex_test = gameobject_list->addTest("add_vertex", { set_vertex_pos_test }, [&](test::Test& test) { addVertexTest(test); });
    test::Test* delete_vertex_test = gameobject_list->addTest("delete_vertex", { set_vertex_pos_test }, [&](test::Test& test) { deleteVertexTest(test); });

    test::TestList* objectlist_list = createTestList("ObjectList");
    test::Test* objects_test = objectlist_list->addTest("objects", [&](test::Test& test) { objectsTest(test); });
    test::Test* joints_test = objectlist_list->addTest("joints", [&](test::Test& test) { jointsTest(test); });
    test::Test* add_test = objectlist_list->addTest("add", { objects_test }, [&](test::Test& test) { addTest(test); });
    test::Test* add_joint_test = objectlist_list->addTest("add_joint", { joints_test }, [&](test::Test& test) { addJointTest(test); });
    test::Test* duplicate_test = objectlist_list->addTest("duplicate", { add_test, add_joint_test }, [&](test::Test& test) { duplicateTest(test); });
    test::Test* duplicate_with_children_test = objectlist_list->addTest("duplicate_with_children", { duplicate_test }, [&](test::Test& test) { duplicateWithChildrenTest(test); });
    test::Test* remove_joint_test = objectlist_list->addTest("remove_joint", { add_joint_test }, [&](test::Test& test) { removeJointTest(test); });
    test::Test* remove_test = objectlist_list->addTest("remove", { add_test, remove_joint_test }, [&](test::Test& test) { removeTest(test); });
    test::Test* remove_with_children_test = objectlist_list->addTest("remove_with_children", { remove_test }, [&](test::Test& test) { removeWithChildrenTest(test); });
    test::Test* clear_test = objectlist_list->addTest("clear", { objects_test }, [&](test::Test& test) { clearTest(test); });
}

void SimulationTests::basicTest(test::Test& test) {
    Simulation simulation;
    T_ASSERT(T_COMPARE(simulation.getAllSize(), 0));
}

void SimulationTests::boxTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box = simulation.createBox(
        "box0", b2Vec2(1.0f, 1.0f), utils::to_radians(45.0f), b2Vec2(1.0f, 1.0f), sf::Color::Green
    );
    T_ASSERT(T_CHECK(box));
    T_COMPARE(box->getName(), "box0");
    T_COMPARE(box->getId(), 0);
    T_COMPARE(box->getGlobalPosition(), b2Vec2(1.0f, 1.0f), &SimulationTests::b2Vec2ToStr);
    T_APPROX_COMPARE(box->getGlobalRotation(), utils::to_radians(45.0f));
}

void SimulationTests::ballTest(test::Test& test) {
    Simulation simulation;
    BallObject* ball = simulation.createBall(
        "ball0", b2Vec2(1.0f, 1.0f), 1.0f, sf::Color::Green, sf::Color::Green
    );
    T_ASSERT(T_CHECK(ball));
    T_COMPARE(ball->getName(), "ball0");
    T_COMPARE(ball->getId(), 0);
    T_COMPARE(ball->getGlobalPosition(), b2Vec2(1.0f, 1.0f), &SimulationTests::b2Vec2ToStr);
    T_APPROX_COMPARE(ball->getGlobalRotation(), utils::to_radians(0.0f));
}

void SimulationTests::polygonTest(test::Test& test) {
    Simulation simulation;
    std::vector<b2Vec2> vertices;
    for (size_t i = 0; i < 6; i++) {
        b2Vec2 vertex = utils::get_circle_vertex<b2Vec2>(i, 6, 1.0f);
        vertices.push_back(vertex);
    }
    PolygonObject* polygon = simulation.createPolygon(
        "polygon0", b2Vec2(1.0f, 1.0f), utils::to_radians(45.0f), vertices, sf::Color::Green
    );
    T_ASSERT(T_CHECK(polygon));
    T_COMPARE(polygon->getName(), "polygon0");
    T_COMPARE(polygon->getId(), 0);
    T_COMPARE(polygon->getGlobalPosition(), b2Vec2(1.0f, 1.0f), &SimulationTests::b2Vec2ToStr);
    T_APPROX_COMPARE(polygon->getGlobalRotation(), utils::to_radians(45.0f));
}

void SimulationTests::chainTest(test::Test& test) {
    Simulation simulation;
    std::vector<b2Vec2> vertices = {
        b2Vec2(25.0f, 8.0f),
        b2Vec2(15.0f, 2.0f),
        b2Vec2(5.0f, 0.0f),
        b2Vec2(-5.0f, 0.0f),
        b2Vec2(-15.0f, 2.0f),
        b2Vec2(-25.0f, 8.0f),
    };
    ChainObject* chain = simulation.createChain(
        "chain0", b2Vec2(1.0f, 1.0f), utils::to_radians(45.0f), vertices, sf::Color(255, 255, 255)
    );
    T_ASSERT(T_CHECK(chain));
    T_COMPARE(chain->getName(), "chain0");
    T_COMPARE(chain->getId(), 0);
    T_COMPARE(chain->getGlobalPosition(), b2Vec2(1.0f, 1.0f), &SimulationTests::b2Vec2ToStr);
    T_APPROX_COMPARE(chain->getGlobalRotation(), utils::to_radians(45.0f));
}

void SimulationTests::revoluteJointTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.0f, 0.0f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(0.0f, 5.0f));
    b2RevoluteJointDef joint_def;
    RevoluteJoint* joint = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 5.0f));
    T_ASSERT(T_CHECK(joint));
    T_ASSERT(T_COMPARE(simulation.getJointsSize(), 1));
    T_CHECK(simulation.getJoint(0) == joint);
    T_ASSERT(T_COMPARE(box0->getJoints().size(), 1));
    T_CHECK(box0->getJoint(0) == joint);
    T_ASSERT(T_COMPARE(box1->getJoints().size(), 1));
    T_CHECK(box1->getJoint(0) == joint);
    T_COMPARE(joint->getAnchorA(), b2Vec2(0.0f, 5.0f), &SimulationTests::b2Vec2ToStr);
}

void SimulationTests::carTest(test::Test& test) {
    Simulation simulation;
    std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
    std::vector<float> wheels = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    PolygonObject* car = simulation.createCar(
        "car0", b2Vec2(0.0f, 0.0f), lengths, wheels, sf::Color(255, 0, 0)
    );
    T_ASSERT(T_COMPARE(simulation.getAllSize(), 4));
    {
        T_ASSERT(T_CHECK(car));
        T_COMPARE(car->getName(), "car0");
        T_COMPARE(car->getId(), 0);
        T_VEC2_APPROX_COMPARE(car->getGlobalPosition(), b2Vec2(0.0f, 0.0f));
        T_APPROX_COMPARE(car->getGlobalRotation(), utils::to_radians(0.0f));
    }
    {
        BallObject* wheel0 = dynamic_cast<BallObject*>(simulation.getFromAll(1));
        T_ASSERT(T_CHECK(wheel0, "Object is not a BallObject"));
        T_COMPARE(wheel0->getName(), "car0 wheel0");
        T_COMPARE(wheel0->getId(), 1);
        T_CHECK(wheel0->getParent() == car);
        T_VEC2_APPROX_COMPARE(wheel0->getGlobalPosition(), b2Vec2(5.0f, 0.0f));
    }
    {
        BallObject* wheel1 = dynamic_cast<BallObject*>(simulation.getFromAll(2));
        T_ASSERT(T_CHECK(wheel1, "Object is not a BallObject"));
        T_COMPARE(wheel1->getName(), "car0 wheel1");
        T_COMPARE(wheel1->getId(), 2);
        T_CHECK(wheel1->getParent() == car);
        T_VEC2_APPROX_COMPARE(wheel1->getGlobalPosition(), b2Vec2(-2.50000024f, 4.33012676f));
    }
    {
        BallObject* wheel2 = dynamic_cast<BallObject*>(simulation.getFromAll(3));
        T_ASSERT(T_CHECK(wheel2, "Object is not a BallObject"));
        T_COMPARE(wheel2->getName(), "car0 wheel2");
        T_COMPARE(wheel2->getId(), 3);
        T_CHECK(wheel2->getParent() == car);
        T_VEC2_APPROX_COMPARE(wheel2->getGlobalPosition(), b2Vec2(-2.49999952f, -4.33012724f));
    }
}

void SimulationTests::boxSerializeTest(test::Test& test) {
    Simulation simulation;
    BoxObject* boxA = simulation.createBox(
        "box0", b2Vec2(1.5f, -3.5f), utils::to_radians(45.0f), b2Vec2(1.1f, 2.0f), sf::Color::Green
    );
    std::string str = boxA->serialize();
    std::unique_ptr<BoxObject> uptr = BoxObject::deserialize(str, &simulation);
    BoxObject* boxB = uptr.get();
    boxCmp(test, boxA, boxB);
}

void SimulationTests::ballSerializeTest(test::Test& test) {
    Simulation simulation;
    BallObject* ballA = simulation.createBall(
        "ball0", b2Vec2(1.5f, -3.5f), 2.2f, sf::Color::Green, sf::Color::Green
    );
    std::string str = ballA->serialize();
    std::unique_ptr<BallObject> uptr = BallObject::deserialize(str, &simulation);
    BallObject* ballB = uptr.get();
    ballCmp(test, ballA, ballB);
}

void SimulationTests::polygonSerializeTest(test::Test& test) {
    Simulation simulation;
    std::vector<b2Vec2> vertices;
    for (size_t i = 0; i < 6; i++) {
        b2Vec2 vertex = utils::get_circle_vertex<b2Vec2>(i, 6, 1.0f);
        vertices.push_back(vertex);
    }
    PolygonObject* polygonA = simulation.createPolygon(
        "polygon0", b2Vec2(1.5f, -3.5f), utils::to_radians(45.0f), vertices, sf::Color::Green
    );
    std::string str = polygonA->serialize();
    std::unique_ptr<PolygonObject> uptr = PolygonObject::deserialize(str, &simulation);
    PolygonObject* polygonB = uptr.get();
    polygonCmp(test, polygonA, polygonB);
}

void SimulationTests::chainSerializeTest(test::Test& test) {
    Simulation simulation;
    std::vector<b2Vec2> vertices = {
        b2Vec2(25.0f, 8.0f),
        b2Vec2(15.0f, 2.0f),
        b2Vec2(5.0f, 0.0f),
        b2Vec2(-5.0f, 0.0f),
        b2Vec2(-15.0f, 2.0f),
        b2Vec2(-25.0f, 8.0f),
    };
    ChainObject* chainA = simulation.createChain(
        "chain0", b2Vec2(1.5f, -3.5f), utils::to_radians(45.0f), vertices, sf::Color::Green
    );
    std::string str = chainA->serialize();
    std::unique_ptr<ChainObject> uptr = ChainObject::deserialize(str, &simulation);
    ChainObject* chainB = uptr.get();
    chainCmp(test, chainA, chainB);
}

void SimulationTests::revoluteJointSerializeTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = simulation.createBox(
        "box0", b2Vec2(1.5f, -3.5f), utils::to_radians(45.0f), b2Vec2(1.5f, 2.0f), sf::Color::Green
    );
    BoxObject* box1 = simulation.createBox(
        "box1", b2Vec2(0.0f, 5.0f), utils::to_radians(-10.0f), b2Vec2(3.0f, 0.5f), sf::Color::Green
    );
    b2RevoluteJointDef joint_def;
    joint_def.Initialize(box0->getRigidBody(), box1->getRigidBody(), b2Vec2(0.0f, 5.0f));
    joint_def.collideConnected = true;
    joint_def.enableLimit = true;
    joint_def.enableMotor = true;
    joint_def.lowerAngle = utils::to_radians(5.0f);
    joint_def.maxMotorTorque = 150.0f;
    joint_def.motorSpeed = 33.3f;
    joint_def.referenceAngle = utils::to_radians(1.1f);
    joint_def.upperAngle = utils::to_radians(55.0f);
    RevoluteJoint* jointA = simulation.createRevoluteJoint(joint_def, box0, box1);
    std::string str = jointA->serialize();
    std::unique_ptr<RevoluteJoint> uptr = RevoluteJoint::deserialize(str, &simulation);
    RevoluteJoint* jointB = uptr.get();
    T_ASSERT(T_COMPARE(simulation.getJointsSize(), 1));
    T_CHECK(simulation.getJoint(0) == jointA);
    T_ASSERT(T_COMPARE(box0->getJoints().size(), 1));
    T_CHECK(box0->getJoint(0) == jointA);
    T_ASSERT(T_COMPARE(box1->getJoints().size(), 1));
    T_CHECK(box1->getJoint(0) == jointA);
    revoluteJointCmp(test, jointA, jointB);
}

void SimulationTests::carSerializeTest(test::Test& test) {
    Simulation simulationA;
    std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
    std::vector<float> wheels = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    simulationA.createCar(
        "car0", b2Vec2(0.0f, 0.0f), lengths, wheels, sf::Color(255, 0, 0)
    );
    std::string str = simulationA.serialize();
    Simulation simulationB;
    simulationB.deserialize(str);
    simCmp(test, simulationA, simulationB);
}

void SimulationTests::advanceTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box = createBox(simulation, "box0", b2Vec2(0.0f, 0.0f));
    simulation.advance(1.0f / 60.0f);
    T_VEC2_APPROX_COMPARE(box->getGlobalPosition(), b2Vec2(0.0f, -0.002722f));
}

void SimulationTests::saveloadTest(test::Test& test) {
    Simulation simulationA;
    BoxObject* boxA = simulationA.createBox(
        "box0", b2Vec2(1.5f, -3.5f), utils::to_radians(45.0f), b2Vec2(1.1f, 2.0f), sf::Color::Green
    );
    const std::filesystem::path tmp_dir = "tests/tmp";
    if (!std::filesystem::exists(tmp_dir)) {
        std::filesystem::create_directory(tmp_dir);
    }
    const std::filesystem::path temp_filename = tmp_dir / "box.txt";
    simulationA.save(temp_filename.string());
    Simulation simulationB;
    simulationB.load(temp_filename.string());
    simCmp(test, simulationA, simulationB);
}

void SimulationTests::boxStackTest(test::Test& test) {
    Simulation simulationA;
    std::vector<b2Vec2> ground_vertices = {
        b2Vec2(8.0f, 0.0f),
        b2Vec2(-8.0f, 0.0f),
    };
    ChainObject* ground = simulationA.createChain(
        "ground", b2Vec2(0.0f, 0.0f), utils::to_radians(0.0f), ground_vertices, sf::Color(255, 255, 255)
    );
    BoxObject* box0 = createBox(simulationA, "box0", b2Vec2(0.0f, 0.6f));
    BoxObject* box1 = createBox(simulationA, "box1", b2Vec2(0.5f, 1.7f));
    BoxObject* box2 = createBox(simulationA, "box2", b2Vec2(1.0f, 2.8f));
    const int fps = 60;
    float advance_time = 3.0f;
    for (size_t i = 0; i < fps * advance_time; i++) {
        simulationA.advance(1.0f / fps);
    }
    Simulation simulationB;
    simulationB.load("tests/box_stack.txt");
    simCmp(test, simulationA, simulationB);
}

void SimulationTests::movingCarTest(test::Test& test) {
    Simulation simulationA;
    std::vector<b2Vec2> ground_vertices = {
        b2Vec2(8.0f, 0.0f),
        b2Vec2(-8.0f, 0.0f),
    };
    ChainObject* ground = simulationA.createChain(
        "ground", b2Vec2(0.0f, 0.0f), utils::to_radians(0.0f), ground_vertices, sf::Color(255, 255, 255)
    );
    std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
    std::vector<float> wheels = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    GameObject* car = simulationA.createCar(
        "car0", b2Vec2(0.0f, 6.0f), lengths, wheels, sf::Color(255, 0, 0)
    );
    car->setType(b2_dynamicBody, false);
    car->setDensity(1.0f, false);
    car->setFriction(0.3f, false);
    car->setRestitution(0.5f, false);
    const int fps = 60;
    size_t advance_steps = 100;
    for (size_t i = 0; i < advance_steps; i++) {
        simulationA.advance(1.0f / fps);
    }
    Simulation simulationB;
    simulationB.load("tests/moving_car.txt");
    simCmp(test, simulationA, simulationB);
}

void SimulationTests::setParentTwoTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.0f, 0.6f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(0.5f, 1.7f));
    b2Vec2 box0_local_pos_before = box0->getPosition();
    b2Vec2 box0_global_pos_before = box0->getGlobalPosition();
    b2Vec2 box1_local_pos_before = box1->getPosition();
    b2Vec2 box1_global_pos_before = box1->getGlobalPosition();
    box1->setParent(box0);
    T_CHECK(box0->getParent() == nullptr);
    T_ASSERT(T_COMPARE(box1->getParentId(), box0->getId()));
    T_ASSERT(T_COMPARE(box0->getChildren().size(), 1));
    T_CHECK(box0->getChild(0) == box1);
    b2Vec2 box0_local_pos_after = box0->getPosition();
    b2Vec2 box0_global_pos_after = box0->getGlobalPosition();
    b2Vec2 box1_local_pos_after = box1->getPosition();
    b2Vec2 box1_global_pos_after = box1->getGlobalPosition();
    T_VEC2_APPROX_COMPARE(box0_local_pos_after, box0_local_pos_before);
    T_VEC2_APPROX_COMPARE(box0_global_pos_after, box0_global_pos_before);
    T_VEC2_APPROX_COMPARE(box1_local_pos_after, box1_local_pos_before - box0_local_pos_before);
    T_VEC2_APPROX_COMPARE(box1_global_pos_after, box1_global_pos_before);
}

void SimulationTests::setParentThreeTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
    BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
    b2Vec2 box0_local_pos_before = box0->getPosition();
    b2Vec2 box0_global_pos_before = box0->getGlobalPosition();
    b2Vec2 box1_local_pos_before = box1->getPosition();
    b2Vec2 box1_global_pos_before = box1->getGlobalPosition();
    b2Vec2 box2_local_pos_before = box2->getPosition();
    b2Vec2 box2_global_pos_before = box2->getGlobalPosition();
    box1->setParent(box0);
    box2->setParent(box1);
    T_CHECK(box0->getParent() == nullptr);
    T_ASSERT(T_COMPARE(box1->getParentId(), box0->getId()));
    T_ASSERT(T_COMPARE(box2->getParentId(), box1->getId()));
    T_ASSERT(T_COMPARE(box0->getChildren().size(), 1));
    T_ASSERT(T_COMPARE(box1->getChildren().size(), 1));
    T_CHECK(box0->getChild(0) == box1);
    T_CHECK(box1->getChild(0) == box2);
    std::vector<GameObject*> box2_parent_chain = box2->getParentChain();
    T_ASSERT(T_COMPARE(box2_parent_chain.size(), 2));
    T_COMPARE(box2_parent_chain[0]->getName(), "box1");
    T_COMPARE(box2_parent_chain[1]->getName(), "box0");
    std::vector<GameObject*> box0_all_children = box0->getAllChildren();
    T_ASSERT(T_COMPARE(box0_all_children.size(), 2));
    T_COMPARE(box0_all_children[0]->getName(), "box1");
    T_COMPARE(box0_all_children[1]->getName(), "box2");
    b2Vec2 box0_local_pos_after = box0->getPosition();
    b2Vec2 box0_global_pos_after = box0->getGlobalPosition();
    b2Vec2 box1_local_pos_after = box1->getPosition();
    b2Vec2 box1_global_pos_after = box1->getGlobalPosition();
    b2Vec2 box2_local_pos_after = box2->getPosition();
    b2Vec2 box2_global_pos_after = box2->getGlobalPosition();
    T_VEC2_APPROX_COMPARE(box0_local_pos_after, box0_local_pos_before);
    T_VEC2_APPROX_COMPARE(box0_global_pos_after, box0_global_pos_before);
    T_VEC2_APPROX_COMPARE(box1_local_pos_after, box1_local_pos_before - box0_local_pos_before);
    T_VEC2_APPROX_COMPARE(box1_global_pos_after, box1_global_pos_before);
    T_VEC2_APPROX_COMPARE(box2_local_pos_after, box2_local_pos_before - box1_local_pos_before);
    T_VEC2_APPROX_COMPARE(box2_global_pos_after, box2_global_pos_before);
}

void SimulationTests::parentLoopTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
    BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
    box1->setParent(box0);
    box2->setParent(box1);
    bool exception = false;
    try {
        box0->setParent(box2);
    } catch (std::exception exc) {
        exception = true;
    }
    T_CHECK(exception);
}

void SimulationTests::setPositionTwoTest(test::Test& test) {
    Simulation simulation;
    b2Vec2 box0_initial_pos = b2Vec2(0.5f, 0.5f);
    b2Vec2 box1_initial_pos = b2Vec2(1.1f, 1.1f);
    BoxObject* box0 = createBox(simulation, "box0", box0_initial_pos);
    BoxObject* box1 = createBox(simulation, "box1", box1_initial_pos);
    b2Vec2 rel_pos = box1_initial_pos - box0_initial_pos;
    b2Vec2 box0_new_pos(1.0f, 1.0f);
    box1->setParent(box0);
    box0->setPosition(box0_new_pos);
    T_VEC2_APPROX_COMPARE(box0->getGlobalPosition(), box0_new_pos);
    T_VEC2_APPROX_COMPARE(box1->getGlobalPosition(), box0_new_pos + rel_pos);
    b2Vec2 box1_new_pos(2.0f, 2.0f);
    box1->setPosition(box1_new_pos);
    T_VEC2_APPROX_COMPARE(box0->getGlobalPosition(), box0_new_pos);
    T_VEC2_APPROX_COMPARE(box1->getGlobalPosition(), box0_new_pos + box1_new_pos);
    b2Vec2 box1_new_global_pos(2.0f, 2.0f);
    box1->setGlobalPosition(box1_new_global_pos);
    T_VEC2_APPROX_COMPARE(box0->getGlobalPosition(), box0_new_pos);
    T_VEC2_APPROX_COMPARE(box1->getPosition(), box1_new_global_pos - box0_new_pos);
    T_VEC2_APPROX_COMPARE(box1->getGlobalPosition(), box1_new_global_pos);
}

void SimulationTests::setPositionThreeTest(test::Test& test) {
    Simulation simulation;
    b2Vec2 box0_initial_pos = b2Vec2(0.5f, 0.5f);
    b2Vec2 box1_initial_pos = b2Vec2(1.1f, 1.1f);
    b2Vec2 box2_initial_pos = b2Vec2(1.75f, 1.75f);
    BoxObject* box0 = createBox(simulation, "box0", box0_initial_pos);
    BoxObject* box1 = createBox(simulation, "box1", box1_initial_pos);
    BoxObject* box2 = createBox(simulation, "box2", box2_initial_pos);
    b2Vec2 box1_box0_rel_pos = box1_initial_pos - box0_initial_pos;
    b2Vec2 box2_box1_rel_pos = box2_initial_pos - box1_initial_pos;
    b2Vec2 box0_new_pos(1.0f, 1.0f);
    box1->setParent(box0);
    box2->setParent(box1);
    box0->setPosition(box0_new_pos);
    T_VEC2_APPROX_COMPARE(box0->getGlobalPosition(), box0_new_pos);
    T_VEC2_APPROX_COMPARE(box1->getGlobalPosition(), box0_new_pos + box1_box0_rel_pos);
    T_VEC2_APPROX_COMPARE(box2->getGlobalPosition(), box0_new_pos + box1_box0_rel_pos + box2_box1_rel_pos);
    b2Vec2 box1_new_pos(2.0f, 2.0f);
    box1->setPosition(box1_new_pos);
    T_VEC2_APPROX_COMPARE(box0->getGlobalPosition(), box0_new_pos);
    T_VEC2_APPROX_COMPARE(box1->getGlobalPosition(), box0_new_pos + box1_new_pos);
    T_VEC2_APPROX_COMPARE(box2->getGlobalPosition(), box0_new_pos + box1_new_pos + box2_box1_rel_pos);
}

void SimulationTests::setAngleTest(test::Test& test) {
    Simulation simulation;
    b2Vec2 box0_initial_pos = b2Vec2(1.0f, 1.0f);
    b2Vec2 box1_initial_pos = b2Vec2(1.0f, 2.0f);
    b2Vec2 box2_initial_pos = b2Vec2(1.0f, 3.0f);
    BoxObject* box0 = createBox(simulation, "box0", box0_initial_pos);
    BoxObject* box1 = createBox(simulation, "box1", box1_initial_pos);
    BoxObject* box2 = createBox(simulation, "box2", box2_initial_pos);
    box1->setParent(box0);
    box2->setParent(box1);
    float box0_new_angle = utils::to_radians(45.0f);
    box0->setAngle(box0_new_angle);
    T_VEC2_APPROX_COMPARE(box0->getGlobalPosition(), box0_initial_pos);
    T_APPROX_COMPARE(box0->getGlobalRotation(), box0_new_angle);
    b2Vec2 box1_pos_check = utils::rotate_point(box1_initial_pos, box0_initial_pos, box0_new_angle);
    T_VEC2_APPROX_COMPARE(box1->getGlobalPosition(), box1_pos_check);
    T_APPROX_COMPARE(box1->getGlobalRotation(), box0_new_angle);
    b2Vec2 box2_pos1_check = utils::rotate_point(box2_initial_pos, box0_initial_pos, box0_new_angle);
    T_VEC2_APPROX_COMPARE(box2->getGlobalPosition(), box2_pos1_check);
    T_APPROX_COMPARE(box2->getGlobalRotation(), box0_new_angle);
    float box1_new_angle = utils::to_radians(45.0f);
    box1->setAngle(box1_new_angle);
    T_VEC2_APPROX_COMPARE(box0->getGlobalPosition(), box0_initial_pos);
    T_APPROX_COMPARE(box0->getGlobalRotation(), box0_new_angle);
    T_VEC2_APPROX_COMPARE(box1->getGlobalPosition(), box1_pos_check);
    T_APPROX_COMPARE(box1->getGlobalRotation(), box0_new_angle + box1_new_angle);
    b2Vec2 box2_pos2_check = utils::rotate_point(box2_pos1_check, box1_pos_check, box1_new_angle);
    T_VEC2_APPROX_COMPARE(box2->getGlobalPosition(), box2_pos2_check);
    T_APPROX_COMPARE(box2->getGlobalRotation(), box0_new_angle + box1_new_angle);
}

void SimulationTests::setVertexPosTest(test::Test& test) {
    Simulation simulation;
    PolygonObject* polygon = simulation.createRegularPolygon(
        "polygon", b2Vec2(0.0f, 0.0f), 0.0f, 6, 1.0f, sf::Color::Red
    );
    b2Vec2 old_vertex_pos = polygon->getGlobalVertexPos(0);
    b2Vec2 new_vertex_pos = old_vertex_pos + b2Vec2(1.0f, 0.0f);
    polygon->setGlobalVertexPos(0, new_vertex_pos);
    T_VEC2_APPROX_COMPARE(polygon->getGlobalVertexPos(0), new_vertex_pos);
}

void SimulationTests::addVertexTest(test::Test& test) {
    Simulation simulation;
    PolygonObject* polygon = simulation.createRegularPolygon(
        "polygon", b2Vec2(0.0f, 0.0f), 0.0f, 6, 1.0f, sf::Color::Red
    );
    b2Vec2 midpoint = 0.5f * (polygon->getGlobalVertexPos(0) + polygon->getGlobalVertexPos(1));
    polygon->addVertexGlobal(1, midpoint);
    T_VEC2_APPROX_COMPARE(polygon->getGlobalVertexPos(1), midpoint);
}

void SimulationTests::deleteVertexTest(test::Test& test) {
    Simulation simulation;
    PolygonObject* polygon = simulation.createRegularPolygon(
        "polygon", b2Vec2(0.0f, 0.0f), 0.0f, 6, 1.0f, sf::Color::Red
    );
    std::vector<EditableVertex> vertices = polygon->getVertices();
    polygon->tryDeleteVertex(5);
    T_ASSERT(T_COMPARE(polygon->getVertexCount(), 5));
    T_VEC2_APPROX_COMPARE(polygon->getGlobalVertexPos(0), vertices[0].pos);
    T_VEC2_APPROX_COMPARE(polygon->getGlobalVertexPos(1), vertices[1].pos);
    T_VEC2_APPROX_COMPARE(polygon->getGlobalVertexPos(2), vertices[2].pos);
    T_VEC2_APPROX_COMPARE(polygon->getGlobalVertexPos(3), vertices[3].pos);
    T_VEC2_APPROX_COMPARE(polygon->getGlobalVertexPos(4), vertices[4].pos);
    polygon->tryDeleteVertex(2);
    T_ASSERT(T_COMPARE(polygon->getVertexCount(), 4));
    T_VEC2_APPROX_COMPARE(polygon->getGlobalVertexPos(0), vertices[0].pos);
    T_VEC2_APPROX_COMPARE(polygon->getGlobalVertexPos(1), vertices[1].pos);
    T_VEC2_APPROX_COMPARE(polygon->getGlobalVertexPos(2), vertices[3].pos);
    T_VEC2_APPROX_COMPARE(polygon->getGlobalVertexPos(3), vertices[4].pos);
}

void SimulationTests::objectsTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
    BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
    box1->setParent(box0);
    T_ASSERT(T_COMPARE(simulation.getAllSize(), 3));
    T_ASSERT(T_COMPARE(simulation.getTopSize(), 2));
    T_COMPARE(simulation.getFromAll(0)->getName(), box0->getName());
    T_COMPARE(simulation.getFromAll(1)->getName(), box1->getName());
    T_COMPARE(simulation.getFromAll(2)->getName(), box2->getName());
    T_COMPARE(simulation.getFromTop(0)->getName(), box0->getName());
    T_COMPARE(simulation.getFromTop(1)->getName(), box2->getName());
    T_CHECK(simulation.getById(0) == box0);
    T_CHECK(simulation.getById(1) == box1);
    T_CHECK(simulation.getById(2) == box2);
    T_CHECK(simulation.getByName("box0") == box0);
    T_CHECK(simulation.getByName("box1") == box1);
    T_CHECK(simulation.getByName("box2") == box2);
    T_COMPARE(simulation.getAllIndex(box0), 0);
    T_COMPARE(simulation.getAllIndex(box1), 1);
    T_COMPARE(simulation.getAllIndex(box2), 2);
    T_COMPARE(simulation.getTopIndex(box0), 0);
    T_COMPARE(simulation.getTopIndex(box1), -1);
    T_COMPARE(simulation.getTopIndex(box2), 1);
    T_CHECK(simulation.contains(box0));
    T_CHECK(simulation.contains(box1));
    T_CHECK(simulation.contains(box2));
    T_ASSERT(T_COMPARE(simulation.getAllVector().size(), 3));
    T_CHECK(simulation.getAllVector()[0] == box0);
    T_CHECK(simulation.getAllVector()[1] == box1);
    T_CHECK(simulation.getAllVector()[2] == box2);
    T_ASSERT(T_COMPARE(simulation.getTopVector().size(), 2));
    T_CHECK(simulation.getTopVector()[0] == box0);
    T_CHECK(simulation.getTopVector()[1] == box2);
    T_COMPARE(simulation.getMaxId(), 2);
}

void SimulationTests::jointsTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
    BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
    RevoluteJoint* joint0 = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 0.0f));
    RevoluteJoint* joint1 = simulation.createRevoluteJoint(box1, box2, b2Vec2(0.0f, 0.0f));
    T_ASSERT(T_COMPARE(simulation.getJointsSize(), 2));
    T_CHECK(simulation.getJoint(0) == joint0);
    T_CHECK(simulation.getJoint(1) == joint1);
    T_ASSERT(T_COMPARE(box0->getJoints().size(), 1));
    T_CHECK(box0->getJoint(0) == joint0);
    T_ASSERT(T_COMPARE(box1->getJoints().size(), 2));
    T_CHECK(box1->getJoint(0) == joint0);
    T_CHECK(box1->getJoint(1) == joint1);
    T_ASSERT(T_COMPARE(box2->getJoints().size(), 1));
    T_CHECK(box2->getJoint(0) == joint1);
}

void SimulationTests::addTest(test::Test& test) {
    Simulation simulationA;
    BoxObject* box0 = createBox(simulationA, "box0", b2Vec2(1.5f, -3.5f));
    std::string str0 = box0->serialize();
    std::unique_ptr<BoxObject> uptr1 = BoxObject::deserialize(str0, &simulationA);
    BoxObject* box1 = dynamic_cast<BoxObject*>(simulationA.add(std::move(uptr1), true));
    T_ASSERT(T_COMPARE(simulationA.getAllSize(), 2));
    T_COMPARE(box1->getId(), 1);
    Simulation simulationB;
    std::string str1 = box1->serialize();
    std::unique_ptr<BoxObject> uptr2 = BoxObject::deserialize(str1, &simulationB);
    BoxObject* box2 = dynamic_cast<BoxObject*>(simulationB.add(std::move(uptr2), false));
    T_ASSERT(T_COMPARE(simulationB.getAllSize(), 1));
    boxCmp(test, box1, box2);
}

void SimulationTests::addJointTest(test::Test& test) {
    Simulation simulationA;
    BoxObject* box0A = createBox(simulationA, "box0", b2Vec2(1.5f, -3.5f));
    BoxObject* box1A = createBox(simulationA, "box1", b2Vec2(1.0f, 0.5f));
    RevoluteJoint* joint0 = simulationA.createRevoluteJoint(box0A, box1A, b2Vec2(0.0f, 0.0f));
    std::string str = joint0->serialize();
    Simulation simulationB;
    BoxObject* box0B = createBox(simulationB, "box0", b2Vec2(1.5f, -3.5f));
    BoxObject* box1B = createBox(simulationB, "box1", b2Vec2(1.0f, 0.5f));
    std::unique_ptr<RevoluteJoint> uptr = RevoluteJoint::deserialize(str, &simulationB);
    RevoluteJoint* joint1 = dynamic_cast<RevoluteJoint*>(simulationB.addJoint(std::move(uptr)));
    T_ASSERT(T_COMPARE(simulationB.getJointsSize(), 1));
    T_CHECK(simulationB.getJoint(0) == joint1);
    T_ASSERT(T_COMPARE(box0B->getJoints().size(), 1));
    T_CHECK(box0B->getJoint(0) == joint1);
    T_ASSERT(T_COMPARE(box1B->getJoints().size(), 1));
    T_CHECK(box1B->getJoint(0) == joint1);
    revoluteJointCmp(test, joint0, joint1);
}

void SimulationTests::duplicateTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
    BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
    RevoluteJoint* joint0 = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 0.0f));
    RevoluteJoint* joint1 = simulation.createRevoluteJoint(box1, box2, b2Vec2(0.0f, 0.0f));
    box1->setParent(box0);
    box2->setParent(box1);
    BoxObject* box3 = dynamic_cast<BoxObject*>(simulation.duplicate(box0));
    T_ASSERT(T_COMPARE(simulation.getAllSize(), 4));
    T_COMPARE(box3->getId(), 3);
    boxCmp(test, box0, box3, false);
    T_ASSERT(T_COMPARE(simulation.getJointsSize(), 2));
    T_CHECK(simulation.getJoint(0) == joint0);
    T_CHECK(simulation.getJoint(1) == joint1);
    T_ASSERT(T_COMPARE(box0->getJoints().size(), 1));
    T_CHECK(box0->getJoint(0) == joint0);
    T_ASSERT(T_COMPARE(box1->getJoints().size(), 2));
    T_CHECK(box1->getJoint(0) == joint0);
    T_CHECK(box1->getJoint(1) == joint1);
    T_ASSERT(T_COMPARE(box2->getJoints().size(), 1));
    T_CHECK(box2->getJoint(0) == joint1);
    T_ASSERT(T_COMPARE(box3->getJoints().size(), 0));
}

void SimulationTests::duplicateWithChildrenTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
    BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
    RevoluteJoint* joint0 = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 0.0f));
    RevoluteJoint* joint1 = simulation.createRevoluteJoint(box1, box2, b2Vec2(0.0f, 0.0f));
    box1->setParent(box0);
    box2->setParent(box1);
    BoxObject* box3 = dynamic_cast<BoxObject*>(simulation.duplicate(box0, true));
    T_ASSERT(T_COMPARE(simulation.getAllSize(), 6));
    T_ASSERT(T_COMPARE(box3->getChildren().size(), 1));
    BoxObject* box4 = dynamic_cast<BoxObject*>(box3->getChild(0));
    T_ASSERT(T_COMPARE(box4->getChildren().size(), 1));
    BoxObject* box5 = dynamic_cast<BoxObject*>(box4->getChild(0));
    T_COMPARE(box3->getId(), 3);
    T_COMPARE(box4->getId(), 4);
    T_COMPARE(box5->getId(), 5);
    boxCmp(test, box0, box3, false);
    boxCmp(test, box1, box4, false);
    boxCmp(test, box2, box5, false);
    T_ASSERT(T_COMPARE(simulation.getJointsSize(), 4));
    T_CHECK(simulation.getJoint(0) == joint0);
    T_CHECK(simulation.getJoint(1) == joint1);
    Joint* joint2 = simulation.getJoint(2);
    Joint* joint3 = simulation.getJoint(3);
    T_ASSERT(T_COMPARE(box0->getJoints().size(), 1));
    T_CHECK(box0->getJoint(0) == joint0);
    T_ASSERT(T_COMPARE(box1->getJoints().size(), 2));
    T_CHECK(box1->getJoint(0) == joint0);
    T_CHECK(box1->getJoint(1) == joint1);
    T_ASSERT(T_COMPARE(box2->getJoints().size(), 1));
    T_CHECK(box2->getJoint(0) == joint1);
    T_ASSERT(T_COMPARE(box3->getJoints().size(), 1));
    T_CHECK(box3->getJoint(0) == joint2);
    T_ASSERT(T_COMPARE(box4->getJoints().size(), 2));
    T_CHECK(box4->getJoint(0) == joint2);
    T_CHECK(box4->getJoint(1) == joint3);
    T_ASSERT(T_COMPARE(box5->getJoints().size(), 1));
    T_CHECK(box5->getJoint(0) == joint3);
}

void SimulationTests::removeJointTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
    BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
    RevoluteJoint* joint0 = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 0.0f));
    RevoluteJoint* joint1 = simulation.createRevoluteJoint(box1, box2, b2Vec2(0.0f, 0.0f));
    box1->setParent(box0);
    box2->setParent(box1);
    simulation.removeJoint(joint0);
    T_ASSERT(T_COMPARE(simulation.getJointsSize(), 1));
    T_CHECK(simulation.getJoint(0) == joint1);
    T_ASSERT(T_COMPARE(box0->getJoints().size(), 0));
    T_ASSERT(T_COMPARE(box1->getJoints().size(), 1));
    T_CHECK(box1->getJoint(0) == joint1);
    T_ASSERT(T_COMPARE(box2->getJoints().size(), 1));
    T_CHECK(box2->getJoint(0) == joint1);
}

void SimulationTests::removeTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
    BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
    RevoluteJoint* joint0 = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 0.0f));
    RevoluteJoint* joint1 = simulation.createRevoluteJoint(box1, box2, b2Vec2(0.0f, 0.0f));
    box1->setParent(box0);
    box2->setParent(box1);
    simulation.remove(box1, false);
    T_ASSERT(T_COMPARE(simulation.getAllSize(), 2));
    T_ASSERT(T_COMPARE(simulation.getTopSize(), 2));
    T_CHECK(simulation.getFromAll(0) == box0);
    T_ASSERT(T_COMPARE(box0->getChildren().size(), 0));
    T_CHECK(box0->getParent() == nullptr);
    T_CHECK(simulation.getFromAll(1) == box2);
    T_ASSERT(T_COMPARE(box2->getChildren().size(), 0));
    T_CHECK(box2->getParent() == nullptr);
    T_ASSERT(T_COMPARE(simulation.getJointsSize(), 0));
}

void SimulationTests::removeWithChildrenTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
    BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
    RevoluteJoint* joint0 = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 0.0f));
    RevoluteJoint* joint1 = simulation.createRevoluteJoint(box1, box2, b2Vec2(0.0f, 0.0f));
    box1->setParent(box0);
    box2->setParent(box1);
    simulation.remove(box1, true);
    T_ASSERT(T_COMPARE(simulation.getAllSize(), 1));
    T_ASSERT(T_COMPARE(simulation.getTopSize(), 1));
    T_CHECK(simulation.getFromAll(0) == box0);
    T_ASSERT(T_COMPARE(box0->getChildren().size(), 0));
    T_CHECK(box0->getParent() == nullptr);
    T_ASSERT(T_COMPARE(simulation.getJointsSize(), 0));
}

void SimulationTests::clearTest(test::Test& test) {
    Simulation simulation;
    BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
    BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
    BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
    RevoluteJoint* joint0 = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 0.0f));
    RevoluteJoint* joint1 = simulation.createRevoluteJoint(box1, box2, b2Vec2(0.0f, 0.0f));
    box1->setParent(box0);
    box2->setParent(box1);
    simulation.clear();
    T_ASSERT(T_COMPARE(simulation.getAllSize(), 0));
    T_ASSERT(T_COMPARE(simulation.getTopSize(), 0));
    T_ASSERT(T_COMPARE(simulation.getJointsSize(), 0));
    T_CHECK(simulation.getById(0) == nullptr);
    T_CHECK(simulation.getById(1) == nullptr);
    T_CHECK(simulation.getById(2) == nullptr);
    T_CHECK(simulation.getByName("box0") == nullptr);
    T_CHECK(simulation.getByName("box1") == nullptr);
    T_CHECK(simulation.getByName("box2") == nullptr);
}

std::string SimulationTests::colorToStr(const sf::Color& color) {
    return "(" + utils::color_to_str(color) + ")";
}

std::string SimulationTests::b2Vec2ToStr(const b2Vec2& vec) {
    return "(" + utils::vec_to_str(vec) + ")";
}

BoxObject* SimulationTests::createBox(Simulation& simulation, const std::string& name, const b2Vec2& pos) const {
    BoxObject* box = simulation.createBox(
        name,
        pos,
        utils::to_radians(0.0f),
        b2Vec2(1.0f, 1.0f),
        sf::Color(0, 255, 0)
    );
    return box;
}

void SimulationTests::objCmpCommon(test::Test& test, const GameObject* objA, const GameObject* objB, bool cmp_id) {
    if (cmp_id) {
        T_COMPARE(objB->getId(), objA->getId());
        T_COMPARE(objB->getParentId(), objA->getParentId());
        T_COMPARE(objB->getChildren().size(), objA->getChildren().size());
        if (objA->getChildren().size() == objB->getChildren().size()) {
            for (size_t i = 0; i < objA->getChildren().size(); i++) {
                T_COMPARE(objB->getChild(i)->getId(), objA->getChild(i)->getId());
            }
        }
    }
    T_COMPARE(objB->getColor(), objA->getColor(), &SimulationTests::colorToStr);
    T_COMPARE(objB->getName(), objA->getName());
    T_COMPARE(objB->getTransform().q.GetAngle(), objA->getTransform().q.GetAngle());
    T_COMPARE(objB->getTransform().p, objA->getTransform().p, &SimulationTests::b2Vec2ToStr);
    T_COMPARE(objB->getLinearVelocity(), objA->getLinearVelocity(), &SimulationTests::b2Vec2ToStr);
    T_COMPARE(objB->getAngularVelocity(), objA->getAngularVelocity());
    T_CHECK(objB->getVertices() == objA->getVertices());
}

void SimulationTests::boxCmp(test::Test& test, BoxObject* boxA, BoxObject* boxB, bool cmp_id) {
    T_ASSERT(T_CHECK(boxA && boxB, "Objects have different types"));
    T_CHECK(boxA->compare(*boxB, cmp_id));
    objCmpCommon(test, boxA, boxB, cmp_id);
    T_COMPARE(boxB->size, boxA->size, &SimulationTests::b2Vec2ToStr);
}

void SimulationTests::ballCmp(test::Test& test, BallObject* ballA, BallObject* ballB, bool cmp_id) {
    T_ASSERT(T_CHECK(ballA && ballB, "Objects have different types"));
    T_CHECK(ballA->compare(*ballB, cmp_id));
    objCmpCommon(test, ballA, ballB, cmp_id);
    T_COMPARE(ballB->radius, ballA->radius);
}

void SimulationTests::polygonCmp(test::Test& test, PolygonObject* polygonA, PolygonObject* polygonB, bool cmp_id) {
    T_ASSERT(T_CHECK(polygonA && polygonB, "Objects have different types"));
    T_CHECK(polygonA->compare(*polygonB, cmp_id));
    objCmpCommon(test, polygonA, polygonB, cmp_id);
}

void SimulationTests::chainCmp(test::Test& test, ChainObject* chainA, ChainObject* chainB, bool cmp_id) {
    T_ASSERT(T_CHECK(chainA && chainB, "Objects have different types"));
    T_CHECK(chainA->compare(*chainB, cmp_id));
    objCmpCommon(test, chainA, chainB, cmp_id);
}

void SimulationTests::jointCmpCommon(test::Test& test, Joint* jointA, Joint* jointB) {
    T_COMPARE(jointB->object1->getId(), jointA->object1->getId());
    T_COMPARE(jointB->object2->getId(), jointA->object2->getId());
    T_COMPARE(jointB->getAnchorA(), jointA->getAnchorA(), &SimulationTests::b2Vec2ToStr);
    T_COMPARE(jointB->getAnchorB(), jointA->getAnchorB(), &SimulationTests::b2Vec2ToStr);
    T_COMPARE(jointB->getCollideConnected(), jointA->getCollideConnected());
}

void SimulationTests::revoluteJointCmp(test::Test& test, RevoluteJoint* jointA, RevoluteJoint* jointB) {
    T_ASSERT(T_CHECK(jointA && jointB, "Joints have different types"));
    T_CHECK(*jointA == *jointB);
    jointCmpCommon(test, jointA, jointB);
    T_COMPARE(jointB->getLowerLimit(), jointA->getLowerLimit());
    T_COMPARE(jointB->getMaxMotorTorque(), jointA->getMaxMotorTorque());
    T_COMPARE(jointB->getMotorSpeed(), jointA->getMotorSpeed());
    T_COMPARE(jointB->getReferenceAngle(), jointA->getReferenceAngle());
    T_COMPARE(jointB->getUpperLimit(), jointA->getUpperLimit());
    T_COMPARE(jointB->isLimitEnabled(), jointA->isLimitEnabled());
    T_COMPARE(jointB->isMotorEnabled(), jointA->isMotorEnabled());
}

void SimulationTests::simCmp(test::Test& test, Simulation& simA, Simulation& simB) {
    T_CONTAINER("Sumulations are different:");
    T_ASSERT(T_COMPARE(simA.getAllSize(), simB.getAllSize()));
    for (size_t i = 0; i < simA.getAllSize(); i++) {
        GameObject* objA = simA.getFromAll(i);
        GameObject* objB = simB.getFromAll(i);
        T_CONTAINER("Objects \"" + objA->getName() + "\" and " + objB->getName() + "\" are different: ");
        switch (objA->getType()) {
            case GameObject::GameObjectType::Box:
                boxCmp(test, dynamic_cast<BoxObject*>(objA), dynamic_cast<BoxObject*>(objB));
                break;
            case GameObject::GameObjectType::Ball:
                ballCmp(test, dynamic_cast<BallObject*>(objA), dynamic_cast<BallObject*>(objB));
                break;
            case GameObject::GameObjectType::Polygon:
                polygonCmp(test, dynamic_cast<PolygonObject*>(objA), dynamic_cast<PolygonObject*>(objB));
                break;
            case GameObject::GameObjectType::Chain:
                chainCmp(test, dynamic_cast<ChainObject*>(objA), dynamic_cast<ChainObject*>(objB));
                break;
            default:
                mAssert(false, "Unknown GameObject type");
        }
    }
    T_ASSERT(T_COMPARE(simA.getJointsSize(), simB.getJointsSize()));
    for (size_t i = 0; i < simA.getJointsSize(); i++) {
        Joint* jointA = simA.getJoint(i);
        Joint* jointB = simB.getJoint(i);
        std::string jointA_str = std::to_string(jointA->object1->getId()) + "-" + std::to_string(jointA->object2->getId());
        std::string jointB_str = std::to_string(jointB->object1->getId()) + "-" + std::to_string(jointB->object2->getId());
        T_CONTAINER("Joints " + jointA_str + " and " + jointB_str + " are different: ");
        switch (jointA->getType()) {
            case b2JointType::e_revoluteJoint:
                revoluteJointCmp(test, dynamic_cast<RevoluteJoint*>(jointA), dynamic_cast<RevoluteJoint*>(jointB));
                break;
            default:
                mAssert(false, "Unknown Joint type");
        }
    }
}
