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
	std::pair<float, float> getCircleVertex(ptrdiff_t index, size_t point_count, float radius, float offset = 0.0f);
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
	b2Vec2 get_pos(const std::vector<float>& lengths, ptrdiff_t i);
	std::vector<std::string> splitString(std::string str);
	std::string current_time();
	void extend_bounds(sf::FloatRect& rect1, const sf::FloatRect& rect2);
	void extend_bounds(sf::FloatRect& rect, const sf::Vector2f point);

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
	float length(const T& vec) {
		return sqrt(vec.x * vec.x + vec.y * vec.y);
	}

	template <typename T>
	T normalize(const T& vec) {
		float inv_length = 1.0f / length(vec);
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
	bool right_side(const T& p0, const T& p1, const T& p2) {
		return get_line_D(p0, p1, p2) < 0;
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

	template <typename TVec2>
	float cross2d(const TVec2& v1, const TVec2& v2) {
		return v1.x * v2.y - v1.y * v2.x;
	}

	template <typename TVec2>
	bool line_intersect(const TVec2& v1, const TVec2& v2, const TVec2& v3, const TVec2& v4, float epsilon, TVec2& intersection) {
		TVec2 p = v1;
		TVec2 r = v2 - v1;
		TVec2 q = v3;
		TVec2 s = v4 - v3;
		TVec2 qp = q - p;
		float rs = cross2d(r, s);
		float qps = cross2d(qp, s);
		float qpr = cross2d(qp, r);
		float t = qps / rs;
		float u = qpr / rs;
		if (abs(rs) >= epsilon && t >= 0.0f + epsilon && t <= 1.0f - epsilon && u >= 0.0f + epsilon && u <= 1.0f - epsilon) {
			intersection = p + t * r;
			return true;
		} else {
			return false;
		}
	}

	template <typename TVec2>
	float sqr_distance(const TVec2& v1, const TVec2& v2) {
		TVec2 C = v1 - v2;
		return dot(C, C);
	}

	template <typename TVec2>
	float vector_angle(const TVec2& v1, const TVec2& v2) {
		float d = dot(v1, v2);
		float l = length(v1) * length(v2);
		float dl = d / l;
		return acos(dl);
	}

	template <typename T>
	T neg_mod(T a, T b) {
		return (a % b + b) % b;
	}

	template <typename T>
	bool contains_point(const std::vector<T>& polygon, const T& point) {
		for (size_t i = 0; i < polygon.size(); i++) {
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

	template <typename T, typename U>
	void remove_by_value(T& vec, const U& val) {
		vec.erase(std::remove(vec.begin(), vec.end(), val), vec.end());
	}

	template<typename T>
	T calc_new_average(T avg, size_t n, T new_value) {
		T weight = 1.0 / (n + 1);
		T sum_weight = 1.0 - weight;
		avg = sum_weight * avg + weight * new_value;
		return avg;
	}

	template<typename T>
	T average(const std::vector<T>& vec) {
		T avg = 0.0;
		for (size_t i = 0; i < vec.size(); i++) {
			avg = calc_new_average(avg, i, vec[i]);
		}
		return avg;
	}

}
