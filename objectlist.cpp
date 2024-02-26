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

GameObject* GameObjectList::getFromTop(size_t i) {
    return top_objects[i];
}

GameObject* GameObjectList::getFromAll(size_t i) {
    return all_objects[i];
}

GameObject* GameObjectList::getById(size_t id) {
    auto it = ids.find(ObjectId(id, nullptr));
    if (it != ids.end()) {
        return const_cast<GameObject*>((*it).ptr);
    }
}

ptrdiff_t GameObjectList::getTopIndex(GameObject* object) const {
    return top_objects.getIndex(object);
}

Joint* GameObjectList::getJoint(size_t i) {
    return joints[i];
}

const std::vector<GameObject*>& GameObjectList::getTopVector() const {
    return top_objects.getVector();
}

std::vector<GameObject*> GameObjectList::getAllVector() const {
    return all_objects.getVector();
}

ptrdiff_t GameObjectList::getMaxId() const {
    if (ids.size() > 0) {
        return (*ids.rbegin()).id;
    } else {
        return -1;
    }
}

GameObject* GameObjectList::add(std::unique_ptr<GameObject> object, bool assign_new_id) {
    try {
        GameObject* ptr = object.get();
        if (assign_new_id) {
            ptr->id = getMaxId() + 1;
        }
        if (ptr->id < 0) {
            throw std::runtime_error("Invalid object id: " + std::to_string(object->id));
        }
        GameObject* parent = nullptr;
        if (ptr->parent_id >= 0) {
            auto pid = ids.find(ObjectId(ptr->parent_id, nullptr));
            if (pid == ids.end()) {
                throw std::runtime_error(
                    "Parent not found: id: "
                    + std::to_string(ptr->id) 
                    + " parent_id: " 
                    + std::to_string(ptr->parent_id)
                );
            } else {
                parent = const_cast<GameObject*>((*pid).ptr);
            }
        }
        if (parent) {
            ptr->setParent(parent);
        } else {
            top_objects.add(ptr);
        }
        auto inserted = ids.insert(ObjectId(ptr->id, ptr));
        if (!inserted.second) {
            throw std::runtime_error("Duplicate object id: " + std::to_string(ptr->id));
        }
        all_objects.add(std::move(object));
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
    }
}

Joint* GameObjectList::addJoint(std::unique_ptr<Joint> joint) {
    Joint* ptr = joint.get();
    joint->object1->joints.add(joint.get());
    joint->object2->joints.add(joint.get());
    joints.add(std::move(joint));
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

void GameObjectList::setParent(GameObject* object1, GameObject* object2) {
    assert(object1);
    assert(all_objects.contains(object1));
    object1->setParent(object2);
    if (object2) {
        top_objects.remove(object1);
    } else {
        top_objects.add(object1);
    }
}

void GameObjectList::remove(GameObject* object, bool remove_children) {
    assert(object);
    ptrdiff_t index = all_objects.getIndex(object);
    assert(index >= 0);
    std::vector<Joint*> joints_copy = object->joints.getVector();
    for (Joint* joint : joints_copy) {
        joint->valid = false;
        removeJoint(joint);
    }
    setParent(object, nullptr);
    std::vector<GameObject*> children_copy = object->getChildren();
    if (remove_children) {
        for (size_t i = 0; i < children_copy.size(); i++) {
            GameObject* child = children_copy[i];
            remove(child, true);
        }
    } else {
        for (size_t i = 0; i < children_copy.size(); i++) {
            GameObject* child = children_copy[i];
            setParent(child, nullptr);
        }
    }
    top_objects.remove(object);
    ids.erase(ObjectId(object->id, nullptr));
    all_objects.removeByIndex(index);
}

void GameObjectList::removeJoint(Joint* joint) {
    assert(joint);
    joint->object1->joints.remove(joint);
    joint->object2->joints.remove(joint);
    joints.remove(joint);
}

void GameObjectList::clear() {
    joints.clear();
    all_objects.clear();
    top_objects.clear();
    ids = std::set<ObjectId>();
}
