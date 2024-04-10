#pragma once

#include "simulation/simulation.h"
#include "test.h"

class SimulationTests : public test::TestModule {
public:
	SimulationTests();

protected:
	void createTestLists() override;

private:
	static std::string colorToStr(const sf::Color& color);
	static std::string b2Vec2ToStr(const b2Vec2& vec);
	void objCmpCommon(test::Test& test, const GameObject* objA, const GameObject* objB);
	void jointCmpCommon(test::Test& test, Joint* jointA, Joint* jointB);
	void boxCmp(test::Test& test, BoxObject* boxA, BoxObject* boxB);
	void ballCmp(test::Test& test, BallObject* ballA, BallObject* ballB);
	void polygonCmp(test::Test& test, PolygonObject* polygonA, PolygonObject* polygonB);
	void chainCmp(test::Test& test, ChainObject* chainA, ChainObject* chainB);
	void simCmp(test::Test& test, Simulation& simA, Simulation& simB);
};
