#pragma once

#include <serializer.h>

class GameObject;

class Joint {
public:
	GameObject* object1 = nullptr;
	GameObject* object2 = nullptr;

	b2JointType getType() const;
	b2Body* getBodyA();
	b2Body* getBodyB();
	b2Vec2 getAnchorA() const;
	b2Vec2 getAnchorB() const;
	bool getCollideConnected() const;
	virtual TokenWriter& serialize(TokenWriter& tw) const = 0;
	~Joint();

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
	TokenWriter& serialize(TokenWriter& tw) const override;
	static b2RevoluteJointDef deserialize(TokenReader& tr, ptrdiff_t& p_body_a, ptrdiff_t& p_body_b);

private:
	b2RevoluteJoint* revolute_joint = nullptr;

};
