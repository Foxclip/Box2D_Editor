#pragma once

#include <SFML/Graphics.hpp>

namespace fw {

	template <typename TVec2>
	TVec2 quantize_and_offset(const TVec2& vec) {
		return TVec2(floor(vec.x) + 0.5f, floor(vec.y) + 0.5f);
	}

	class CanvasWidget;

	void draw_line(
		sf::RenderTarget& target,
		const sf::Vector2f& v1,
		const sf::Vector2f& v2,
		const sf::Color& color
	);

	void draw_line(
		CanvasWidget* canvas,
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

	void draw_line(
		CanvasWidget* canvas,
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
		CanvasWidget* canvas,
		const sf::FloatRect& bounds,
		const sf::Color& color
	);

	void draw_wire_rect(
		sf::RenderTarget& target,
		const sf::FloatRect& bounds,
		const sf::Color& color,
		const sf::Transform& transform
	);

	void draw_wire_rect(
		CanvasWidget* canvas,
		const sf::FloatRect& bounds,
		const sf::Color& color,
		const sf::Transform& transform
	);

}
