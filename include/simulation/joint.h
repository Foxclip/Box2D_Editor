#pragma once

#include "serializer.h"
#include "common/data_pointer_unique.h"

class GameObject;
class GameObjectList;

// adding Joint derived class
// add isEqual to derived class
// add comparison to == operator in Joint
// add comparisons to Simulation test module

class Joint {
public:
	GameObject* object1 = nullptr;
	GameObject* object2 = nullptr;

	virtual ~Joint();
	b2JointType getType() const;
	b2Body* getBodyA();
	b2Body* getBodyB();
	b2Vec2 getAnchorA() const;
	b2Vec2 getAnchorB() const;
	bool getCollideConnected() const;
	std::string serialize() const;
	virtual TokenWriter& serialize(TokenWriter& tw) const = 0;
	bool operator==(const Joint& other) const;

protected:
	b2Joint* joint = nullptr;
};

class RevoluteJoint : public Joint {
public:
	RevoluteJoint(const b2RevoluteJointDef& def, b2World* world, GameObject* object1, GameObject* object2);
	RevoluteJoint(b2RevoluteJoint* joint);
	float getReferenceAngle() const;
	float getJointAngle() const;
	float getJointSpeed() const;
	bool isLimitEnabled() const;
	void enableLimit(bool flag);
	float getLowerLimit() const;
	float getUpperLimit() const;
	void setLimits(float lower, float upper);
	bool isMotorEnabled() const;
	void enableMotor(bool flag);
	void setMotorSpeed(float speed);
	float getMotorSpeed() const;
	void setMaxMotorTorque(float torque);
	float getMaxMotorTorque() const;
	using Joint::serialize;
	TokenWriter& serialize(TokenWriter& tw) const override;
	static DataPointer<RevoluteJoint> deserialize(const std::string& str, GameObjectList* object_list);
	static DataPointer<RevoluteJoint> deserialize(
		const std::string& str, GameObjectList* object_list, GameObject* new_object_a, GameObject* new_object_b
	);
	static DataPointer<RevoluteJoint> deserialize(TokenReader& tr, GameObjectList* object_list);
	static DataPointer<RevoluteJoint> deserialize(
		TokenReader& tr, GameObjectList* object_list, GameObject* new_object_a, GameObject* new_object_b
	);
	bool isEqual(const Joint* other) const;

private:
	b2RevoluteJoint* revolute_joint = nullptr;

};
