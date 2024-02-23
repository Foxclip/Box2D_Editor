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
        for (size_t i = 0; i < object->getChildren().size(); i++) {
            addToAll(object->getChildren()[i].get(), assign_new_id);
        }
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
    } catch (std::exception exc) {
        throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
    }
}

bool GameObjectList::removeFromAll(GameObject* object) {
    bool result = false;
    for (size_t i = 0; i < all_objects.size(); i++) {
        if (all_objects[i] == object) {
            GameObject* parent = object->getParent();
            if (parent) {
                parent->removeChild(object);
            }
            std::vector<Joint*> joints_copy(joints.begin(), joints.end());
            for (Joint* joint : joints_copy) {
                joint->valid = false;
                removeJoint(joint);
            }
            all_objects.erase(all_objects.begin() + i);
            ids.erase(ObjectId(object->id, nullptr));
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
