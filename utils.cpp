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
		float x = std::cos(angle);
		float y = std::sin(angle);
		return std::make_pair(x, y);
	}

}
