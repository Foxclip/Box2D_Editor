#include "utils.h"
#include <numbers>
#include <utility>
#include <cmath>

namespace utils {

	const float DEG_IN_RAD = 180.0f / std::numbers::pi;
	const float RAD_IN_DEG = std::numbers::pi / 180.0f;

	float to_degrees(float angle) {
		return DEG_IN_RAD * angle;
	}

	float to_radians(float angle) {
		return RAD_IN_DEG * angle;
	}

	std::pair<float, float> getCircleVertex(int index, int point_count, float radius, float offset) {
		float angle = (float)index / point_count * 2 * std::numbers::pi + offset;
		float x = std::cos(angle) * radius;
		float y = std::sin(angle) * radius;
		return std::make_pair(x, y);
	}

	b2Vec2 tob2(sf::Vector2f vec) {
		return b2Vec2(vec.x, vec.y);
	}

	sf::Vector2f tosf(b2Vec2 vec) {
		return sf::Vector2f(vec.x, vec.y);
	}

}
