#include "utils.h"
#include <numbers>
#include <utility>
#include <cmath>
#include "tokenizer.h"

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
		for (int i = 0; i < vec.size(); i++) {
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

	b2Vec2 get_pos(const std::vector<float>& lengths, int i) {
		float angle = (float)i / lengths.size() * 2 * b2_pi;
		b2Vec2 vector = b2Vec2(std::cos(angle), std::sin(angle));
		int index = i < lengths.size() ? i : i % lengths.size();
		b2Vec2 pos = lengths[index] * vector;
		return pos;
	}

	std::vector<std::string> splitString(std::string str) {
		std::vector<std::string> results;
		std::string current_word;
		str += EOF;
		for (int i = 0; i < str.size(); i++) {
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

}
