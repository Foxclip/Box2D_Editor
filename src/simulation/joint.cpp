#include "simulation/joint.h"
#include "simulation/gameobject.h"

Joint::~Joint() {
	size_t object_a_id = GameObject::getGameobject(joint->GetBodyA())->id;
	size_t object_b_id = GameObject::getGameobject(joint->GetBodyB())->id;
	joint->GetBodyA()->GetWorld()->DestroyJoint(joint);
}

RevoluteJoint::RevoluteJoint(const b2RevoluteJointDef& def, b2World* world, GameObject* object1, GameObject* object2) {
	this->object1 = object1;
	this->object2 = object2;
	b2RevoluteJointDef def_copy(def);
	def_copy.bodyA = object1->rigid_body;
	def_copy.bodyB = object2->rigid_body;
	joint = world->CreateJoint(&def_copy);
}

RevoluteJoint::RevoluteJoint(b2RevoluteJoint* joint) {
	this->joint = joint;
}

b2Vec2 RevoluteJoint::getAnchorA() const {
	return joint->GetAnchorA();
}

b2Vec2 RevoluteJoint::getAnchorB() const {
	return joint->GetAnchorB();
}

TokenWriter& RevoluteJoint::serialize(TokenWriter& tw) const {
	b2RevoluteJoint* joint = static_cast<b2RevoluteJoint*>(this->joint);
	size_t bodyAId = GameObject::getGameobject(joint->GetBodyA())->id;
	size_t bodyBId = GameObject::getGameobject(joint->GetBodyB())->id;
	tw << "joint revolute" << "\n";
	{
		TokenWriterIndent joint_indent(tw);
		tw.writeSizetParam("body_a", bodyAId);
		tw.writeSizetParam("body_b", bodyBId);
		tw.writeb2Vec2Param("anchor_a", joint->GetLocalAnchorA());
		tw.writeb2Vec2Param("anchor_b", joint->GetLocalAnchorB());
		tw.writeBoolParam("collide_connected", joint->GetCollideConnected());
		tw.writeFloatParam("angle_lower_limit", joint->GetLowerLimit());
		tw.writeFloatParam("angle_upper_limit", joint->GetUpperLimit());
		tw.writeBoolParam("angle_limit_enabled", joint->IsLimitEnabled());
		tw.writeFloatParam("motor_max_torque", joint->GetMaxMotorTorque());
		tw.writeFloatParam("motor_speed", joint->GetMotorSpeed());
		tw.writeBoolParam("motor_enabled", joint->IsMotorEnabled());
	}
	tw << "/joint";
	return tw;
}

b2RevoluteJointDef RevoluteJoint::deserialize(TokenReader& tr, ptrdiff_t& p_body_a, ptrdiff_t& p_body_b) {
	try {
		b2RevoluteJointDef def;
		def.bodyA = nullptr;
		def.bodyB = nullptr;
		p_body_a = -1;
		p_body_b = -1;
		if (tr.tryEat("joint")) {
			tr.eat("revolute");
		}
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "body_a") {
				p_body_a = tr.readULL();
			} else if (pname == "body_b") {
				p_body_b = tr.readULL();
			} else if (pname == "anchor_a") {
				def.localAnchorA = tr.readb2Vec2();
			} else if (pname == "anchor_b") {
				def.localAnchorB = tr.readb2Vec2();
			} else if (pname == "collide_connected") {
				def.collideConnected = tr.readBool();
			} else if (pname == "angle_lower_limit") {
				def.lowerAngle = tr.readFloat();
			} else if (pname == "angle_upper_limit") {
				def.upperAngle = tr.readFloat();
			} else if (pname == "angle_limit_enabled") {
				def.enableLimit = tr.readBool();
			} else if (pname == "motor_max_torque") {
				def.maxMotorTorque = tr.readFloat();
			} else if (pname == "motor_speed") {
				def.motorSpeed = tr.readFloat();
			} else if (pname == "motor_enabled") {
				def.enableMotor = tr.readBool();
			} else if (pname == "/joint") {
				break;
			} else {
				throw std::runtime_error("Unknown RevoluteJoint parameter name: " + pname);
			}
		}
		return def;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}
