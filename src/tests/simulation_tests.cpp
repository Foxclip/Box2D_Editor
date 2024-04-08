#include "tests/simulation_tests.h"

SimulationTests::SimulationTests() : TestModule("Simulation") { }

void SimulationTests::createTestLists() {
    test::TestList* list = createTestList("Simulation");
    list->OnBeforeRunTest = []() { logger.manualDeactivate(); };
    list->OnAfterRunTest = []() { logger.manualActivate(); };
    std::function<std::string(const sf::Color&)> color_to_str =
        [](const sf::Color& color) {
        return "(" + utils::color_to_str(color) + ")";
    };
    std::function<void(test::Test&, GameObject*, GameObject*)> obj_cmp_common = 
        [&, color_to_str](test::Test& test, GameObject* objA, GameObject* objB) {
        tCompare(objB->getChildren().size(), objA->getChildren().size());
        if (objA->getChildren().size() == objB->getChildren().size()) {
            for (size_t i = 0; i < objA->getChildren().size(); i++) {
                tCompare(objB->getChild(i)->getId(), objA->getChild(i)->getId());
            }
        }
        tCompare(objB->color, objA->color, color_to_str);
        tCompare(objB->getId(), objA->getId());
        tCompare(objB->getName(), objA->getName());
        tCompare(objB->parent_id, objA->parent_id);
        tCompare(objB->getTransform().q.GetAngle(), objA->getTransform().q.GetAngle());
        tCompare(objB->getTransform().p.x, objA->getTransform().p.x);
        tCompare(objB->getTransform().p.y, objA->getTransform().p.y);
        tCheck(objB->getVertices() == objA->getVertices());
    };
    std::function<void(test::Test&, Joint*, Joint*)> joint_cmp_common =
        [&](test::Test& test, Joint* jointA, Joint* jointB) {
        tCompare(jointB->object1->getId(), jointA->object1->getId());
        tCompare(jointB->object2->getId(), jointA->object2->getId());
        tCompare(jointB->getAnchorA().x, jointA->getAnchorA().x);
        tCompare(jointB->getAnchorA().y, jointA->getAnchorA().y);
        tCompare(jointB->getAnchorB().x, jointA->getAnchorB().x);
        tCompare(jointB->getAnchorB().y, jointA->getAnchorB().y);
        tCompare(jointB->getCollideConnected(), jointA->getCollideConnected());
    };

    test::Test* basic_test = list->addTest("basic", [&](test::Test& test) {
        Simulation simulation;
        tAssertCompare(simulation.getAllSize(), 0);
    });
    test::Test* box_test = list->addTest("box", { basic_test }, [&](test::Test& test) {
        Simulation simulation;
        simulation.create_box(
            "box0",
            b2Vec2(1.0f, 1.0f),
            utils::to_radians(45.0f),
            b2Vec2(1.0f, 1.0f),
            sf::Color::Green
        );
        tAssertCompare(simulation.getAllSize(), 1);
        BoxObject* box = dynamic_cast<BoxObject*>(simulation.getFromAll(0));
        tAssert(box, "Object is not a BoxObject");
        tCompare(box->getName(), "box0");
        tCompare(box->getId(), 0);
        tApproxCompare(box->getGlobalPosition().x, 1.0f);
        tApproxCompare(box->getGlobalPosition().y, 1.0f);
        tApproxCompare(box->getGlobalRotation(), utils::to_radians(45.0f));
    });
    test::Test* ball_test = list->addTest("ball", { basic_test }, [&](test::Test& test) {
        Simulation simulation;
        simulation.create_ball(
            "ball0",
            b2Vec2(1.0f, 1.0f),
            1.0f,
            sf::Color::Green,
            sf::Color::Green
        );
        tAssertCompare(simulation.getAllSize(), 1);
        BallObject* ball = dynamic_cast<BallObject*>(simulation.getFromAll(0));
        tAssert(ball, "Object is not a BallObject");
        tCompare(ball->getName(), "ball0");
        tCompare(ball->getId(), 0);
        tApproxCompare(ball->getGlobalPosition().x, 1.0f);
        tApproxCompare(ball->getGlobalPosition().y, 1.0f);
        tApproxCompare(ball->getGlobalRotation(), utils::to_radians(0.0f));
    });
    test::Test* polygon_test = list->addTest("polygon", { basic_test }, [&](test::Test& test) {
        Simulation simulation;
        std::vector<b2Vec2> vertices;
        for (size_t i = 0; i < 6; i++) {
            b2Vec2 vertex = utils::get_circle_vertex<b2Vec2>(i, 6, 1.0f);
            vertices.push_back(vertex);
        }
        simulation.create_polygon(
            "polygon0",
            b2Vec2(1.0f, 1.0f),
            utils::to_radians(45.0f),
            vertices,
            sf::Color::Green
        );
        tAssertCompare(simulation.getAllSize(), 1);
        PolygonObject* polygon = dynamic_cast<PolygonObject*>(simulation.getFromAll(0));
        tAssert(polygon, "Object is not a PolygonObject");
        tCompare(polygon->getName(), "polygon0");
        tCompare(polygon->getId(), 0);
        tApproxCompare(polygon->getGlobalPosition().x, 1.0f);
        tApproxCompare(polygon->getGlobalPosition().y, 1.0f);
        tApproxCompare(polygon->getGlobalRotation(), utils::to_radians(45.0f));
    });
    test::Test* chain_test = list->addTest("chain", { basic_test }, [&](test::Test& test) {
        Simulation simulation;
        std::vector<b2Vec2> vertices = {
            b2Vec2(25.0f, 8.0f),
            b2Vec2(15.0f, 2.0f),
            b2Vec2(5.0f, 0.0f),
            b2Vec2(-5.0f, 0.0f),
            b2Vec2(-15.0f, 2.0f),
            b2Vec2(-25.0f, 8.0f),
        };
        simulation.create_chain(
            "chain0",
            b2Vec2(1.0f, 1.0f),
            utils::to_radians(45.0f),
            vertices,
            sf::Color(255, 255, 255)
        );
        tAssertCompare(simulation.getAllSize(), 1);
        ChainObject* chain = dynamic_cast<ChainObject*>(simulation.getFromAll(0));
        tAssert(chain, "Object is not a ChainObject");
        tCompare(chain->getName(), "chain0");
        tCompare(chain->getId(), 0);
        tApproxCompare(chain->getGlobalPosition().x, 1.0f);
        tApproxCompare(chain->getGlobalPosition().y, 1.0f);
        tApproxCompare(chain->getGlobalRotation(), utils::to_radians(45.0f));
    });
    test::Test* revolute_joint_test = list->addTest("revolute_joint", { box_test }, [&](test::Test& test) {
        Simulation simulation;
        BoxObject* box0 = simulation.create_box(
            "box0",
            b2Vec2(0.0f, 0.0f),
            utils::to_radians(0.0f),
            b2Vec2(1.0f, 1.0f),
            sf::Color::Green
        );
        BoxObject* box1 = simulation.create_box(
            "box1",
            b2Vec2(0.0f, 5.0f),
            utils::to_radians(0.0f),
            b2Vec2(1.0f, 1.0f),
            sf::Color::Green
        );
        b2RevoluteJointDef joint_def;
        joint_def.Initialize(box0->rigid_body, box1->rigid_body, b2Vec2(0.0f, 5.0f));
        simulation.createRevoluteJoint(joint_def, box0, box1);
        tAssertCompare(simulation.getJointsSize(), 1);
        RevoluteJoint* joint = dynamic_cast<RevoluteJoint*>(simulation.getJoint(0));
        tAssert(joint, "Joint is not a RevoluteJoint");
        tAssertCompare(joint->getAnchorA().x, 0.0f);
        tAssertCompare(joint->getAnchorA().y, 5.0f);
    });
    test::Test* car_test = list->addTest("car", { ball_test, polygon_test, revolute_joint_test }, [&](test::Test& test) {
        Simulation simulation;
        std::vector<float> lengths = { 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f };
        std::vector<float> wheels = { 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
        simulation.create_car(
            "car0",
            b2Vec2(0.0f, 0.0f),
            lengths,
            wheels,
            sf::Color(255, 0, 0)
        );
        tAssertCompare(simulation.getAllSize(), 4);
        PolygonObject* car = dynamic_cast<PolygonObject*>(simulation.getFromAll(0));
        {
            tAssert(car, "Object is not a PolygonObject");
            tCompare(car->getName(), "car0");
            tCompare(car->getId(), 0);
            tApproxCompare(car->getGlobalPosition().x, 0.0f);
            tApproxCompare(car->getGlobalPosition().y, 0.0f);
            tApproxCompare(car->getGlobalRotation(), utils::to_radians(0.0f));
        }
        {
            BallObject* wheel0 = dynamic_cast<BallObject*>(simulation.getFromAll(1));
            tAssert(wheel0, "Object is not a BallObject");
            tCompare(wheel0->getName(), "car0 wheel0");
            tCompare(wheel0->getId(), 1);
            tCheck(wheel0->getParent() == car);
            tApproxCompare(wheel0->getGlobalPosition().x, 5.0f);
            tApproxCompare(wheel0->getGlobalPosition().y, 0.0f);
        }
        {
            BallObject* wheel1 = dynamic_cast<BallObject*>(simulation.getFromAll(2));
            tAssert(wheel1, "Object is not a BallObject");
            tCompare(wheel1->getName(), "car0 wheel1");
            tCompare(wheel1->getId(), 2);
            tCheck(wheel1->getParent() == car);
            tApproxCompare(wheel1->getGlobalPosition().x, -2.50000024f);
            tApproxCompare(wheel1->getGlobalPosition().y, 4.33012676f);
        }
        {
            BallObject* wheel2 = dynamic_cast<BallObject*>(simulation.getFromAll(3));
            tAssert(wheel2, "Object is not a BallObject");
            tCompare(wheel2->getName(), "car0 wheel2");
            tCompare(wheel2->getId(), 3);
            tCheck(wheel2->getParent() == car);
            tApproxCompare(wheel2->getGlobalPosition().x, -2.49999952f);
            tApproxCompare(wheel2->getGlobalPosition().y, -4.33012724f);
        }
    });
    test::Test* box_serialize_test = list->addTest("box_serialize", { box_test }, [=, this](test::Test& test) {
        Simulation simulation;
        simulation.create_box(
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
        tCheck(*boxA == *boxB);
        obj_cmp_common(test, boxA, boxB);
        tCompare(boxB->size.x, boxA->size.x);
        tCompare(boxB->size.y, boxA->size.y);
    });
    test::Test* ball_serialize_test = list->addTest("ball_serialize", { ball_test }, [=, this](test::Test& test) {
        Simulation simulation;
        simulation.create_ball(
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
        obj_cmp_common(test, ballA, ballB);
        tCompare(ballB->radius, ballA->radius);
    });
    test::Test* polygon_serialize_test = list->addTest("polygon_serialize", { polygon_test }, [=, this](test::Test& test) {
        Simulation simulation;
        std::vector<b2Vec2> vertices;
        for (size_t i = 0; i < 6; i++) {
            b2Vec2 vertex = utils::get_circle_vertex<b2Vec2>(i, 6, 1.0f);
            vertices.push_back(vertex);
        }
        simulation.create_polygon(
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
        obj_cmp_common(test, polygonA, polygonB);
    });
    test::Test* chain_serialize_test = list->addTest("chain_serialize", { chain_test }, [=, this](test::Test& test) {
        Simulation simulation;
        std::vector<b2Vec2> vertices = {
            b2Vec2(25.0f, 8.0f),
            b2Vec2(15.0f, 2.0f),
            b2Vec2(5.0f, 0.0f),
            b2Vec2(-5.0f, 0.0f),
            b2Vec2(-15.0f, 2.0f),
            b2Vec2(-25.0f, 8.0f),
        };
        simulation.create_chain(
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
        obj_cmp_common(test, chainA, chainB);
    });
    test::Test* revolute_joint_serialize_test = list->addTest("revolute_joint_serialize", { revolute_joint_test }, [=, this](test::Test& test) {
        Simulation simulation;
        BoxObject* box0 = simulation.create_box(
            "box0",
            b2Vec2(1.5f, -3.5f),
            utils::to_radians(45.0f),
            b2Vec2(1.5f, 2.0f),
            sf::Color::Green
        );
        BoxObject* box1 = simulation.create_box(
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
        tCheck(*jointA == *jointB);
        joint_cmp_common(test, jointA, jointB);
        tCompare(jointB->getLowerLimit(), jointA->getLowerLimit());
        tCompare(jointB->getMaxMotorTorque(), jointA->getMaxMotorTorque());
        tCompare(jointB->getMotorSpeed(), jointA->getMotorSpeed());
        tCompare(jointB->getReferenceAngle(), jointA->getReferenceAngle());
        tCompare(jointB->getUpperLimit(), jointA->getUpperLimit());
        tCompare(jointB->isLimitEnabled(), jointA->isLimitEnabled());
        tCompare(jointB->isMotorEnabled(), jointA->isMotorEnabled());
    });
}
