#pragma once

#include "simulation/simulation.h"
#include "test.h"

class SimulationTests : public test::TestModule {
public:
	SimulationTests(test::TestManager& manager);
	void createTestLists() override;

private:
	void basicTest(test::Test& test);
	void boxTest(test::Test& test);
	void ballTest(test::Test& test);
	void polygonTest(test::Test& test);
	void chainTest(test::Test& test);
	void revoluteJointTest(test::Test& test);
	void carTest(test::Test& test);
	void serializeTest(test::Test& test);
	void boxSerializeTest(test::Test& test);
	void ballSerializeTest(test::Test& test);
	void polygonSerializeTest(test::Test& test);
	void chainSerializeTest(test::Test& test);
	void revoluteJointSerializeTest(test::Test& test);
	void carSerializeTest(test::Test& test);
	void advanceTest(test::Test& test);
	void saveloadTest(test::Test& test);
	void boxStackTest(test::Test& test);
	void movingCarTest(test::Test& test);

	void setParentTwoTest(test::Test& test);
	void setParentThreeTest(test::Test& test);
	void parentLoopTest(test::Test& test);
	void setPositionTwoTest(test::Test& test);
	void setPositionThreeTest(test::Test& test);
	void setAngleTest(test::Test& test);
	void setVertexPosTest(test::Test& test);
	void addVertexTest(test::Test& test);
	void deleteVertexTest(test::Test& test);

	void objectsTest(test::Test& test);
	void jointsTest(test::Test& test);
	void addTest(test::Test& test);
	void addJointTest(test::Test& test);
	void duplicateTest(test::Test& test);
	void duplicateWithChildrenTest(test::Test& test);
	void removeJointTest(test::Test& test);
	void removeTest(test::Test& test);
	void removeWithoutChildren1Test(test::Test& test);
	void removeWithoutChildren2Test(test::Test& test);
	void removeWithChildrenTest(test::Test& test);
	void eventTest(test::Test& test);
	void clearTest(test::Test& test);

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
