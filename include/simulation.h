#pragma once

#include <memory>
#include "objectlist.h"
#include "test.h"

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
	b2World* getWorld() const;
	BoxObject* create_box(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const b2Vec2& size,
		const sf::Color& color
	);
	BallObject* create_ball(
		const std::string& name,
		const b2Vec2& pos,
		float radius,
		const sf::Color& color,
		const sf::Color& notch_color = sf::Color::Transparent
	);
	PolygonObject* create_polygon(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const std::vector<b2Vec2>& vertices,
		const sf::Color& color
	);
	PolygonObject* create_car(
		const std::string& name,
		const b2Vec2& pos,
		const std::vector<float>& lengths,
		const std::vector<float>& wheels,
		const sf::Color& color
	);
	ChainObject* create_chain(
		const std::string& name,
		const b2Vec2& pos,
		float angle,
		const std::vector<b2Vec2>& vertices,
		const sf::Color& color
	);

private:
	void load_from_file(const std::string& filename);

};

class SimulationTests : public test::TestModule {
public:
	SimulationTests();

protected:
	void createTestLists() override;

private:
};
