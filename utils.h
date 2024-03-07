#pragma once

#include <utility>
#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include <fstream>
#include <filesystem>
#include <vector>
#include <numbers>

namespace utils {

	float to_degrees(float angle);
	float to_radians(float angle);
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
	std::vector<std::string> splitString(std::string str);
	std::string current_time();
	void extend_bounds(sf::FloatRect& rect1, const sf::FloatRect& rect2);
	void extend_bounds(sf::FloatRect& rect, const sf::Vector2f point);
	bool rect_fixture_intersect(const b2Vec2& lower_bound, const b2Vec2& upper_bound, const b2Fixture* fixture);
	float sgn(float value);
	std::string char_to_str(char c);
	std::string char_to_esc(std::string str);
	std::string esc_to_char(std::string str);

	template <typename TVec2>
	TVec2 get_circle_vertex(ptrdiff_t index, size_t point_count, float radius, float offset = 0.0f) {
		float angle = (float)index / point_count * 2 * std::numbers::pi + offset;
		float x = std::cos(angle) * radius;
		float y = std::sin(angle) * radius;
		return TVec2(x, y);
	}

	template <typename TVec2>
	TVec2 get_pos(const std::vector<float>& lengths, ptrdiff_t i) {
		float angle = (float)i / lengths.size() * 2 * b2_pi;
		TVec2 vector = TVec2(std::cos(angle), std::sin(angle));
		size_t index = i < lengths.size() ? i : i % lengths.size();
		TVec2 pos = lengths[index] * vector;
		return pos;
	}

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
		if (length(v1 - v2) <= abs(epsilon) || length(v3 - v4) <= abs(epsilon)) {
			return false;
		}
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
	int line_circle_intersect(const TVec2& v1, const TVec2& v2, const TVec2& center, float radius, float epsilon, TVec2& intersection1, TVec2& intersection2) {
		if (length(v1 - v2) <= abs(epsilon)) {
			return 0;
		}
		TVec2 l1 = v1 - center;
		TVec2 l2 = v2 - center;
		float dx = l2.x - l1.x;
		float dy = l2.y - l1.y;
		float dr = length(l2 - l1);
		float D = cross2d(l1, l2);
		float discr = (radius * radius) * (dr * dr) - (D * D);
		if (discr < 0) {
			return 0;
		} else if (discr >= -epsilon && discr <= epsilon) {
			float x = D * dy / (dr * dr);
			float y = -D * dx / (dr * dr);
			intersection1 = TVec2(x, y) + center;
			return 1;
		} else {
			float x1 = (D * dy - sgn(dy) * dx * sqrt(discr)) / (dr * dr);
			float y1 = (-D * dx - abs(dy) * sqrt(discr)) / (dr * dr);
			float x2 = (D * dy + sgn(dy) * dx * sqrt(discr)) / (dr * dr);
			float y2 = (-D * dx + abs(dy) * sqrt(discr)) / (dr * dr);
			intersection1 = TVec2(x1, y1) + center;
			intersection2 = TVec2(x2, y2) + center;
			return 2;
		}
	}

	template <typename TVec2>
	int segment_circle_intersect(const TVec2& v1, const TVec2& v2, const TVec2& center, float radius, float epsilon1, float epsilon2, TVec2& intersection1, TVec2& intersection2) {
		TVec2 i1, i2;
		int line_intersect = line_circle_intersect(v1, v2, center, radius, epsilon1, i1, i2);
		if (line_intersect == 0) {
			return 0;
		} else if (line_intersect == 1) {
			TVec2 B = v2 - v1;
			TVec2 A = i1 - v1;
			float t = dot(A, B) / dot(B, B);
			bool ti = t >= 0.0f + epsilon2 && t <= 1.0f - epsilon2;
			if (!ti) {
				return 0;
			} else {
				intersection1 = i1;
				return 1;
			}
		} else {
			TVec2 B = v2 - v1;
			TVec2 A1 = i1 - v1;
			float t1 = dot(A1, B) / dot(B, B);
			TVec2 A2 = i2 - v1;
			float t2 = dot(A2, B) / dot(B, B);
			bool t1i = t1 >= 0.0f + epsilon2 && t1 <= 1.0f - epsilon2;
			bool t2i = t2 >= 0.0f + epsilon2 && t2 <= 1.0f - epsilon2;
			if (!t1i && !t2i) {
				return 0;
			} else if (t1i && !t2i) {
				intersection1 = i1;
				return 1;
			} else if (!t1i && t2i) {
				intersection1 = i2;
				return 1;
			} else {
				intersection1 = i1;
				intersection2 = i2;
				return 2;
			}
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

	template <typename TVec2>
	TVec2 quantize(const TVec2& vec) {
		return TVec2(floor(vec.x) + 0.5f, floor(vec.y) + 0.5f);
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
			if (right_side(p1, p2, point)) {
				return false;
			}
		}
		return true;
	}

	template <typename TVec2>
	TVec2 rotate_point(const TVec2& point, const TVec2& pivot, float angle) {
		TVec2 rel_pos = point - pivot;
		float old_angle = atan2(rel_pos.y, rel_pos.x);
		float new_angle = old_angle + angle;
		float radius = length(rel_pos);
		float new_x = cos(new_angle) * radius;
		float new_y = sin(new_angle) * radius;
		TVec2 new_rel_pos = TVec2(new_x, new_y);
		TVec2 new_pos = pivot + new_rel_pos;
		return new_pos;
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
