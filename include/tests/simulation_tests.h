#pragma once

#include "simulation/simulation.h"
#include "test.h"

class SimulationTests : public test::TestModule {
public:
	SimulationTests();

protected:
	void createTestLists() override;

private:
};
