#include "objectlist.h"

ObjectId::ObjectId(size_t id, const GameObject* ptr) {
    this->id = id;
    this->ptr = ptr;
}

bool ObjectId::operator<(const ObjectId& other) const {
    return id < other.id;
}

size_t GameObjectList::getTopSize() const {
    return top_objects.size();
}

size_t GameObjectList::getAllSize() const {
    return all_objects.size();
}

size_t GameObjectList::getJointsSize() const {
    return joints.size();
}

GameObject* GameObjectList::getFromTop(size_t i) const {
    return top_objects[i];
}

GameObject* GameObjectList::getFromAll(size_t i) const {
    return all_objects[i];
}

GameObject* GameObjectList::getById(size_t id) const {
    auto it = ids.find(ObjectId(id, nullptr));
    if (it != ids.end()) {
        return (GameObject*)((*it).ptr);
    }
}

Joint* GameObjectList::getJoint(size_t i) const {
    return joints[i];
}

const std::vector<GameObject*>& GameObjectList::getTopVector() const {
    return top_objects;
}

const std::vector<GameObject*>& GameObjectList::getAllVector() const {
    return all_objects;
}

ptrdiff_t GameObjectList::getMaxId() const {
    if (ids.size() > 0) {
        return (*ids.rbegin()).id;
    } else {
        return -1;
    }
}

GameObject* GameObjectList::add(std::unique_ptr<GameObject> gameobject, bool assign_new_id) {
    GameObject* ptr = gameobject.get();
    addToAll(ptr, assign_new_id);
    if (ptr->parent_id < 0) {
        top_objects.push_back(ptr);
        uptrs.push_back(std::move(gameobject));
    } else {
        GameObject* parent = getById(ptr->parent_id);
        parent->addChild(std::move(gameobject));
    }
    return ptr;
}

Joint* GameObjectList::addJoint(std::unique_ptr<Joint> joint) {
    Joint* ptr = joint.get();
    joint->object1->joints.insert(joint.get());
    joint->object2->joints.insert(joint.get());
    joints.push_back(ptr);
    joint_uptrs.push_back(std::move(joint));
    return ptr;
}

GameObject* GameObjectList::duplicate(const GameObject* object) {
    TokenWriter tw;
    std::string str = object->serialize(tw).toStr();
    TokenReader tr(str);
    std::unique_ptr<GameObject> new_object;
    if (dynamic_cast<const BoxObject*>(object)) {
        new_object = BoxObject::deserialize(tr, world);
    } else if (dynamic_cast<const BallObject*>(object)) {
        new_object = BallObject::deserialize(tr, world);
    } else if (dynamic_cast<const CarObject*>(object)) {
        new_object = CarObject::deserialize(tr, world);
    } else if (dynamic_cast<const ChainObject*>(object)) {
        new_object = ChainObject::deserialize(tr, world);
    } else {
        assert(false, "Unknown object type");
    }
    GameObject* ptr = new_object.get();
    ptr->orig_id = object->id;
    add(std::move(new_object), true);
    return ptr;
}

Joint* GameObjectList::duplicateJoint(const Joint* joint, GameObject* new_a, GameObject* new_b) {
    TokenWriter tw;
    std::string str = joint->serialize(tw).toStr();
    TokenReader tr(str);
    std::unique_ptr<Joint> new_joint;
    ptrdiff_t body_a, body_b;
    if (dynamic_cast<const RevoluteJoint*>(joint)) {
        b2RevoluteJointDef def = RevoluteJoint::deserialize(tr, body_a, body_b);
        new_joint = std::make_unique<RevoluteJoint>(def, world, new_a, new_b);
    } else {
        assert(false, "Unknown joint type");
    }
    Joint* ptr = new_joint.get();
    addJoint(std::move(new_joint));
    return ptr;
}

bool GameObjectList::remove(GameObject* object) {
    bool result = removeFromAll(object);
    if (result) {
        for (size_t i = 0; i < uptrs.size(); i++) {
            if (uptrs[i].get() == object) {
                top_objects.erase(top_objects.begin() + i);
                uptrs.erase(uptrs.begin() + i);
                break;
            }
        }
    }
    return result;
}

bool GameObjectList::removeJoint(Joint* joint) {
    joint->object1->joints.erase(joint);
    joint->object2->joints.erase(joint);
    for (size_t i = 0; i < joints.size(); i++) {
        if (joints[i] == joint) {
            joints.erase(joints.begin() + i);
            joint_uptrs.erase(joint_uptrs.begin() + i);
            return true;
        }
    }
    return false;
}

void GameObjectList::clear() {
    all_objects = std::vector<GameObject*>();
    top_objects = std::vector<GameObject*>();
    joints = std::vector<Joint*>();
    ids = std::set<ObjectId>();
    joint_uptrs = std::vector<std::unique_ptr<Joint>>();
    uptrs = std::vector<std::unique_ptr<GameObject>>();
}

void GameObjectList::addToAll(GameObject* object, bool assign_new_id) {
    try {
        if (assign_new_id) {
            object->id = getMaxId() + 1;
        }
        if (object->id < 0) {
            throw std::runtime_error("Invalid object id: " + std::to_string(object->id));
        }
        all_objects.push_back(object);
        auto inserted = ids.insert(ObjectId(object->id, object));
        if (!inserted.second) {
            throw std::runtime_error("Duplicate object id: " + std::to_string(object->id));
        }
        for (size_t i = 0; i < object->getChildren().size(); i++) {
            addToAll(object->getChildren()[i].get(), assign_new_id);
        }
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
    }
}

bool GameObjectList::removeFromAll(GameObject* object) {
    bool result = false;
    for (size_t i = 0; i < all_objects.size(); i++) {
        if (all_objects[i] == object) {
            std::vector<Joint*> joints_copy(object->joints.begin(), object->joints.end());
            for (Joint* joint : joints_copy) {
                joint->valid = false;
                removeJoint(joint);
            }
            all_objects.erase(all_objects.begin() + i);
            ids.erase(ObjectId(object->id, nullptr));
            GameObject* parent = object->getParent();
            if (parent) {
                parent->removeChild(object);
            }
            result = true;
            break;
        }
    }
    if (result) {
        for (size_t i = 0; i < object->getChildren().size(); i++) {
            removeFromAll(object->getChildren()[i].get());
            object->removeChild(i);
        }
    }
    return result;
}
