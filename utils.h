#pragma once

#include <utility>
#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

namespace utils {

	float to_degrees(float angle);
	float to_radians(float angle);
	std::pair<float, float> getCircleVertex(int index, int point_count, float radius, float offset = 0.0f);
	b2Vec2 tob2(sf::Vector2f vec);
	sf::Vector2f tosf(b2Vec2 vec);

}
