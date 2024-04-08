#include "simulation/joint.h"
#include "simulation/gameobject.h"
#include "simulation/objectlist.h"

Joint::~Joint() {
	size_t object_a_id = GameObject::getGameobject(joint->GetBodyA())->id;
	size_t object_b_id = GameObject::getGameobject(joint->GetBodyB())->id;
	joint->GetBodyA()->GetWorld()->DestroyJoint(joint);
}

bool Joint::operator==(const Joint& other) const {
	if (getType() != other.getType()) {
		return false;
	}
	if (
		object1->getId() != other.object1->getId() ||
		object2->getId() != other.object2->getId()
	) {
		return false;
	}
	if (getAnchorA() != other.getAnchorA()) {
		return false;
	}
	if (getAnchorB() != other.getAnchorB()) {
		return false;
	}
	if (getCollideConnected() != other.getCollideConnected()) {
		return false;
	}
	return true;
}

b2JointType Joint::getType() const {
	return joint->GetType();
}

b2Body* Joint::getBodyA() {
	return joint->GetBodyA();
}

b2Body* Joint::getBodyB() {
	return joint->GetBodyB();
}

b2Vec2 Joint::getAnchorA() const {
	return joint->GetAnchorA();
}

b2Vec2 Joint::getAnchorB() const {
	return joint->GetAnchorB();
}

bool Joint::getCollideConnected() const {
	return joint->GetCollideConnected();
}

std::string Joint::serialize() const {
	TokenWriter tw;
	serialize(tw);
	return tw.toStr();
}

RevoluteJoint::RevoluteJoint(const b2RevoluteJointDef& def, b2World* world, GameObject* object1, GameObject* object2) {
	this->object1 = object1;
	this->object2 = object2;
	b2RevoluteJointDef def_copy(def);
	def_copy.bodyA = object1->rigid_body;
	def_copy.bodyB = object2->rigid_body;
	joint = world->CreateJoint(&def_copy);
	revolute_joint = dynamic_cast<b2RevoluteJoint*>(joint);
}

RevoluteJoint::RevoluteJoint(b2RevoluteJoint* joint) {
	this->joint = joint;
	revolute_joint = dynamic_cast<b2RevoluteJoint*>(joint);
}

float RevoluteJoint::getReferenceAngle() const {
	return revolute_joint->GetReferenceAngle();
}

float RevoluteJoint::getJointAngle() const {
	return revolute_joint->GetJointAngle();
}

float RevoluteJoint::getJointSpeed() const {
	return revolute_joint->GetJointSpeed();
}

bool RevoluteJoint::isLimitEnabled() const {
	return revolute_joint->IsLimitEnabled();
}

void RevoluteJoint::enableLimit(bool flag) {
	revolute_joint->EnableLimit(flag);
}

float RevoluteJoint::getLowerLimit() const {
	return revolute_joint->GetLowerLimit();
}

float RevoluteJoint::getUpperLimit() const {
	return revolute_joint->GetUpperLimit();
}

void RevoluteJoint::setLimits(float lower, float upper) {
	revolute_joint->SetLimits(lower, upper);
}

bool RevoluteJoint::isMotorEnabled() const {
	return revolute_joint->IsMotorEnabled();
}

void RevoluteJoint::enableMotor(bool flag) {
	revolute_joint->EnableMotor(flag);
}

void RevoluteJoint::setMotorSpeed(float speed) {
	revolute_joint->SetMotorSpeed(speed);
}

float RevoluteJoint::getMotorSpeed() const {
	return revolute_joint->GetMotorSpeed();
}

void RevoluteJoint::setMaxMotorTorque(float torque) {
	revolute_joint->SetMaxMotorTorque(torque);
}

float RevoluteJoint::getMaxMotorTorque() const {
	return revolute_joint->GetMaxMotorTorque();
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
		tw.writeFloatParam("reference_angle", joint->GetReferenceAngle());
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

std::unique_ptr<RevoluteJoint> RevoluteJoint::deserialize(const std::string& str, GameObjectList* object_list) {
	TokenReader tr(str);
	std::unique_ptr<RevoluteJoint> uptr = deserialize(tr, object_list);
	return uptr;
}

std::unique_ptr<RevoluteJoint> RevoluteJoint::deserialize(TokenReader& tr, GameObjectList* object_list) {
	try {
		b2RevoluteJointDef def;
		def.bodyA = nullptr;
		def.bodyB = nullptr;
		ptrdiff_t object_a_id = -1;
		ptrdiff_t object_b_id = -1;
		if (tr.tryEat("joint")) {
			tr.eat("revolute");
		}
		while (tr.validRange()) {
			std::string pname = tr.readString();
			if (pname == "body_a") {
				object_a_id = tr.readULL();
			} else if (pname == "body_b") {
				object_b_id = tr.readULL();
			} else if (pname == "anchor_a") {
				def.localAnchorA = tr.readb2Vec2();
			} else if (pname == "anchor_b") {
				def.localAnchorB = tr.readb2Vec2();
			} else if (pname == "collide_connected") {
				def.collideConnected = tr.readBool();
			} else if (pname == "reference_angle") {
				def.referenceAngle = tr.readFloat();
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
		GameObject* object1 = object_list->getById(object_a_id);
		GameObject* object2 = object_list->getById(object_b_id);
		std::unique_ptr<RevoluteJoint> uptr = std::make_unique<RevoluteJoint>(def, object_list->getWorld(), object1, object2);
		return uptr;
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

bool RevoluteJoint::operator==(const RevoluteJoint& other) const {
	const RevoluteJoint* other_ptr = dynamic_cast<const RevoluteJoint*>(&other);
	if (!other_ptr) {
		return false;
	}
	if (static_cast<const Joint&>(*this) != other) {
		return false;
	}
	if (getLowerLimit() != other_ptr->getLowerLimit()) {
		return false;
	}
	if (getMaxMotorTorque() != other_ptr->getMaxMotorTorque()) {
		return false;
	}
	if (getMotorSpeed() != other_ptr->getMotorSpeed()) {
		return false;
	}
	if (getReferenceAngle() != other_ptr->getReferenceAngle()) {
		return false;
	}
	if (getUpperLimit() != other_ptr->getUpperLimit()) {
		return false;
	}
	if (isLimitEnabled() != other_ptr->isLimitEnabled()) {
		return false;
	}
	if (isMotorEnabled() != other_ptr->isMotorEnabled()) {
		return false;
	}
	return true;
}
