#include "tests/simulation_tests.h"

SimulationTests::SimulationTests(test::TestManager& manager) : TestModule("Simulation", manager) { }

void SimulationTests::createTestLists() {
    createSimulationList();
    createGameObjectList();
    createObjectListList();
}

void SimulationTests::createSimulationList() {
    test::TestList* list = createTestList("Simulation");

    test::Test* basic_test = list->addTest(
        "basic",
        [&](test::Test& test) {
            Simulation simulation;
            tAssert(tCompare(simulation.getAllSize(), 0));
        }
    );
    test::Test* box_test = list->addTest(
        "box",
        {
            basic_test
        },
        [&](test::Test& test) {
            Simulation simulation;
            BoxObject* box = simulation.createBox(
                "box0", b2Vec2(1.0f, 1.0f), utils::to_radians(45.0f), b2Vec2(1.0f, 1.0f), sf::Color::Green
            );
            tAssert(tCheck(box));
            tCompare(box->getName(), "box0");
            tCompare(box->getId(), 0);
            tCompare(box->getGlobalPosition(), b2Vec2(1.0f, 1.0f), &SimulationTests::b2Vec2ToStr);
            tApproxCompare(box->getGlobalRotation(), utils::to_radians(45.0f));
        }
    );
    test::Test* ball_test = list->addTest(
        "ball",
        {
            basic_test
        },
        [&](test::Test& test) {
            Simulation simulation;
            BallObject* ball = simulation.createBall(
                "ball0", b2Vec2(1.0f, 1.0f), 1.0f, sf::Color::Green, sf::Color::Green
            );
            tAssert(tCheck(ball));
            tCompare(ball->getName(), "ball0");
            tCompare(ball->getId(), 0);
            tCompare(ball->getGlobalPosition(), b2Vec2(1.0f, 1.0f), &SimulationTests::b2Vec2ToStr);
            tApproxCompare(ball->getGlobalRotation(), utils::to_radians(0.0f));
        }
    );
    test::Test* polygon_test = list->addTest(
        "polygon",
        {
            basic_test
        },
        [&](test::Test& test) {
            Simulation simulation;
            std::vector<b2Vec2> vertices;
            for (size_t i = 0; i < 6; i++) {
                b2Vec2 vertex = utils::get_circle_vertex<b2Vec2>(i, 6, 1.0f);
                vertices.push_back(vertex);
            }
            PolygonObject* polygon = simulation.createPolygon(
                "polygon0", b2Vec2(1.0f, 1.0f), utils::to_radians(45.0f), vertices, sf::Color::Green
            );
            tAssert(tCheck(polygon));
            tCompare(polygon->getName(), "polygon0");
            tCompare(polygon->getId(), 0);
            tCompare(polygon->getGlobalPosition(), b2Vec2(1.0f, 1.0f), &SimulationTests::b2Vec2ToStr);
            tApproxCompare(polygon->getGlobalRotation(), utils::to_radians(45.0f));
        }
    );
    test::Test* chain_test = list->addTest(
        "chain",
        {
            basic_test
        },
        [&](test::Test& test) {
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
            tAssert(tCheck(chain));
            tCompare(chain->getName(), "chain0");
            tCompare(chain->getId(), 0);
            tCompare(chain->getGlobalPosition(), b2Vec2(1.0f, 1.0f), &SimulationTests::b2Vec2ToStr);
            tApproxCompare(chain->getGlobalRotation(), utils::to_radians(45.0f));
        }
    );
    test::Test* revolute_joint_test = list->addTest(
        "revolute_joint",
        {
            box_test
        },
        [&](test::Test& test) {
            Simulation simulation;
            BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.0f, 0.0f));
            BoxObject* box1 = createBox(simulation, "box1", b2Vec2(0.0f, 5.0f));
            b2RevoluteJointDef joint_def;
            RevoluteJoint* joint = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 5.0f));
            tAssert(tCheck(joint));
            tCompare(joint->getAnchorA(), b2Vec2(0.0f, 5.0f), &SimulationTests::b2Vec2ToStr);
        }
    );
    test::Test* car_test = list->addTest(
        "car",
        {
            ball_test,
            polygon_test,
            revolute_joint_test
        },
        [&](test::Test& test) {
            Simulation simulation;
            std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
            std::vector<float> wheels = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
            PolygonObject* car = simulation.createCar(
                "car0", b2Vec2(0.0f, 0.0f), lengths, wheels, sf::Color(255, 0, 0)
            );
            tAssert(tCompare(simulation.getAllSize(), 4));
            {
                tAssert(tCheck(car));
                tCompare(car->getName(), "car0");
                tCompare(car->getId(), 0);
                tVec2ApproxCompare(car->getGlobalPosition(), b2Vec2(0.0f, 0.0f));
                tApproxCompare(car->getGlobalRotation(), utils::to_radians(0.0f));
            }
            {
                BallObject* wheel0 = dynamic_cast<BallObject*>(simulation.getFromAll(1));
                tAssert(tCheck(wheel0, "Object is not a BallObject"));
                tCompare(wheel0->getName(), "car0 wheel0");
                tCompare(wheel0->getId(), 1);
                tCheck(wheel0->getParent() == car);
                tVec2ApproxCompare(wheel0->getGlobalPosition(), b2Vec2(5.0f, 0.0f));
            }
            {
                BallObject* wheel1 = dynamic_cast<BallObject*>(simulation.getFromAll(2));
                tAssert(tCheck(wheel1, "Object is not a BallObject"));
                tCompare(wheel1->getName(), "car0 wheel1");
                tCompare(wheel1->getId(), 2);
                tCheck(wheel1->getParent() == car);
                tVec2ApproxCompare(wheel1->getGlobalPosition(), b2Vec2(-2.50000024f, 4.33012676f));
            }
            {
                BallObject* wheel2 = dynamic_cast<BallObject*>(simulation.getFromAll(3));
                tAssert(tCheck(wheel2, "Object is not a BallObject"));
                tCompare(wheel2->getName(), "car0 wheel2");
                tCompare(wheel2->getId(), 3);
                tCheck(wheel2->getParent() == car);
                tVec2ApproxCompare(wheel2->getGlobalPosition(), b2Vec2(-2.49999952f, -4.33012724f));
            }
        }
    );
    test::Test* box_serialize_test = list->addTest(
        "box_serialize",
        {
            box_test
        },
        [=, this](test::Test& test) {
            Simulation simulation;
            BoxObject* boxA = simulation.createBox(
                "box0", b2Vec2(1.5f, -3.5f), utils::to_radians(45.0f), b2Vec2(1.1f, 2.0f), sf::Color::Green
            );
            std::string str = boxA->serialize();
            std::unique_ptr<BoxObject> uptr = BoxObject::deserialize(str, &simulation);
            BoxObject* boxB = uptr.get();
            boxCmp(test, boxA, boxB);
        }
    );
    test::Test* ball_serialize_test = list->addTest(
        "ball_serialize",
        {
            ball_test
        },
        [=, this](test::Test& test) {
            Simulation simulation;
            BallObject* ballA = simulation.createBall(
                "ball0", b2Vec2(1.5f, -3.5f), 2.2f, sf::Color::Green, sf::Color::Green
            );
            std::string str = ballA->serialize();
            std::unique_ptr<BallObject> uptr = BallObject::deserialize(str, &simulation);
            BallObject* ballB = uptr.get();
            ballCmp(test, ballA, ballB);
        }
    );
    test::Test* polygon_serialize_test = list->addTest(
        "polygon_serialize",
        {
            polygon_test
        },
        [=, this](test::Test& test) {
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
    );
    test::Test* chain_serialize_test = list->addTest(
        "chain_serialize",
        {
            chain_test
        },
        [=, this](test::Test& test) {
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
    );
    test::Test* revolute_joint_serialize_test = list->addTest(
        "revolute_joint_serialize",
        {
            revolute_joint_test
        },
        [=, this](test::Test& test) {
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
            revoluteJointCmp(test, jointA, jointB);
        }
    );
    test::Test* car_serialize_test = list->addTest(
        "car_serialize",
        {
            car_test,
            ball_serialize_test,
            polygon_serialize_test,
            revolute_joint_serialize_test
        },
        [&](test::Test& test) {
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
    );
    test::Test* advance_test = list->addTest(
        "advance",
        {
            box_test
        },
        [&](test::Test& test) {
            Simulation simulation;
            BoxObject* box = createBox(simulation, "box0", b2Vec2(0.0f, 0.0f));
            simulation.advance(1.0f / 60.0f);
            tVec2ApproxCompare(box->getGlobalPosition(), b2Vec2(0.0f, -0.002722f));
        }
    );
    test::Test* saveload_test = list->addTest(
        "saveload",
        {
            box_test,
            box_serialize_test
        },
        [=, this](test::Test& test) {
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
            tAssert(tCompare(simulationB.getAllSize(), 1));
            BoxObject* boxB = dynamic_cast<BoxObject*>(simulationB.getFromAll(0));
            tAssert(tCheck(boxB, "Object is not a BoxObject"));
            boxCmp(test, boxA, boxB);
        }
    );
    test::Test* box_stack_test = list->addTest(
        "box_stack",
        {
            advance_test,
            saveload_test
        },
        [&](test::Test& test) {
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
    );
    test::Test* moving_car_test = list->addTest(
        "moving_car",
        {
            advance_test,
            saveload_test,
            car_serialize_test
        },
        [&](test::Test& test) {
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
    );
}

void SimulationTests::createGameObjectList() {
    test::TestList* list = createTestList("GameObject");

    test::Test* set_parent_two_test = list->addTest(
        "set_parent_two",
        [&](test::Test& test) {
            Simulation simulation;
            BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.0f, 0.6f));
            BoxObject* box1 = createBox(simulation, "box1", b2Vec2(0.5f, 1.7f));
            b2Vec2 box0_local_pos_before = box0->getPosition();
            b2Vec2 box0_global_pos_before = box0->getGlobalPosition();
            b2Vec2 box1_local_pos_before = box1->getPosition();
            b2Vec2 box1_global_pos_before = box1->getGlobalPosition();
            box1->setParent(box0);
            tCheck(box0->getParent() == nullptr);
            tAssert(tCompare(box1->getParentId(), box0->getId()));
            tAssert(tCompare(box0->getChildren().size(), 1));
            tCheck(box0->getChild(0) == box1);
            b2Vec2 box0_local_pos_after = box0->getPosition();
            b2Vec2 box0_global_pos_after = box0->getGlobalPosition();
            b2Vec2 box1_local_pos_after = box1->getPosition();
            b2Vec2 box1_global_pos_after = box1->getGlobalPosition();
            tVec2ApproxCompare(box0_local_pos_after, box0_local_pos_before);
            tVec2ApproxCompare(box0_global_pos_after, box0_global_pos_before);
            tVec2ApproxCompare(box1_local_pos_after, box1_local_pos_before - box0_local_pos_before);
            tVec2ApproxCompare(box1_global_pos_after, box1_global_pos_before);
        }
    );
    test::Test* set_parent_three_test = list->addTest(
        "set_parent_three",
        {
            set_parent_two_test
        },
        [&](test::Test& test) {
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
            tCheck(box0->getParent() == nullptr);
            tAssert(tCompare(box1->getParentId(), box0->getId()));
            tAssert(tCompare(box2->getParentId(), box1->getId()));
            tAssert(tCompare(box0->getChildren().size(), 1));
            tAssert(tCompare(box1->getChildren().size(), 1));
            tCheck(box0->getChild(0) == box1);
            tCheck(box1->getChild(0) == box2);
            std::vector<GameObject*> box2_parent_chain = box2->getParentChain();
            tAssert(tCompare(box2_parent_chain.size(), 2));
            tCompare(box2_parent_chain[0]->getName(), "box1");
            tCompare(box2_parent_chain[1]->getName(), "box0");
            std::vector<GameObject*> box0_all_children = box0->getAllChildren();
            tAssert(tCompare(box0_all_children.size(), 2));
            tCompare(box0_all_children[0]->getName(), "box1");
            tCompare(box0_all_children[1]->getName(), "box2");
            b2Vec2 box0_local_pos_after = box0->getPosition();
            b2Vec2 box0_global_pos_after = box0->getGlobalPosition();
            b2Vec2 box1_local_pos_after = box1->getPosition();
            b2Vec2 box1_global_pos_after = box1->getGlobalPosition();
            b2Vec2 box2_local_pos_after = box2->getPosition();
            b2Vec2 box2_global_pos_after = box2->getGlobalPosition();
            tVec2ApproxCompare(box0_local_pos_after, box0_local_pos_before);
            tVec2ApproxCompare(box0_global_pos_after, box0_global_pos_before);
            tVec2ApproxCompare(box1_local_pos_after, box1_local_pos_before - box0_local_pos_before);
            tVec2ApproxCompare(box1_global_pos_after, box1_global_pos_before);
            tVec2ApproxCompare(box2_local_pos_after, box2_local_pos_before - box1_local_pos_before);
            tVec2ApproxCompare(box2_global_pos_after, box2_global_pos_before);
        }
    );
    test::Test* set_position_two_test = list->addTest(
        "set_position_two",
        {
            set_parent_two_test
        },
        [&](test::Test& test) {
            Simulation simulation;
            b2Vec2 box0_initial_pos = b2Vec2(0.5f, 0.5f);
            b2Vec2 box1_initial_pos = b2Vec2(1.1f, 1.1f);
            BoxObject* box0 = createBox(simulation, "box0", box0_initial_pos);
            BoxObject* box1 = createBox(simulation, "box1", box1_initial_pos);
            b2Vec2 rel_pos = box1_initial_pos - box0_initial_pos;
            b2Vec2 box0_new_pos(1.0f, 1.0f);
            box1->setParent(box0);
            box0->setPosition(box0_new_pos);
            tVec2ApproxCompare(box0->getGlobalPosition(), box0_new_pos);
            tVec2ApproxCompare(box1->getGlobalPosition(), box0_new_pos + rel_pos);
            b2Vec2 box1_new_pos(2.0f, 2.0f);
            box1->setPosition(box1_new_pos);
            tVec2ApproxCompare(box0->getGlobalPosition(), box0_new_pos);
            tVec2ApproxCompare(box1->getGlobalPosition(), box0_new_pos + box1_new_pos);
            b2Vec2 box1_new_global_pos(2.0f, 2.0f);
            box1->setGlobalPosition(box1_new_global_pos);
            tVec2ApproxCompare(box0->getGlobalPosition(), box0_new_pos);
            tVec2ApproxCompare(box1->getPosition(), box1_new_global_pos - box0_new_pos);
            tVec2ApproxCompare(box1->getGlobalPosition(), box1_new_global_pos);
        }
    );
    test::Test* set_position_three_test = list->addTest(
        "set_position_three",
        {
            set_parent_two_test,
            set_position_two_test
        },
        [&](test::Test& test) {
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
            tVec2ApproxCompare(box0->getGlobalPosition(), box0_new_pos);
            tVec2ApproxCompare(box1->getGlobalPosition(), box0_new_pos + box1_box0_rel_pos);
            tVec2ApproxCompare(box2->getGlobalPosition(), box0_new_pos + box1_box0_rel_pos + box2_box1_rel_pos);
            b2Vec2 box1_new_pos(2.0f, 2.0f);
            box1->setPosition(box1_new_pos);
            tVec2ApproxCompare(box0->getGlobalPosition(), box0_new_pos);
            tVec2ApproxCompare(box1->getGlobalPosition(), box0_new_pos + box1_new_pos);
            tVec2ApproxCompare(box2->getGlobalPosition(), box0_new_pos + box1_new_pos + box2_box1_rel_pos);
        }
    );
    test::Test* set_angle_test = list->addTest(
        "set_angle",
        {
            set_parent_three_test,
        },
        [&](test::Test& test) {
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
            tVec2ApproxCompare(box0->getGlobalPosition(), box0_initial_pos);
            tApproxCompare(box0->getGlobalRotation(), box0_new_angle);
            b2Vec2 box1_pos_check = utils::rotate_point(box1_initial_pos, box0_initial_pos, box0_new_angle);
            tVec2ApproxCompare(box1->getGlobalPosition(), box1_pos_check);
            tApproxCompare(box1->getGlobalRotation(), box0_new_angle);
            b2Vec2 box2_pos1_check = utils::rotate_point(box2_initial_pos, box0_initial_pos, box0_new_angle);
            tVec2ApproxCompare(box2->getGlobalPosition(), box2_pos1_check);
            tApproxCompare(box2->getGlobalRotation(), box0_new_angle);
            float box1_new_angle = utils::to_radians(45.0f);
            box1->setAngle(box1_new_angle);
            tVec2ApproxCompare(box0->getGlobalPosition(), box0_initial_pos);
            tApproxCompare(box0->getGlobalRotation(), box0_new_angle);
            tVec2ApproxCompare(box1->getGlobalPosition(), box1_pos_check);
            tApproxCompare(box1->getGlobalRotation(), box0_new_angle + box1_new_angle);
            b2Vec2 box2_pos2_check = utils::rotate_point(box2_pos1_check, box1_pos_check, box1_new_angle);
            tVec2ApproxCompare(box2->getGlobalPosition(), box2_pos2_check);
            tApproxCompare(box2->getGlobalRotation(), box0_new_angle + box1_new_angle);
        }
    );
    test::Test* set_vertex_pos_test = list->addTest(
        "set_vertex_pos",
        [&](test::Test& test) {
            Simulation simulation;
            PolygonObject* polygon = simulation.createRegularPolygon(
                "polygon", b2Vec2(0.0f, 0.0f), 0.0f, 6, 1.0f, sf::Color::Red
            );
            b2Vec2 old_vertex_pos = polygon->getGlobalVertexPos(0);
            b2Vec2 new_vertex_pos = old_vertex_pos + b2Vec2(1.0f, 0.0f);
            polygon->setGlobalVertexPos(0, new_vertex_pos);
            tVec2ApproxCompare(polygon->getGlobalVertexPos(0), new_vertex_pos);
        }
    );
    test::Test* add_vertex_test = list->addTest(
        "add_vertex",
        {
            set_vertex_pos_test
        },
        [&](test::Test& test) {
            Simulation simulation;
            PolygonObject* polygon = simulation.createRegularPolygon(
                "polygon", b2Vec2(0.0f, 0.0f), 0.0f, 6, 1.0f, sf::Color::Red
            );
            b2Vec2 midpoint = 0.5f * (polygon->getGlobalVertexPos(0) + polygon->getGlobalVertexPos(1));
            polygon->addVertexGlobal(1, midpoint);
            tVec2ApproxCompare(polygon->getGlobalVertexPos(1), midpoint);
        }
    );
    test::Test* delete_vertex_test = list->addTest(
        "delete_vertex",
        {
            set_vertex_pos_test
        },
        [&](test::Test& test) {
            Simulation simulation;
            PolygonObject* polygon = simulation.createRegularPolygon(
                "polygon", b2Vec2(0.0f, 0.0f), 0.0f, 6, 1.0f, sf::Color::Red
            );
            std::vector<EditableVertex> vertices = polygon->getVertices();
            polygon->tryDeleteVertex(5);
            tAssert(tCompare(polygon->getVertexCount(), 5));
            tVec2ApproxCompare(polygon->getGlobalVertexPos(0), vertices[0].pos);
            tVec2ApproxCompare(polygon->getGlobalVertexPos(1), vertices[1].pos);
            tVec2ApproxCompare(polygon->getGlobalVertexPos(2), vertices[2].pos);
            tVec2ApproxCompare(polygon->getGlobalVertexPos(3), vertices[3].pos);
            tVec2ApproxCompare(polygon->getGlobalVertexPos(4), vertices[4].pos);
            polygon->tryDeleteVertex(2);
            tAssert(tCompare(polygon->getVertexCount(), 4));
            tVec2ApproxCompare(polygon->getGlobalVertexPos(0), vertices[0].pos);
            tVec2ApproxCompare(polygon->getGlobalVertexPos(1), vertices[1].pos);
            tVec2ApproxCompare(polygon->getGlobalVertexPos(2), vertices[3].pos);
            tVec2ApproxCompare(polygon->getGlobalVertexPos(3), vertices[4].pos);
        }
    );
}

void SimulationTests::createObjectListList() {
    test::TestList* list = createTestList("ObjectList");

    test::Test* objects_test = list->addTest(
        "objects",
        [&](test::Test& test) {
            Simulation simulation;
            BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
            BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
            BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
            box1->setParent(box0);
            tAssert(tCompare(simulation.getAllSize(), 3));
            tAssert(tCompare(simulation.getTopSize(), 2));
            tCompare(simulation.getFromAll(0)->getName(), box0->getName());
            tCompare(simulation.getFromAll(1)->getName(), box1->getName());
            tCompare(simulation.getFromAll(2)->getName(), box2->getName());
            tCompare(simulation.getFromTop(0)->getName(), box0->getName());
            tCompare(simulation.getFromTop(1)->getName(), box2->getName());
            tCheck(simulation.getById(0) == box0);
            tCheck(simulation.getById(1) == box1);
            tCheck(simulation.getById(2) == box2);
            tCheck(simulation.getByName("box0") == box0);
            tCheck(simulation.getByName("box1") == box1);
            tCheck(simulation.getByName("box2") == box2);
            tCompare(simulation.getAllIndex(box0), 0);
            tCompare(simulation.getAllIndex(box1), 1);
            tCompare(simulation.getAllIndex(box2), 2);
            tCompare(simulation.getTopIndex(box0), 0);
            tCompare(simulation.getTopIndex(box1), -1);
            tCompare(simulation.getTopIndex(box2), 1);
            tCheck(simulation.contains(box0));
            tCheck(simulation.contains(box1));
            tCheck(simulation.contains(box2));
            tAssert(tCompare(simulation.getAllVector().size(), 3));
            tCheck(simulation.getAllVector()[0] == box0);
            tCheck(simulation.getAllVector()[1] == box1);
            tCheck(simulation.getAllVector()[2] == box2);
            tAssert(tCompare(simulation.getTopVector().size(), 2));
            tCheck(simulation.getTopVector()[0] == box0);
            tCheck(simulation.getTopVector()[1] == box2);
            tCompare(simulation.getMaxId(), 2);
        }
    );
    test::Test* joints_test = list->addTest(
        "joints",
        [&](test::Test& test) {
            Simulation simulation;
            BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
            BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
            BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
            RevoluteJoint* joint0 = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 0.0f));
            RevoluteJoint* joint1 = simulation.createRevoluteJoint(box1, box2, b2Vec2(0.0f, 0.0f));
            tAssert(tCompare(simulation.getJointsSize(), 2));
            tCheck(simulation.getJoint(0) == joint0);
            tCheck(simulation.getJoint(1) == joint1);
        }
    );
    test::Test* add_test = list->addTest(
        "add",
        {
            objects_test
        },
        [&](test::Test& test) {
            Simulation simulationA;
            BoxObject* box0 = createBox(simulationA, "box0", b2Vec2(1.5f, -3.5f));
            std::string str0 = box0->serialize();
            std::unique_ptr<BoxObject> uptr1 = BoxObject::deserialize(str0, &simulationA);
            BoxObject* box1 = dynamic_cast<BoxObject*>(simulationA.add(std::move(uptr1), true));
            tAssert(tCompare(simulationA.getAllSize(), 2));
            tCompare(box1->getId(), 1);
            Simulation simulationB;
            std::string str1 = box1->serialize();
            std::unique_ptr<BoxObject> uptr2 = BoxObject::deserialize(str1, &simulationB);
            BoxObject* box2 = dynamic_cast<BoxObject*>(simulationB.add(std::move(uptr2), false));
            tAssert(tCompare(simulationB.getAllSize(), 1));
            boxCmp(test, box1, box2);
        }
    );
    test::Test* add_joint_test = list->addTest(
        "add_joint",
        {
            joints_test
        },
        [&](test::Test& test) {
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
            tAssert(tCompare(simulationB.getJointsSize(), 1));
            revoluteJointCmp(test, joint0, joint1);
        }
    );
    test::Test* duplicate_test = list->addTest(
        "duplicate",
        {
            add_test,
            add_joint_test
        },
        [&](test::Test& test) {
            Simulation simulation;
            BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
            BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
            BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
            RevoluteJoint* joint0 = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 0.0f));
            RevoluteJoint* joint1 = simulation.createRevoluteJoint(box1, box2, b2Vec2(0.0f, 0.0f));
            box1->setParent(box0);
            box2->setParent(box1);
            BoxObject* box3 = dynamic_cast<BoxObject*>(simulation.duplicate(box0));
            tAssert(tCompare(simulation.getAllSize(), 4));
            tCompare(box3->getId(), 3);
            boxCmp(test, box0, box3, false);
        }
    );
    test::Test* duplicate_with_children_test = list->addTest(
        "duplicate_with_children",
        {
            duplicate_test
        },
        [&](test::Test& test) {
            Simulation simulation;
            BoxObject* box0 = createBox(simulation, "box0", b2Vec2(0.5f, 0.5f));
            BoxObject* box1 = createBox(simulation, "box1", b2Vec2(1.1f, 1.1f));
            BoxObject* box2 = createBox(simulation, "box2", b2Vec2(1.75f, 1.75f));
            RevoluteJoint* joint0 = simulation.createRevoluteJoint(box0, box1, b2Vec2(0.0f, 0.0f));
            RevoluteJoint* joint1 = simulation.createRevoluteJoint(box1, box2, b2Vec2(0.0f, 0.0f));
            box1->setParent(box0);
            box2->setParent(box1);
            BoxObject* box3 = dynamic_cast<BoxObject*>(simulation.duplicate(box0, true));
            tAssert(tCompare(simulation.getAllSize(), 6));
            tAssert(tCompare(box3->getChildren().size(), 1));
            BoxObject* box4 = dynamic_cast<BoxObject*>(box3->getChild(0));
            tAssert(tCompare(box4->getChildren().size(), 1));
            BoxObject* box5 = dynamic_cast<BoxObject*>(box4->getChild(0));
            tCompare(box3->getId(), 3);
            tCompare(box4->getId(), 4);
            tCompare(box5->getId(), 5);
            boxCmp(test, box0, box3, false);
            boxCmp(test, box1, box4, false);
            boxCmp(test, box2, box5, false);
        }
    );
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
        tCompare(objB->getId(), objA->getId());
        tCompare(objB->getParentId(), objA->getParentId());
        tCompare(objB->getChildren().size(), objA->getChildren().size());
        if (objA->getChildren().size() == objB->getChildren().size()) {
            for (size_t i = 0; i < objA->getChildren().size(); i++) {
                tCompare(objB->getChild(i)->getId(), objA->getChild(i)->getId());
            }
        }
    }
    tCompare(objB->getColor(), objA->getColor(), &SimulationTests::colorToStr);
    tCompare(objB->getName(), objA->getName());
    tCompare(objB->getTransform().q.GetAngle(), objA->getTransform().q.GetAngle());
    tCompare(objB->getTransform().p, objA->getTransform().p, &SimulationTests::b2Vec2ToStr);
    tCompare(objB->getLinearVelocity(), objA->getLinearVelocity(), &SimulationTests::b2Vec2ToStr);
    tCompare(objB->getAngularVelocity(), objA->getAngularVelocity());
    tCheck(objB->getVertices() == objA->getVertices());
}

