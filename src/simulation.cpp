#include "simulation.h"

Simulation::Simulation() {
    reset();
}

void Simulation::advance(float time_step) {
    world->Step(time_step, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
    transformFromRigidbody();
}

void Simulation::load(const std::string& filename) {
	load_from_file(filename);
}

void Simulation::reset() {
    clear(); 
    b2Vec2 gravity(0.0f, -9.8f);
    world = std::make_unique<b2World>(gravity);
}

void Simulation::load_from_file(const std::string& filename) {
    LoggerTag tag_saveload("saveload");
    try {
        std::string str = utils::file_to_str(filename);
        deserialize(str);
        logger << "Simulation loaded from " << filename << "\n";
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": " + filename + ": " + std::string(exc.what()));
    }
}

std::string Simulation::serialize() const {
    TokenWriter tw;
    std::string str = serialize(tw);
    return str;
}

std::string Simulation::serialize(TokenWriter& tw) const {
    LoggerTag tag_serialize("serialize");
    logger << __FUNCTION__"\n";
    LoggerIndent serialize_indent;
    size_t index = 0;
    std::function<void(GameObject*)> serialize_obj = [&](GameObject* obj) {
        logger << "Object: " << obj->id << "\n";
        if (index > 0) {
            tw << "\n\n";
        }
        obj->serialize(tw);
        index++;
    };
    std::function<void(GameObject*)> serialize_tree = [&](GameObject* obj) {
        serialize_obj(obj);
        LoggerIndent children_indent;
        for (size_t i = 0; i < obj->getChildren().size(); i++) {
            serialize_tree(obj->getChild(i));
        }
    };
    tw << "simulation" << "\n\n";
    {
        TokenWriterIndent simulation_indent(tw);
        {
            logger << "Objects\n";
            LoggerIndent objects_indent;
            if (getAllSize() == 0) {
                logger << "<empty>\n";
            }
            for (size_t i = 0; i < getTopSize(); i++) {
                GameObject* gameobject = getFromTop(i);
                serialize_tree(gameobject);
            }
        }
        {
            logger << "Joints\n";
            LoggerIndent joints_indent;
            if (getJointsSize() == 0) {
                logger << "<empty>\n";
            } else {
                tw << "\n\n";
            }
            for (size_t i = 0; i < getJointsSize(); i++) {
                if (i > 0) {
                    tw << "\n\n";
                }
                Joint* joint = getJoint(i);
                logger << "Joint: " << joint->object1->id << " " << joint->object2->id << "\n";
                getJoint(i)->serialize(tw);
            }
        }
    }
    tw << "\n\n";
    tw << "/simulation";
    return tw.toStr();
}

void Simulation::deserialize(const std::string& str) {
    TokenReader tr(str);
    deserialize(tr);
}

void Simulation::deserialize(TokenReader& tr) {
    reset();
    try {
        while (tr.validRange()) {
            std::string entity = tr.readString();
            if (entity == "object") {
                std::unique_ptr<GameObject> gameobject;
                std::string type = tr.readString();
                if (type == "box") {
                    gameobject = BoxObject::deserialize(tr, this);
                } else if (type == "ball") {
                    gameobject = BallObject::deserialize(tr, this);
                } else if (type == "polygon") {
                    gameobject = PolygonObject::deserialize(tr, this);
                } else if (type == "chain") {
                    gameobject = ChainObject::deserialize(tr, this);
                } else {
                    throw std::runtime_error("Unknown object type: " + type);
                }
                add(std::move(gameobject), false);
            } else if (entity == "joint") {
                std::string type = tr.readString();
                if (type == "revolute") {
                    ptrdiff_t body_a_id, body_b_id;
                    b2RevoluteJointDef def = RevoluteJoint::deserialize(tr, body_a_id, body_b_id);
                    GameObject* object1 = getById(body_a_id);
                    GameObject* object2 = getById(body_b_id);
                    std::unique_ptr<RevoluteJoint> uptr = std::make_unique<RevoluteJoint>(def, world.get(), object1, object2);
                    addJoint(std::move(uptr));
                } else {
                    throw std::runtime_error("Unknown joint type: " + type);
                }
            } else {
                throw std::runtime_error("Unknown entity type: " + entity);
            }
        }
    } catch (std::exception exc) {
        throw std::runtime_error("Line " + std::to_string(tr.getLine(-1)) + ": " + exc.what());
    }
}

b2World* Simulation::getWorld() const {
    return world.get();
}

BoxObject* Simulation::create_box(
    const std::string& name,
    const b2Vec2& pos,
    float angle,
    const b2Vec2& size,
    const sf::Color& color
) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    def.angle = angle;
    std::unique_ptr<BoxObject> uptr = std::make_unique<BoxObject>(
        this, def, size, color
    );
    BoxObject* ptr = uptr.get();
    ptr->setName(name);
    add(std::move(uptr), true);
    return ptr;
}

