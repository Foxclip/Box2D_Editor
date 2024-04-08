#pragma once

#include <serializer.h>

class GameObject;

class Joint {
public:
	GameObject* object1 = nullptr;
	GameObject* object2 = nullptr;

	virtual TokenWriter& serialize(TokenWriter& tw) const = 0;
	~Joint();

protected:
	b2Joint* joint = nullptr;
};

class RevoluteJoint : public Joint {
public:
	RevoluteJoint(const b2RevoluteJointDef& def, b2World* world, GameObject* object1, GameObject* object2);
	RevoluteJoint(b2RevoluteJoint* joint);
	b2Vec2 getAnchorA() const;
	b2Vec2 getAnchorB() const;
	TokenWriter& serialize(TokenWriter& tw) const override;
	static b2RevoluteJointDef deserialize(TokenReader& tr, ptrdiff_t& p_body_a, ptrdiff_t& p_body_b);

private:
};
