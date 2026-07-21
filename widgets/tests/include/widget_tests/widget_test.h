#pragma once

#include <SFML/Graphics.hpp>
#include "widgets/font.h"
#include "test_lib/test.h"

class WidgetTest : public test::TestModule {
public:
	WidgetTest(const std::string& name, test::TestModule* parent, const std::vector<TestNode*>& required_nodes = { });

protected:
	sf::RenderWindow& getWindow();
	fw::Font& getFont();

};
