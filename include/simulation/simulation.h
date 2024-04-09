#pragma once

#include <memory>
#include "objectlist.h"

class Simulation : public GameObjectList {
public:
	const int32 VELOCITY_ITERATIONS = 6;
	const int32 POSITION_ITERATIONS = 2;

	Simulation();
	void advance(float time_step);
	void load(const std::string& filename);
	void reset();
	std::string serialize() const;
	std::string serialize(TokenWriter& tw) const;
	void deserialize(const std::string& str);
	void deserialize(TokenReader& tr);
	BoxObject* createBox(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const b2Vec2& size,
		const sf::Color& color
	);
	BallObject* createBall(
		const std::string& name,
		const b2Vec2& pos,
		float radius,
		const sf::Color& color,
		const sf::Color& notch_color = sf::Color::Transparent
	);
	PolygonObject* createPolygon(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const std::vector<b2Vec2>& vertices,
		const sf::Color& color
	);
	PolygonObject* createCar(
		const std::string& name,
		const b2Vec2& pos,
		const std::vector<float>& lengths,
		const std::vector<float>& wheels,
		const sf::Color& color
	);
	ChainObject* createChain(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const std::vector<b2Vec2>& vertices,
		const sf::Color& color
	);
	RevoluteJoint* createRevoluteJoint(
		const b2RevoluteJointDef& def,
		GameObject* obj1,
		GameObject* obj2
	);

private:

};
