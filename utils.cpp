#include "utils.h"
#include <utility>
#include <cmath>
#include <iostream>

namespace utils {

	const float DEG_IN_RAD = 180.0f / std::numbers::pi;
	const float RAD_IN_DEG = std::numbers::pi / 180.0f;

	float to_degrees(float angle) {
		return DEG_IN_RAD * angle;
	}

	float to_radians(float angle) {
		return RAD_IN_DEG * angle;
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
		return (
			point.x >= rect.left
			&& point.x <= rect.left + rect.width
			&& point.y >= rect.top
			&& point.y <= rect.top + rect.height
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

	b2BodyType str_to_body_type(std::string str) {
		try {
			if (str == "static") {
				return b2_staticBody;
			} else if (str == "kinematic") {
				return b2_kinematicBody;
			} else if (str == "dynamic") {
				return b2_dynamicBody;
			} else {
				throw std::runtime_error("Unknown body type: " + str);
			}
		} catch (std::exception exc) {
			throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
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

	std::string file_to_str(std::filesystem::path path) {
		if (!std::filesystem::exists(path)) {
			throw std::format("File not found: {}", path.string());
		}
		std::ifstream t(path);
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();
	}

	std::string color_to_str(sf::Color color) {
		return
			std::to_string(color.r)
			+ " " + std::to_string(color.g)
			+ " " + std::to_string(color.b)
			+ " " + std::to_string(color.a)
		;
	}

	std::string farr_to_str(std::vector<float>& vec) {
		std::string str;
		for (size_t i = 0; i < vec.size(); i++) {
			str += std::to_string(vec[i]);
			if (i < vec.size() - 1) {
				str += " ";
			}
		}
		return str;
	}

	std::string bool_to_str(bool value) {
		return value ? "true" : "false";
	}

	std::vector<std::string> splitString(std::string str) {
		std::vector<std::string> results;
		std::string current_word;
		str += EOF;
		for (size_t i = 0; i < str.size(); i++) {
			char c = str[i];
			if (c == '\n' || c == EOF) {
				if (current_word.size() > 0) {
					results.push_back(current_word);
				}
				if (c == '\n') {
					results.push_back("\n");
				}
				current_word = "";
			} else {
				current_word += c;
			}
		}
		return results;
	}

	std::string current_time() {
		time_t t;
		std::time(&t);
		tm l;
		localtime_s(&l, &t);
		std::stringstream ss;
		ss << std::setw(2) << std::setfill('0') << l.tm_hour;
		ss << ":" << std::setw(2) << std::setfill('0') << l.tm_min;
		ss << ":" << std::setw(2) << std::setfill('0') << l.tm_sec;
		return ss.str();
	}

	void extend_bounds(sf::FloatRect& rect1, const sf::FloatRect& rect2) {
		float rect1_right = rect1.left + rect1.width;
		float rect2_right = rect2.left + rect2.width;
		float rect1_bottom = rect1.top + rect1.height;
		float rect2_bottom = rect2.top + rect2.height;
		if (rect2.left < rect1.left) {
			rect1.left = rect2.left;
		}
		if (rect2.top < rect1.top) {
			rect1.top = rect2.top;
		}
		if (rect2_right > rect1_right) {
			rect1.width += rect2_right - rect1_right;
		}
		if (rect2_bottom > rect1_bottom) {
			rect1.height += rect2_bottom - rect1_bottom;
		}
	}

	void extend_bounds(sf::FloatRect& rect, const sf::Vector2f point) {
		float rect_right = rect.left + rect.width;
		float rect_bottom = rect.top + rect.height;
		if (point.x < rect.left) {
			rect.width += rect.left - point.x;
			rect.left = point.x;
		}
		if (point.y < rect.top) {
			rect.height += rect.top - point.y;
			rect.top = point.y;
		}
		if (point.x > rect_right) {
			rect.width += point.x - rect_right;
		}
		if (point.y > rect_bottom) {
			rect.height += point.y - rect_bottom;
		}
	}

	bool rect_fixture_intersect(const b2Vec2& lower_bound, const b2Vec2& upper_bound, const b2Fixture* fixture) {
		b2Vec2 bottom_left = b2Vec2(std::min(lower_bound.x, upper_bound.x), std::min(lower_bound.y, upper_bound.y));
		b2Vec2 bottom_right = b2Vec2(std::max(lower_bound.x, upper_bound.x), std::min(lower_bound.y, upper_bound.y));
		b2Vec2 top_left = b2Vec2(std::min(lower_bound.x, upper_bound.x), std::max(lower_bound.y, upper_bound.y));
		b2Vec2 top_right = b2Vec2(std::max(lower_bound.x, upper_bound.x), std::max(lower_bound.y, upper_bound.y));
		bool zero_area_rect = lower_bound.x == upper_bound.x || lower_bound.y == upper_bound.y;
		const b2Shape* shape = fixture->GetShape();
		const b2Body* body = fixture->GetBody();
		//b2Vec2 body_pos = body->GetPosition();
		auto point_inside_rect = [&](const b2Vec2& point) {
			if (zero_area_rect) {
				return false;
			}
			return
				!right_side(point, top_right, top_left)
				&& !right_side(point, top_left, bottom_left)
				&& !right_side(point, bottom_left, bottom_right)
				&& !right_side(point, bottom_right, top_right)
			;
		};
		auto world_pos = [&](const b2Vec2& pos) {
			return b2Mul(body->GetTransform(), pos);
		};
		auto point_inside_polygon = [&](const b2Vec2& point, const b2PolygonShape* polygon_shape) {
			for (size_t i = 0; i < polygon_shape->m_count; i++) {
				b2Vec2 v1 = world_pos(polygon_shape->m_vertices[i]);
				b2Vec2 v2 = world_pos(polygon_shape->m_vertices[neg_mod(i + 1, (size_t)polygon_shape->m_count)]);
				if (right_side(point, v1, v2)) {
					return false;
				}
			}
			return true;
		};
		if (const b2CircleShape* circle_shape = dynamic_cast<const b2CircleShape*>(shape)) {
			b2Vec2 circle_center = world_pos(circle_shape->m_p);
			if (point_inside_rect(circle_center)) {
				return true;
			}
			float radius = circle_shape->m_radius;
			if (
				length(bottom_left - circle_center) <= radius
				|| length(bottom_right - circle_center) <= radius
				|| length(top_left - circle_center) <= radius
				|| length(top_right - circle_center) <= radius
			) {
				return true;
			}
			b2Vec2 i1, i2;
			auto top_intersect = [&]() { return segment_circle_intersect(top_right, top_left, circle_center, radius, 0.0f, 0.0f, i1, i2); };
			auto left_intersect = [&]() { return segment_circle_intersect(top_left, bottom_left, circle_center, radius, 0.0f, 0.0f, i1, i2); };
			auto bottom_intersect = [&]() { return segment_circle_intersect(bottom_left, bottom_right, circle_center, radius, 0.0f, 0.0f, i1, i2); };
			auto right_intersect = [&]() { return segment_circle_intersect(bottom_right, top_right, circle_center, radius, 0.0f, 0.0f, i1, i2); };
			auto intersect = [&]() { return top_intersect() || left_intersect() || bottom_intersect() || right_intersect(); };
			return intersect();
		} else if (const b2PolygonShape* polygon_shape = dynamic_cast<const b2PolygonShape*>(shape)) {
			for (size_t i = 0; i < polygon_shape->m_count; i++) {
				b2Vec2 v = world_pos(polygon_shape->m_vertices[i]);
				if (point_inside_rect(v)) {
					return true;
				}
			}
			for (size_t i = 0; i < polygon_shape->m_count; i++) {
				b2Vec2 v1 = world_pos(polygon_shape->m_vertices[i]);
				b2Vec2 v2 = world_pos(polygon_shape->m_vertices[neg_mod(i + 1, (size_t)polygon_shape->m_count)]);
				b2Vec2 intersection;
				if (
					line_intersect(v1, v2, top_right, top_left, 0.0f, intersection)
					|| line_intersect(v1, v2, top_left, bottom_left, 0.0f, intersection)
					|| line_intersect(v1, v2, bottom_left, bottom_right, 0.0f, intersection)
					|| line_intersect(v1, v2, bottom_right, top_right, 0.0f, intersection)
				) {
					return true;
				}
			}
			return
				point_inside_polygon(bottom_left, polygon_shape)
				|| point_inside_polygon(bottom_right, polygon_shape)
				|| point_inside_polygon(top_left, polygon_shape)
				|| point_inside_polygon(top_right, polygon_shape)
			;
		} else if (const b2ChainShape* chain_shape = dynamic_cast<const b2ChainShape*>(shape)) {
			for (size_t i = 0; i < chain_shape->m_count; i++) {
				b2Vec2 v = world_pos(chain_shape->m_vertices[i]);
				if (point_inside_rect(v)) {
					return true;
				}
			}
			for (size_t i = 0; i < chain_shape->m_count - 1; i++) {
				b2Vec2 v1 = world_pos(chain_shape->m_vertices[i]);
				b2Vec2 v2 = world_pos(chain_shape->m_vertices[i + 1]);
				b2Vec2 intersection;
				if (
					line_intersect(v1, v2, top_right, top_left, 0.0f, intersection)
					|| line_intersect(v1, v2, top_left, bottom_left, 0.0f, intersection)
					|| line_intersect(v1, v2, bottom_left, bottom_right, 0.0f, intersection)
					|| line_intersect(v1, v2, bottom_right, top_right, 0.0f, intersection)
				) {
					return true;
				}
			}
			return false;
		} else {
			return false;
		}
	}

	float sgn(float value) {
		if (value >= 0.0f) {
			return 1.0f;
		} else {
			return -1.0f;
		}
	}

	std::string char_to_str(char c) {
		if (c < -1) {
			return "(" + std::to_string(c) + ")";
		} else if (c == '\n') {
			return "\\n";
		} else if (c == '\r') {
			return "\\r";
		} else if (c == '\t') {
			return "\\t";
		} else if (c == '\0') {
			return "\\0";
		} else if (c == '\\') {
			return "\\\\";
		} else if (c == '"') {
			return "\\\"";
		} else if (c == EOF) {
			return "(EOF)";
		} else {
			return std::string(1, c);
		}
	}

	std::string char_to_esc(std::string str) {
		std::string result;
		for (size_t i = 0; i < str.size(); i++) {
			char current_char = str[i];
			result += char_to_str(current_char);
		}
		return result;
	}

	std::string esc_to_char(std::string str) {
		try {
			std::string result;
			enum State {
				NORMAL,
				ESCAPE,
			};
			State state = NORMAL;
			std::string str_orig = str;
			str += EOF;
			for (size_t i = 0; i < str.size(); i++) {
				char current_char = str[i];
				if (state == NORMAL) {
					if (current_char == '\\') {
						state = ESCAPE;
					} else if (current_char == EOF) {
						break;
					} else {
						result += current_char;
					}
				} else if (state == ESCAPE) {
					if (current_char == 'n') {
						result += "\n";
						state = NORMAL;
					} else if (current_char == 'r') {
						result += "\r";
						state = NORMAL;
					} else if (current_char == 't') {
						result += "\t";
						state = NORMAL;
					} else if (current_char == '"') {
						result += "\"";
						state = NORMAL;
					} else if (current_char == '\\') {
						result += "\\";
						state = NORMAL;
					} else {
						throw std::runtime_error("Unexpected char: " + char_to_str(current_char));
					}
				}
			}
			return result;
		} catch (std::exception exc) {
			throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
		}
	}

	void quantize_position(sf::Transform& transform) {
		float x_pos = transform.getMatrix()[12];
		float y_pos = transform.getMatrix()[13];
		float x_offset = x_pos - floor(x_pos);
		float y_offset = y_pos - floor(y_pos);
		sf::Vector2f subpixel_offset = sf::Vector2f(x_offset, y_offset);
		transform.translate(-subpixel_offset);
	}

	sf::FloatRect quantize_rect(const sf::FloatRect& rect, bool floor_bottom_right) {
		auto rounding_func = [&](float x) {
			if (floor_bottom_right) {
				return floor(x);
			} else {
				return ceil(x);
			}
		};
		sf::Vector2f top_left = rect.getPosition();
		sf::Vector2f bottom_right = top_left + rect.getSize();
		sf::Vector2f quantized_top_left = sf::Vector2f(floor(top_left.x), floor(top_left.y));
		sf::Vector2f quantized_bottom_right = sf::Vector2f(rounding_func(bottom_right.x), rounding_func(bottom_right.y));
		float quantized_width = quantized_bottom_right.x - quantized_top_left.x;
		float quantized_height = quantized_bottom_right.y - quantized_top_left.y;
		sf::FloatRect quantized_bounds(quantized_top_left, sf::Vector2f(quantized_width, quantized_height));
		return quantized_bounds;
	}

}