void SimulationTests::boxCmp(test::Test& test, BoxObject* boxA, BoxObject* boxB, bool cmp_id) {
    tAssert(tCheck(boxA && boxB, "Objects have different types"));
    tCheck(boxA->compare(*boxB, cmp_id));
    objCmpCommon(test, boxA, boxB, cmp_id);
    tCompare(boxB->size, boxA->size, &SimulationTests::b2Vec2ToStr);
}

void SimulationTests::ballCmp(test::Test& test, BallObject* ballA, BallObject* ballB, bool cmp_id) {
    tAssert(tCheck(ballA && ballB, "Objects have different types"));
    tCheck(ballA->compare(*ballB, cmp_id));
    objCmpCommon(test, ballA, ballB, cmp_id);
    tCompare(ballB->radius, ballA->radius);
}

void SimulationTests::polygonCmp(test::Test& test, PolygonObject* polygonA, PolygonObject* polygonB, bool cmp_id) {
    tAssert(tCheck(polygonA && polygonB, "Objects have different types"));
    tCheck(polygonA->compare(*polygonB, cmp_id));
    objCmpCommon(test, polygonA, polygonB, cmp_id);
}

void SimulationTests::chainCmp(test::Test& test, ChainObject* chainA, ChainObject* chainB, bool cmp_id) {
    tAssert(tCheck(chainA && chainB, "Objects have different types"));
    tCheck(chainA->compare(*chainB, cmp_id));
    objCmpCommon(test, chainA, chainB, cmp_id);
}

