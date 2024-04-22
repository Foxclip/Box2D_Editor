#pragma once

#include <SFML/Graphics.hpp>
#include <cassert>
#include <string>
#include <iostream>

namespace fw {

#ifndef NDEBUG

	void _print_msg(bool value);
	void _print_msg(bool value, const std::string& message);

#define wAssert(value, ...) \
	_print_msg(value, __VA_ARGS__); \
	assert(value);

#else

#define wAssert(value, ...)

#endif // !NDEBUG

	sf::Vector2i to2i(const sf::Vector2f& vec);
	sf::Vector2f to2f(const sf::Vector2i& vec);
	sf::Vector2f to2f(const sf::Vector2u& vec);
	void extend_bounds(sf::FloatRect& rect1, const sf::FloatRect& rect2);
	bool parseLL(const std::string& str, long long& result);
	bool parseFloat(const std::string& str, float& result);
	bool contains_point(const sf::FloatRect& rect, const sf::Vector2f& point);
	bool contains_point(const sf::RectangleShape& shape, const sf::Vector2f& point);
	void quantize_position(sf::Transform& transform);
	enum QuantizeMode {
		QUANTIZE_MODE_FLOOR,
		QUANTIZE_MODE_FLOOR_SUBTRACT,
		QUANTIZE_MODE_CEIL_SUBTRACT,
	};
	sf::FloatRect quantize_rect(const sf::FloatRect& rect, QuantizeMode quantize_mode);

}
