#pragma once

#include <box2d/box2d.h>

class GameObject;

class GameObjectTransform {
public:
	GameObjectTransform(const GameObject* object);
	const b2Transform& getTransform() const;
	const b2Transform& getGlobalTransform() const;
	void invalidateGlobalTransform();
	void setTransform(const b2Vec2& position, float angle);
	void setGlobalTransform(const b2Transform& transform);
	void setPosition(const b2Vec2& position);
	void setAngle(float angle);
	bool operator==(const GameObjectTransform& other) const;

private:
	friend class GameObject;
	const GameObject* object = nullptr;
	b2Transform transform;
	mutable b2Transform global_transform;
	mutable bool global_transform_valid = false;

	void recalcGlobalTransform() const;

};
bool operator==(const b2Rot& left, const b2Rot& right);