void SimulationTests::jointCmpCommon(test::Test& test, Joint* jointA, Joint* jointB) {
    tCompare(jointB->object1->getId(), jointA->object1->getId());
    tCompare(jointB->object2->getId(), jointA->object2->getId());
    tCompare(jointB->getAnchorA(), jointA->getAnchorA(), &SimulationTests::b2Vec2ToStr);
    tCompare(jointB->getAnchorB(), jointA->getAnchorB(), &SimulationTests::b2Vec2ToStr);
    tCompare(jointB->getCollideConnected(), jointA->getCollideConnected());
}

void SimulationTests::revoluteJointCmp(test::Test& test, RevoluteJoint* jointA, RevoluteJoint* jointB) {
    tAssert(tCheck(jointA && jointB, "Joints have different types"));
    tCheck(*jointA == *jointB);
    jointCmpCommon(test, jointA, jointB);
    tCompare(jointB->getLowerLimit(), jointA->getLowerLimit());
    tCompare(jointB->getMaxMotorTorque(), jointA->getMaxMotorTorque());
    tCompare(jointB->getMotorSpeed(), jointA->getMotorSpeed());
    tCompare(jointB->getReferenceAngle(), jointA->getReferenceAngle());
    tCompare(jointB->getUpperLimit(), jointA->getUpperLimit());
    tCompare(jointB->isLimitEnabled(), jointA->isLimitEnabled());
    tCompare(jointB->isMotorEnabled(), jointA->isMotorEnabled());
}

