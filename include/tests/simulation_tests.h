#pragma once

#include "test.h"
#include "simulation.h"

class SimulationTests : public test::TestModule {
public:
	SimulationTests();

protected:
	void createTestLists() override;

private:
};
