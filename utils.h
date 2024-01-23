#pragma once

#include <utility>
#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include "tokenizer.h"

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
	std::string body_type_to_str(b2BodyType type);
	b2BodyType str_to_body_type(std::string str);
	void str_to_file(std::string str, std::string path);
	std::string file_to_str(std::filesystem::path path);
	std::string color_to_str(sf::Color color);
	std::string farr_to_str(std::vector<float>& vec);
	std::string bool_to_str(bool value);
	b2Vec2 get_pos(const std::vector<float>& lengths, int i);

	template <typename T>
	T rot90CCW(const T& vec) {
		return T(-vec.y, vec.x);
	}

	template <typename T>
	T rot90CW(const T& vec) {
		return T(vec.y, -vec.x);
	}

	template <typename T>
	float dot(const T& v1, const T& v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}

	template <typename T>
	T normalize(const T& vec) {
		float length = sqrt(vec.x * vec.x + vec.y * vec.y);
		float inv_length = 1.0f / length;
		return inv_length * vec;
	}

	template <typename T>
	float get_line_D(const T& p0, const T& p1, const T& p2) {
		float D = (p2.x - p1.x) * (p0.y - p1.y) - (p2.y - p1.y) * (p0.x - p1.x);
		return D;
	}

	template <typename T>
	bool left_side(const T& p0, const T& p1, const T& p2) {
		return get_line_D(p0, p1, p2) > 0;
	}

	template <typename T>
	float distance_to_line(const T& p0, const T& p1, const T& p2) {
		return abs(get_line_D(p0, p1, p2)) / (p2 - p1).Length();
	}

	template <typename T>
	T line_project(const T& p0, const T& p1, const T& p2) {
		T A = p0 - p1;
		T B = p2 - p1;
		return p1 + dot(A, B) / dot(B, B) * B;
	}

	template <typename T>
	bool contains_point(const std::vector<T>& polygon, const T& point) {
		for (int i = 0; i < polygon.size(); i++) {
			T p1 = polygon[i];
			T p2 = polygon[(i + 1) % polygon.size()];
			if (!left_side(p1, p2, point)) {
				return false;
			}
		}
		return true;
	}

	template <typename T>
	float get_length(const T& vec) {
		return sqrt(vec.x * vec.x + vec.y * vec.y);
	}

	template <typename T>
	std::string vec_to_str(const T& vec) {
		return std::to_string(vec.x) + " " + std::to_string(vec.y);
	}

}
