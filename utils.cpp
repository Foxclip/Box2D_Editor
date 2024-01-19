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

	std::string body_type_to_str(b2BodyType type) {
		switch (type) {
			case b2_staticBody: return "static";
			case b2_kinematicBody: return "kinematic";
			case b2_dynamicBody: return "dynamic";
		}
	}

	void str_to_file(std::string str, std::string path) {
		std::ofstream ofstream(path);
		if (ofstream.is_open()) {
			ofstream << str;
		} else {
			std::string p(path);
			p.resize(FILENAME_MAX);
			throw std::runtime_error("File write error: " + p);
		}
	}

}