BallObject* Simulation::create_ball(
    const std::string& name,
    const b2Vec2& pos,
    float radius,
    const sf::Color& color,
    const sf::Color& notch_color
) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    std::unique_ptr<BallObject> uptr = std::make_unique<BallObject>(
        this, def, radius, color, notch_color
    );
    BallObject* ptr = uptr.get();
    ptr->setName(name);
    add(std::move(uptr), true);
    return ptr;
}

PolygonObject* Simulation::create_polygon(
    const std::string& name,
    const b2Vec2& pos,
    float angle,
    const std::vector<b2Vec2>& vertices,
    const sf::Color& color
) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    def.angle = angle;
    std::unique_ptr<PolygonObject> uptr = std::make_unique<PolygonObject>(
        this, def, vertices, color
    );
    PolygonObject* ptr = uptr.get();
    ptr->setName(name);
    add(std::move(uptr), true);
    return ptr;
}

PolygonObject* Simulation::create_car(
    const std::string& name,
    const b2Vec2& pos,
    const std::vector<float>& lengths,
    const std::vector<float>& wheels,
    const sf::Color& color
) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    std::vector<b2Vec2> vertices;
    for (size_t i = 0; i < lengths.size(); i++) {
        b2Vec2 pos = utils::get_pos<b2Vec2>(lengths, i);
        vertices.push_back(pos);
    }
    std::unique_ptr<PolygonObject> uptr = std::make_unique<PolygonObject>(
        this, def, vertices, color
    );
    PolygonObject* car_ptr = uptr.get();
    car_ptr->setName(name);
    add(std::move(uptr), true);
    size_t wheel_count = 0;

    for (size_t i = 0; i < wheels.size(); i++) {
        if (wheels[i] == 0.0f) {
            continue;
        }
        b2Vec2 wheel_pos = utils::get_pos<b2Vec2>(lengths, i);
        b2Vec2 anchor_pos = wheel_pos;
        b2Vec2 anchor_pos_world = car_ptr->rigid_body->GetPosition() + anchor_pos;
        float radius = wheels[i];
        b2BodyDef wheel_body_def;
        {
            wheel_body_def.type = b2_dynamicBody;
            wheel_body_def.position = anchor_pos_world;
        }
        std::unique_ptr<BallObject> wheel = std::make_unique<BallObject>(
            this, wheel_body_def, radius, sf::Color(255, 255, 0), sf::Color(64, 64, 0)
        );
        BallObject* wheel_ptr = wheel.get();
        {
            wheel_ptr->setDensity(1.0f, false);
            wheel_ptr->setFriction(0.3f, false);
            wheel_ptr->setRestitution(0.5f, false);
            std::string wheel_name = car_ptr->getName() + " wheel" + std::to_string(wheel_count);
            wheel_ptr->setName(wheel_name);
            wheel_ptr->setParent(car_ptr);
            add(std::move(wheel), true);
        }
        b2RevoluteJointDef wheel_joint_def;
        {
            wheel_joint_def.Initialize(car_ptr->rigid_body, wheel_ptr->rigid_body, anchor_pos_world);
            wheel_joint_def.maxMotorTorque = 30.0f;
            wheel_joint_def.motorSpeed = -10.0f;
            wheel_joint_def.enableMotor = true;
            std::unique_ptr<RevoluteJoint> joint = std::make_unique<RevoluteJoint>(
                wheel_joint_def, world.get(), car_ptr, wheel_ptr
            );
            addJoint(std::move(joint));
        }
        wheel_count++;
    }

    return car_ptr;
}

ChainObject* Simulation::create_chain(
    const std::string& name,
    const b2Vec2& pos,
    float angle,
    const std::vector<b2Vec2>& vertices,
    const sf::Color& color
) {
    b2BodyDef def;
    def.position = pos;
    def.angle = angle;
    std::unique_ptr<ChainObject> uptr = std::make_unique<ChainObject>(
        this, def, vertices, color
    );
    ChainObject* ptr = uptr.get();
    ptr->setName(name);
    add(std::move(uptr), true);
    return ptr;
}

SimulationTests::SimulationTests() : TestModule("Simulation") { }

void SimulationTests::createTestLists() {
    test::TestList* list = createTestList("Simulation");
    test::Test* basic_test = list->addTest("basic", [&](test::Test& test) {
        Simulation simulation;
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
        tAssert(simulation.getAllSize() > 0);
        BoxObject* box = dynamic_cast<BoxObject*>(simulation.getFromAll(0));
        tAssert(box, "Object is not a BoxObject");
        tCompare(box->getName(), std::string("box0"));
        tCheck(box->getGlobalPosition() == b2Vec2(1.0f, 1.0f));
        tApproxCompare(box->getGlobalRotation(), utils::to_radians(45.0f));
    });
}
