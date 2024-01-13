#pragma once

#include <utility>

namespace utils {

	float to_degrees(float angle);
	float to_radians(float angle);
	std::pair<float, float> getCircleVertex(int index, int point_count, float radius, float offset = 0.0f);

}
