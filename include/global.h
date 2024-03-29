#pragma once

#include <SFML/Graphics.hpp>
#include "utils.h"
#include "logger.h"

extern sf::VertexArray line_primitive;

Logger& operator<<(Logger& lg, const b2Vec2& value);
Logger& operator<<(Logger& lg, const sf::Vector2f& value);

void draw_line(
	sf::RenderTarget& target,
	const sf::Vector2f& v1,
	const sf::Vector2f& v2,
	const sf::Color& color
);

void draw_line(
	sf::RenderTarget& target,
	const sf::Vector2f& v1,
	const sf::Vector2f& v2,
	const sf::Color& color,
	const sf::Transform& transform
);

void draw_wire_rect(
	sf::RenderTarget& target,
	const sf::FloatRect& bounds,
	const sf::Color& color
);

void draw_wire_rect(
	sf::RenderTarget& target,
	const sf::FloatRect& bounds,
	const sf::Color& color,
	const sf::Transform& transform
);
