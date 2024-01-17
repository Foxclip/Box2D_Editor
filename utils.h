#pragma once

#include <utility>
#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"

namespace utils {

	float to_degrees(float angle);
	float to_radians(float angle);
	std::pair<float, float> getCircleVertex(int index, int point_count, float radius, float offset = 0.0f);
	b2Vec2 tob2(const sf::Vector2f& vec);
	sf::Vector2f tosf(const b2Vec2& vec);
	sf::Vector2i to2i(const sf::Vector2f& vec);
	sf::Vector2f to2f(const sf::Vector2i& vec);
	int get_max_offset(const sf::Vector2i& v1, const sf::Vector2i& v2);
	void set_origin_to_center_normal(sf::Text& text);
	void set_origin_to_center_bounds(sf::Text& text);
	bool contains_point(const sf::FloatRect& rect, const sf::Vector2f& point);
	bool contains_point(const sf::RectangleShape& shape, const sf::Vector2f& point);
	b2Vec2 rot90CCW(const b2Vec2& vec);
	b2Vec2 rot90CW(const b2Vec2& vec);
	std::vector<b2Vec2> get_rect(const b2Vec2& p1, const b2Vec2& p2, float offset);
	float get_line_D(const b2Vec2& p0, const b2Vec2& p1, const b2Vec2& p2);
	float distance_to_line(const b2Vec2& p0, const b2Vec2& p1, const b2Vec2& p2);
	bool left_side(const b2Vec2& p0, const b2Vec2& p1, const b2Vec2& p2);
	bool contains_point(const std::vector<b2Vec2>& polygon, const b2Vec2& point);
}
