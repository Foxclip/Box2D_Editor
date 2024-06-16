#pragma once

#include "editor/simulation/simulation.h"
#include "test.h"

class SimulationTests : public test::TestModule {
public:
	SimulationTests(test::TestManager& manager);
	void createTestLists() override;

protected:
	void createSimulationList();
	void createGameObjectList();
	void createObjectListList();

private:
	static std::string colorToStr(const sf::Color& color);
	static std::string b2Vec2ToStr(const b2Vec2& vec);
	BoxObject* createBox(Simulation& simulation, const std::string& name, const b2Vec2& pos) const;
	void objCmpCommon(test::Test& test, const GameObject* objA, const GameObject* objB, bool cmp_id = true);
	void boxCmp(test::Test& test, BoxObject* boxA, BoxObject* boxB, bool cmp_id = true);
	void ballCmp(test::Test& test, BallObject* ballA, BallObject* ballB, bool cmp_id = true);
	void polygonCmp(test::Test& test, PolygonObject* polygonA, PolygonObject* polygonB, bool cmp_id = true);
	void chainCmp(test::Test& test, ChainObject* chainA, ChainObject* chainB, bool cmp_id = true);
	void jointCmpCommon(test::Test& test, Joint* jointA, Joint* jointB);
	void revoluteJointCmp(test::Test& test, RevoluteJoint* jointA, RevoluteJoint* jointB);
	void simCmp(test::Test& test, Simulation& simA, Simulation& simB);
};
