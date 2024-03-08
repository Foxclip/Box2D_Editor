#include "objectlist.h"

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
    return ids.find(id);
}

GameObject* GameObjectList::getByName(const std::string& name) const {
    return names.find(name);
}

ptrdiff_t GameObjectList::getTopIndex(GameObject* object) const {
    return top_objects.getIndex(object);
}

Joint* GameObjectList::getJoint(size_t i) const {
    return joints[i];
}

const CompoundVector<GameObject*>& GameObjectList::getTopVector() const {
    return top_objects;
}

const CompoundVector<GameObject*>& GameObjectList::getAllVector() const {
    return all_objects.getCompVector();
}

ptrdiff_t GameObjectList::getMaxId() const {
    if (ids.size() > 0) {
        return ids.max();
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
        bool inserted = ids.add(ptr->id, ptr);
        if (!inserted) {
            throw std::runtime_error("Duplicate object id: " + std::to_string(ptr->id));
        }
        if (ptr->parent_id >= 0) {
            parent = ids.find(ptr->parent_id);
            if (!parent) {
                throw std::runtime_error(
                    "Parent not found: id: "
                    + std::to_string(ptr->id) 
                    + " parent_id: " 
                    + std::to_string(ptr->parent_id)
                );
            }
            ptr->setParent(parent);
        } else {
            top_objects.add(ptr);
        }
        names.add(ptr->name, ptr);
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

GameObject* GameObjectList::duplicate(const GameObject* object, bool with_children) {
    if (with_children) {
        CompoundVector<GameObject*> objects = { const_cast<GameObject*>(object) };
        CompoundVector<GameObject*> all_children = object->getAllChildren();
        objects.insert(objects.end(), all_children.begin(), all_children.end());
        CompoundVector<GameObject*> new_objects = duplicate(objects);
        GameObject* new_object = getById(object->new_id);
        return new_object;
    } else {
        GameObject* new_object = duplicateObject(object);
        return new_object;
    }
}

CompoundVector<GameObject*> GameObjectList::duplicate(const CompoundVector<GameObject*>& old_objects) {
    CompoundVector<GameObject*> new_objects;
    std::set<Joint*> checked_joints;
    // copy objects
    for (GameObject* obj : old_objects) {
        GameObject* copy = duplicateObject(obj);
        obj->new_id = copy->id;
        new_objects.add(copy);
    }
    // set parents
    for (GameObject* old_obj : old_objects) {
        if (old_obj->getParent() && old_objects.contains(old_obj->getParent())) {
            GameObject* new_obj = getById(old_obj->new_id);
            size_t new_parent_id = old_obj->getParent()->new_id;
            GameObject* new_parent = getById(new_parent_id);
            setParent(new_obj, new_parent);
        }
    }
    // copy joints
    for (GameObject* obj : old_objects) {
        for (Joint* joint : obj->joints) {
            if (checked_joints.contains(joint)) {
                continue;
            }
            auto it1 = old_objects.find(joint->object1);
            auto it2 = old_objects.find(joint->object2);
            if (it1 != old_objects.getSet().end() && it2 != old_objects.getSet().end()) {
                GameObject* new_object_1 = getById((*it1)->new_id);
                GameObject* new_object_2 = getById((*it2)->new_id);
                Joint* new_joint = duplicateJoint(joint, new_object_1, new_object_2);
            }
            checked_joints.insert(joint);
        }
    }
    return new_objects;
}

GameObject* GameObjectList::duplicateObject(const GameObject* object) {
    TokenWriter tw;
    std::string str = object->serialize(tw).toStr();
    TokenReader tr(str);
    std::unique_ptr<GameObject> new_object;
    if (dynamic_cast<const BoxObject*>(object)) {
        new_object = BoxObject::deserialize(tr, this);
    } else if (dynamic_cast<const BallObject*>(object)) {
        new_object = BallObject::deserialize(tr, this);
    } else if (dynamic_cast<const PolygonObject*>(object)) {
        new_object = PolygonObject::deserialize(tr, this);
    } else if (dynamic_cast<const ChainObject*>(object)) {
        new_object = ChainObject::deserialize(tr, this);
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

void GameObjectList::setParent(GameObject* child, GameObject* new_parent) {
    try {
        assert(child);
        if (new_parent == child) {
            throw std::runtime_error("Cannot parent object to itself: id " + std::to_string(child->id));
        }
        assert(all_objects.contains(child));
        CompoundVector<GameObject*> parent_chain = new_parent->getParentChain();
        if (parent_chain.contains(child)) {
            std::string chain_str;
            chain_str += std::to_string(child->id);
            chain_str += " -> " + std::to_string(new_parent->id);
            for (size_t i = 0; i < parent_chain.size(); i++) {
                chain_str += " -> " + std::to_string(parent_chain[i]->id);
                if (parent_chain[i] == child) {
                    break;
                }
            }
            throw std::runtime_error("Loop in parent hierarchy: " + chain_str);
        }
        if (new_parent) {
            assert(all_objects.contains(new_parent));
        }
        child->setParent(new_parent);
        if (new_parent) {
            top_objects.remove(child);
        } else {
            top_objects.add(child);
        }
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
    }
}

void GameObjectList::setName(GameObject* object, const std::string& new_name) {
    if (new_name == object->name) {
        return;
    }
    assert(all_objects.contains(object));
    names.remove(object->name, object);
    object->name = new_name;
    names.add(new_name, object);
}

void GameObjectList::transformFromRigidbody() {
    for (size_t i = 0; i < top_objects.size(); i++) {
        GameObject* object = top_objects[i];
        object->transformFromRigidbody();
    }
}

void GameObjectList::remove(GameObject* object, bool remove_children) {
    assert(object);
    ptrdiff_t index = all_objects.getIndex(object);
    assert(index >= 0);
    std::vector<Joint*> joints_copy = object->joints.getVector();
    for (Joint* joint : joints_copy) {
        removeJoint(joint);
    }
    setParent(object, nullptr);
    CompoundVector<GameObject*> children_copy = object->getChildren();
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
    ids.remove(object->id);
    names.remove(object->name, object);
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
    ids.clear();
    names.clear();
}