void SimulationTests::simCmp(test::Test& test, Simulation& simA, Simulation& simB) {
    tContainer("Sumulations are different:");
    tAssert(tCompare(simA.getAllSize(), simB.getAllSize()));
    for (size_t i = 0; i < simA.getAllSize(); i++) {
        GameObject* objA = simA.getFromAll(i);
        GameObject* objB = simB.getFromAll(i);
        tContainer("Objects \"" + objA->getName() + "\" and " + objB->getName() + "\" are different: ");
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
    tAssert(tCompare(simA.getJointsSize(), simB.getJointsSize()));
    for (size_t i = 0; i < simA.getJointsSize(); i++) {
        Joint* jointA = simA.getJoint(i);
        Joint* jointB = simB.getJoint(i);
        std::string jointA_str = std::to_string(jointA->object1->getId()) + "-" + std::to_string(jointA->object2->getId());
        std::string jointB_str = std::to_string(jointB->object1->getId()) + "-" + std::to_string(jointB->object2->getId());
        tContainer("Joints " + jointA_str + " and " + jointB_str + " are different: ");
        switch (jointA->getType()) {
            case b2JointType::e_revoluteJoint:
                revoluteJointCmp(test, dynamic_cast<RevoluteJoint*>(jointA), dynamic_cast<RevoluteJoint*>(jointB));
                break;
            default:
                mAssert(false, "Unknown Joint type");
        }
    }
}
