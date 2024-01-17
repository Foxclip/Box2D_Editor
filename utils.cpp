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

	b2Vec2 tob2(const sf::Vector2f& vec) {
		return b2Vec2(vec.x, vec.y);
	}

	sf::Vector2f tosf(const b2Vec2& vec) {
		return sf::Vector2f(vec.x, vec.y);
	}

	sf::Vector2i to2i(const sf::Vector2f& vec) {
		return sf::Vector2i(vec.x, vec.y);
	}

	sf::Vector2f to2f(const sf::Vector2i& vec) {
		return sf::Vector2f(vec.x, vec.y);
	}

	int get_max_offset(const sf::Vector2i& v1, const sf::Vector2i& v2) {
		float offset_x = abs(v1.x - v2.x);
		float offset_y = abs(v1.y - v2.y);
		return std::max(offset_x, offset_y);
	}

	void set_origin_to_center_normal(sf::Text& text) {
		float x = text.getLocalBounds().width / 2.0f;
		float y = text.getCharacterSize() / 2.0f;
		text.setOrigin(x, y);
	}

	void set_origin_to_center_bounds(sf::Text& text) {
		sf::FloatRect text_bounds = text.getLocalBounds();
		text.setOrigin(text_bounds.width / 2.0f, text_bounds.height / 2.0f);
	}

	bool contains_point(const sf::FloatRect& rect, const sf::Vector2f& point) {
		return !(
			point.x < rect.left
			|| point.x > rect.left + rect.width
			|| point.y < rect.top
			|| point.y > rect.top + rect.height
		);
	}

	bool contains_point(const sf::RectangleShape& shape, const sf::Vector2f& point) {
		return contains_point(shape.getGlobalBounds(), point);
	}

	b2Vec2 rot90CCW(const b2Vec2& vec) {
		return b2Vec2(-vec.y, vec.x);
	}

	b2Vec2 rot90CW(const b2Vec2& vec) {
		return b2Vec2(vec.y, -vec.x);
	}

	float get_line_D(const b2Vec2& p0, const b2Vec2& p1, const b2Vec2& p2) {
		float D =  (p2.x - p1.x) * (p0.y - p1.y) - (p2.y - p1.y) * (p0.x - p1.x);
		return D;
	}

	bool left_side(const b2Vec2& p0, const b2Vec2& p1, const b2Vec2& p2) {
		return get_line_D(p0, p1, p2) > 0;
	}

	float distance_to_line(const b2Vec2& p0, const b2Vec2& p1, const b2Vec2& p2) {
		return abs(get_line_D(p0, p1, p2)) / (p2 - p1).Length();
	}

	bool contains_point(const std::vector<b2Vec2>& polygon, const b2Vec2& point) {
		for (int i = 0; i < polygon.size(); i++) {
			b2Vec2 p1 = polygon[i];
			b2Vec2 p2 = polygon[(i + 1) % polygon.size()];
			if (!left_side(p1, p2, point)) {
				return false;
			}
		}
		return true;
	}

	float get_length(const sf::Vector2f& vec) {
		return sqrt(vec.x * vec.x + vec.y * vec.y);
	}

}
