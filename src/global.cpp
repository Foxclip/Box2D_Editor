#include "global.h"

sf::VertexArray line_primitive = sf::VertexArray(sf::Lines, 2);

Logger& operator<<(Logger& lg, const b2Vec2& value) {
	return lg << "(" << value.x << " " << value.y << ")";
}

void draw_line(
	sf::RenderTarget& target,
	const sf::Vector2f& v1,
	const sf::Vector2f& v2,
	const sf::Color& color
) {
	line_primitive[0].position = utils::quantize_and_offset(v1);
	line_primitive[0].color = color;
	line_primitive[1].position = utils::quantize_and_offset(v2);
	line_primitive[1].color = color;
	target.draw(line_primitive);
}

void draw_line(
	sf::RenderTarget& target,
	const sf::Vector2f& v1,
	const sf::Vector2f& v2,
	const sf::Color& color,
	const sf::Transform& transform
) {
	line_primitive[0].position = utils::quantize_and_offset(transform.transformPoint(v1));
	line_primitive[0].color = color;
	line_primitive[1].position = utils::quantize_and_offset(transform.transformPoint(v2));
	line_primitive[1].color = color;
	target.draw(line_primitive);
}

void draw_wire_rect(
	sf::RenderTarget& target,
	const sf::FloatRect& bounds,
	const sf::Color& color
) {
	sf::Vector2f topRight(bounds.left + bounds.width, bounds.top);
	sf::Vector2f topLeft(bounds.left, bounds.top);
	sf::Vector2f bottomLeft(bounds.left, bounds.top + bounds.height);
	sf::Vector2f bottomRight(bounds.left + bounds.width, bounds.top + bounds.height);
	draw_line(target, topRight, topLeft, color);
	draw_line(target, topLeft, bottomLeft, color);
	draw_line(target, bottomLeft, bottomRight, color);
	draw_line(target, bottomRight, topRight, color);
}

void draw_wire_rect(
	sf::RenderTarget& target,
	const sf::FloatRect& bounds,
	const sf::Color& color,
	const sf::Transform& transform
) {
	sf::Vector2f topRight(bounds.left + bounds.width, bounds.top);
	sf::Vector2f topLeft(bounds.left, bounds.top);
	sf::Vector2f bottomLeft(bounds.left, bounds.top + bounds.height);
	sf::Vector2f bottomRight(bounds.left + bounds.width, bounds.top + bounds.height);
	draw_line(target, topRight, topLeft, color, transform);
	draw_line(target, topLeft, bottomLeft, color, transform);
	draw_line(target, bottomLeft, bottomRight, color, transform);
	draw_line(target, bottomRight, topRight, color, transform);
}
