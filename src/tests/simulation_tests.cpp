#include "tests/simulation_tests.h"

SimulationTests::SimulationTests(test::TestManager& manager) : TestModule("Simulation", manager) { }

void SimulationTests::createTestLists() {
    createSimulationList();
    createGameObjectList();
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
            simulation.createBox(
                "box0",
                b2Vec2(1.0f, 1.0f),
                utils::to_radians(45.0f),
                b2Vec2(1.0f, 1.0f),
                sf::Color::Green
            );
            tAssert(tCompare(simulation.getAllSize(), 1));
            BoxObject* box = dynamic_cast<BoxObject*>(simulation.getFromAll(0));
            tAssert(tCheck(box, "Object is not a BoxObject"));
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
            simulation.createBall(
                "ball0",
                b2Vec2(1.0f, 1.0f),
                1.0f,
                sf::Color::Green,
                sf::Color::Green
            );
            tAssert(tCompare(simulation.getAllSize(), 1));
            BallObject* ball = dynamic_cast<BallObject*>(simulation.getFromAll(0));
            tAssert(tCheck(ball, "Object is not a BallObject"));
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
            simulation.createPolygon(
                "polygon0",
                b2Vec2(1.0f, 1.0f),
                utils::to_radians(45.0f),
                vertices,
                sf::Color::Green
            );
            tAssert(tCompare(simulation.getAllSize(), 1));
            PolygonObject* polygon = dynamic_cast<PolygonObject*>(simulation.getFromAll(0));
            tAssert(tCheck(polygon, "Object is not a PolygonObject"));
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
            simulation.createChain(
                "chain0",
                b2Vec2(1.0f, 1.0f),
                utils::to_radians(45.0f),
                vertices,
                sf::Color(255, 255, 255)
            );
            tAssert(tCompare(simulation.getAllSize(), 1));
            ChainObject* chain = dynamic_cast<ChainObject*>(simulation.getFromAll(0));
            tAssert(tCheck(chain, "Object is not a ChainObject"));
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
            joint_def.Initialize(box0->rigid_body, box1->rigid_body, b2Vec2(0.0f, 5.0f));
            simulation.createRevoluteJoint(joint_def, box0, box1);
            tAssert(tCompare(simulation.getJointsSize(), 1));
            RevoluteJoint* joint = dynamic_cast<RevoluteJoint*>(simulation.getJoint(0));
            tAssert(tCheck(joint, "Joint is not a RevoluteJoint"));
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
            simulation.createCar(
                "car0",
                b2Vec2(0.0f, 0.0f),
                lengths,
                wheels,
                sf::Color(255, 0, 0)
            );
            tAssert(tCompare(simulation.getAllSize(), 4));
            PolygonObject* car = dynamic_cast<PolygonObject*>(simulation.getFromAll(0));
            {
                tAssert(tCheck(car, "Object is not a PolygonObject"));
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
            simulation.createBox(
                "box0",
                b2Vec2(1.5f, -3.5f),
                utils::to_radians(45.0f),
                b2Vec2(1.1f, 2.0f),
                sf::Color::Green
            );
            BoxObject* boxA = dynamic_cast<BoxObject*>(simulation.getFromAll(0));
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
            simulation.createBall(
                "ball0",
                b2Vec2(1.5f, -3.5f),
                2.2f,
                sf::Color::Green,
                sf::Color::Green
            );
            BallObject* ballA = dynamic_cast<BallObject*>(simulation.getFromAll(0));
            std::string str = ballA->serialize();
            std::unique_ptr<BallObject> uptr = BallObject::deserialize(str, &simulation);
            BallObject* ballB = uptr.get();
            tCheck(*ballA == *ballB);
            objCmpCommon(test, ballA, ballB);
            tCompare(ballB->radius, ballA->radius);
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
            simulation.createPolygon(
                "polygon0",
                b2Vec2(1.5f, -3.5f),
                utils::to_radians(45.0f),
                vertices,
                sf::Color::Green
            );
            PolygonObject* polygonA = dynamic_cast<PolygonObject*>(simulation.getFromAll(0));
            std::string str = polygonA->serialize();
            std::unique_ptr<PolygonObject> uptr = PolygonObject::deserialize(str, &simulation);
            PolygonObject* polygonB = uptr.get();
            tCheck(*polygonA == *polygonB);
            objCmpCommon(test, polygonA, polygonB);
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
            simulation.createChain(
                "chain0",
                b2Vec2(1.5f, -3.5f),
                utils::to_radians(45.0f),
                vertices,
                sf::Color::Green
            );
            ChainObject* chainA = dynamic_cast<ChainObject*>(simulation.getFromAll(0));
            std::string str = chainA->serialize();
            std::unique_ptr<ChainObject> uptr = ChainObject::deserialize(str, &simulation);
            ChainObject* chainB = uptr.get();
            tCheck(*chainA == *chainB);
            objCmpCommon(test, chainA, chainB);
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
                "box0",
                b2Vec2(1.5f, -3.5f),
                utils::to_radians(45.0f),
                b2Vec2(1.5f, 2.0f),
                sf::Color::Green
            );
            BoxObject* box1 = simulation.createBox(
                "box1",
                b2Vec2(0.0f, 5.0f),
                utils::to_radians(-10.0f),
                b2Vec2(3.0f, 0.5f),
                sf::Color::Green
            );
            b2RevoluteJointDef joint_def;
            joint_def.Initialize(box0->rigid_body, box1->rigid_body, b2Vec2(0.0f, 5.0f));
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
                "car0",
                b2Vec2(0.0f, 0.0f),
                lengths,
                wheels,
                sf::Color(255, 0, 0)
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
                "box0",
                b2Vec2(1.5f, -3.5f),
                utils::to_radians(45.0f),
                b2Vec2(1.1f, 2.0f),
                sf::Color::Green
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
                "ground",
                b2Vec2(0.0f, 0.0f),
                utils::to_radians(0.0f),
                ground_vertices,
                sf::Color(255, 255, 255)
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
                "ground",
                b2Vec2(0.0f, 0.0f),
                utils::to_radians(0.0f),
                ground_vertices,
                sf::Color(255, 255, 255)
            );
            std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
            std::vector<float> wheels = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
            GameObject* car = simulationA.createCar(
                "car0",
                b2Vec2(0.0f, 6.0f),
                lengths,
                wheels,
                sf::Color(255, 0, 0)
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
            tAssert(tCompare(box1->getParent()->getId(), box0->getId()));
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
            tAssert(tCompare(box1->getParent()->getId(), box0->getId()));
            tAssert(tCompare(box2->getParent()->getId(), box1->getId()));
            tAssert(tCompare(box0->getChildren().size(), 1));
            tAssert(tCompare(box1->getChildren().size(), 1));
            tCheck(box0->getChild(0) == box1);
            tCheck(box1->getChild(0) == box2);
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

void SimulationTests::objCmpCommon(test::Test& test, const GameObject* objA, const GameObject* objB) {
    tCompare(objB->getChildren().size(), objA->getChildren().size());
    if (objA->getChildren().size() == objB->getChildren().size()) {
        for (size_t i = 0; i < objA->getChildren().size(); i++) {
            tCompare(objB->getChild(i)->getId(), objA->getChild(i)->getId());
        }
    }
    tCompare(objB->color, objA->color, &SimulationTests::colorToStr);
    tCompare(objB->getId(), objA->getId());
    tCompare(objB->getName(), objA->getName());
    tCompare(objB->parent_id, objA->parent_id);
    tCompare(objB->getTransform().q.GetAngle(), objA->getTransform().q.GetAngle());
    tCompare(objB->getTransform().p, objA->getTransform().p, &SimulationTests::b2Vec2ToStr);
    tCompare(objB->getLinearVelocity(), objA->getLinearVelocity(), &SimulationTests::b2Vec2ToStr);
    tCompare(objB->getAngularVelocity(), objA->getAngularVelocity());
    tCheck(objB->getVertices() == objA->getVertices());
}

void SimulationTests::boxCmp(test::Test& test, BoxObject* boxA, BoxObject* boxB) {
    tAssert(tCheck(boxA && boxB, "Objects have different types"));
    tCheck(*boxA == *boxB);
    objCmpCommon(test, boxA, boxB);
    tCompare(boxB->size, boxA->size, &SimulationTests::b2Vec2ToStr);
}

void SimulationTests::ballCmp(test::Test& test, BallObject* ballA, BallObject* ballB) {
    tAssert(tCheck(ballA && ballB, "Objects have different types"));
    tCheck(*ballA == *ballB);
    objCmpCommon(test, ballA, ballB);
    tCompare(ballB->radius, ballA->radius);
}

void SimulationTests::polygonCmp(test::Test& test, PolygonObject* polygonA, PolygonObject* polygonB) {
    tAssert(tCheck(polygonA && polygonB, "Objects have different types"));
    tCheck(*polygonA == *polygonB);
    objCmpCommon(test, polygonA, polygonB);
}

void SimulationTests::chainCmp(test::Test& test, ChainObject* chainA, ChainObject* chainB) {
    tAssert(tCheck(chainA && chainB, "Objects have different types"));
    tCheck(*chainA == *chainB);
    objCmpCommon(test, chainA, chainB);
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
