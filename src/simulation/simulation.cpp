#include "simulation/simulation.h"

Simulation::Simulation() {
    reset();
}

size_t Simulation::getStep() const {
    return step;
}

void Simulation::advance(float time_step) {
    world->Step(time_step, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
    transformFromRigidbody();
    step++;
}

void Simulation::load(const std::string& filename) {
    LoggerTag tag_saveload("saveload");
    try {
        std::string str = utils::file_to_str(filename);
        deserialize(str);
        logger << "Simulation loaded from " << filename << "\n";
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": " + filename + ": " + std::string(exc.what()));
    }
}

void Simulation::save(const std::string& filename) const {
    LoggerTag tag_saveload("saveload");
    try {
        std::string str = serialize();
        utils::str_to_file(str, filename);
        logger << "Simulation saved to " << filename << "\n";
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": " + filename + ": " + std::string(exc.what()));
    }
}

void Simulation::reset() {
    clear();
    b2Vec2 gravity(0.0f, -9.8f);
    world = make_data_pointer<b2World>("Simulation World", gravity);
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
        logger << "Object: " << obj->getId() << "\n";
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
                logger << "Joint: " << joint->object1->getId() << " " << joint->object2->getId() << "\n";
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
        tr.tryEat("simulation");
        while (tr.validRange()) {
            std::string entity = tr.readString();
            if (entity == "object") {
                DataPointer<GameObject> gameobject;
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
                    DataPointer<RevoluteJoint> uptr = RevoluteJoint::deserialize(tr, this);
                    addJoint(std::move(uptr));
                } else {
                    throw std::runtime_error("Unknown joint type: " + type);
                }
            } else if (entity == "/simulation") {
                break;
            } else {
                throw std::runtime_error("Unknown entity type: " + entity);
            }
        }
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": Line " + std::to_string(tr.getLine(-1)) + ": " + exc.what());
    }
}

BoxObject* Simulation::createBox(
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
    DataPointer<BoxObject> uptr = make_data_pointer<BoxObject>(
        "BoxObject " + name, this, def, size, color
    );
    BoxObject* ptr = uptr.get();
    ptr->setName(name);
    add(std::move(uptr), true);
    return ptr;
}

BallObject* Simulation::createBall(
    const std::string& name,
    const b2Vec2& pos,
    float radius,
    const sf::Color& color,
    const sf::Color& notch_color
) {
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position = pos;
    DataPointer<BallObject> uptr = make_data_pointer<BallObject>(
        "BallObject " + name, this, def, radius, color, notch_color
    );
    BallObject* ptr = uptr.get();
    ptr->setName(name);
    add(std::move(uptr), true);
    return ptr;
}

PolygonObject* Simulation::createPolygon(
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
    DataPointer<PolygonObject> uptr = make_data_pointer<PolygonObject>(
        "PolygonObject " + name, this, def, vertices, color
    );
    PolygonObject* ptr = uptr.get();
    ptr->setName(name);
    add(std::move(uptr), true);
    return ptr;
}

PolygonObject* Simulation::createRegularPolygon(
    const std::string& name,
    const b2Vec2& pos,
    float angle,
    size_t vertex_count,
    float radius,
    const sf::Color& color
) {
    std::vector<b2Vec2> vertices;
    for (size_t i = 0; i < vertex_count; i++) {
        b2Vec2 pos = utils::get_circle_vertex<b2Vec2>(i, vertex_count, radius);
        vertices.push_back(pos);
    }
    PolygonObject* polygon = createPolygon(name, pos, angle, vertices, color);
    return polygon;
}

PolygonObject* Simulation::createCar(
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
    PolygonObject* car = createPolygon(name, pos, 0.0f, vertices, color);
    size_t wheel_count = 0;
    for (size_t i = 0; i < wheels.size(); i++) {
        if (wheels[i] == 0.0f) {
            continue;
        }
        b2Vec2 wheel_pos = utils::get_pos<b2Vec2>(lengths, i);
        b2Vec2 anchor_pos = wheel_pos;
        b2Vec2 anchor_pos_world = car->getRigidBody()->GetPosition() + anchor_pos;
        float radius = wheels[i];
        std::string wheel_name = car->getName() + " wheel" + std::to_string(wheel_count);
        BallObject* wheel = createBall(
            wheel_name, anchor_pos_world, radius, sf::Color(255, 255, 0), sf::Color(64, 64, 0)
        );
        wheel->setDensity(1.0f, false);
        wheel->setFriction(0.3f, false);
        wheel->setRestitution(0.5f, false);
        wheel->setParent(car);
        b2RevoluteJointDef wheel_joint_def;
        wheel_joint_def.Initialize(car->getRigidBody(), wheel->getRigidBody(), anchor_pos_world);
        wheel_joint_def.maxMotorTorque = 30.0f;
        wheel_joint_def.motorSpeed = -10.0f;
        wheel_joint_def.enableMotor = true;
        RevoluteJoint* wheel_joint = createRevoluteJoint(wheel_joint_def, car, wheel);
        wheel_count++;
    }

    return car;
}

ChainObject* Simulation::createChain(
    const std::string& name,
    const b2Vec2& pos,
    float angle,
    const std::vector<b2Vec2>& vertices,
    const sf::Color& color
) {
    b2BodyDef def;
    def.position = pos;
    def.angle = angle;
    DataPointer<ChainObject> uptr = make_data_pointer<ChainObject>(
        "ChainObject " + name, this, def, vertices, color
    );
    ChainObject* ptr = uptr.get();
    ptr->setName(name);
    add(std::move(uptr), true);
    return ptr;
}

RevoluteJoint* Simulation::createRevoluteJoint(
    const b2RevoluteJointDef& def,
    GameObject* obj1,
    GameObject* obj2
) {
    DataPointer<RevoluteJoint> uptr = make_data_pointer<RevoluteJoint>(
        "RevoluteJoint A: " + obj1->getName() + " B: " + obj2->getName(), def, world.get(), obj1, obj2
    );
    RevoluteJoint* ptr = uptr.get();
    addJoint(std::move(uptr));
    return ptr;
}

RevoluteJoint* Simulation::createRevoluteJoint(
    GameObject* obj1,
    GameObject* obj2,
    const b2Vec2& pos
) {
    b2RevoluteJointDef joint_def;
    joint_def.Initialize(obj1->getRigidBody(), obj2->getRigidBody(), pos);
    RevoluteJoint* joint = createRevoluteJoint(joint_def, obj1, obj2);
    return joint;
}

bool Simulation::operator==(const Simulation& other) const {
    if (getAllSize() != other.getAllSize()) {
        return false;
    }
    for (size_t i = 0; i < getAllSize(); i++) {
        if (*getFromAll(i) != *other.getFromAll(i)) {
            return false;
        }
    }
    return true;
}
