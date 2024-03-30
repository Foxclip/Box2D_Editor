#include "widgets/widgets_common.h"

namespace fw {

#ifndef NDEBUG

	void _print_msg() { }

	void _print_msg(const std::string& message) {
		std::cout << message << "\n";
	}

#endif // !NDEBUG

	sf::Vector2i to2i(const sf::Vector2f& vec) {
		return sf::Vector2i((int)vec.x, (int)vec.y);
	}

	sf::Vector2f to2f(const sf::Vector2i& vec) {
		return sf::Vector2f((float)vec.x, (float)vec.y);
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

	bool parseLL(const std::string& str, long long& result) {
		const char* start = str.c_str();
		char* end;
		result = std::strtoll(start, &end, 10);
		if (end == start) {
			result = 0;
			return false;
		}
		return true;
	}

	bool parseFloat(const std::string& str, float& result) {
		const char* start = str.c_str();
		char* end;
		result = std::strtof(start, &end);
		if (end == start) {
			result = 0.0f;
			return false;
		}
		return true;
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

	void quantize_position(sf::Transform& transform) {
		float x_pos = transform.getMatrix()[12];
		float y_pos = transform.getMatrix()[13];
		float x_offset = x_pos - floor(x_pos);
		float y_offset = y_pos - floor(y_pos);
		sf::Vector2f subpixel_offset = sf::Vector2f(x_offset, y_offset);
		transform.translate(-subpixel_offset);
	}

	sf::FloatRect quantize_rect(const sf::FloatRect& rect, QuantizeMode quantize_mode) {
		auto rounding_func = [&](float x) {
			if (quantize_mode == QUANTIZE_MODE_FLOOR) {
				return floor(x);
			} else if (quantize_mode == QUANTIZE_MODE_FLOOR_SUBTRACT) {
				return floor(x) - 1.0f;
			} else if (quantize_mode == QUANTIZE_MODE_CEIL_SUBTRACT) {
				return ceil(x) - 1.0f;
			} else {
				wAssert(false, "Unknown QuantizeMode"); return floor(x);
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
