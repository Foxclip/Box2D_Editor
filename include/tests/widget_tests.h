#pragma once

#include "widgets/widgets.h"
#include "test.h"

class TestApplication : public fw::Application {
public:
	bool initialized = false;
	bool started = false;
	bool closed = false;

	void onInit() override;
	void onStart() override;
	void onClose() override;
};

class WidgetTests : public test::TestModule {
public:
	WidgetTests(test::TestManager& manager);
	void createTestLists() override;

protected:
	void createApplicationList();

private:
	static std::string sfVec2uToStr(const sf::Vector2u& vec);

};
