#include "widgets/drawing.h"
#include "widgets/canvas_widget.h"

namespace fw {

	sf::VertexArray line_primitive = sf::VertexArray(sf::Lines, 2);
	sf::VertexArray rect_primitive = sf::VertexArray(sf::TrianglesStrip, 4);

	void draw_line(
		sf::RenderTarget& target,
		const sf::Vector2f& v1,
		const sf::Vector2f& v2,
		const sf::Color& color
	) {
		line_primitive[0].position = quantize_and_offset(v1);
		line_primitive[0].color = color;
		line_primitive[1].position = quantize_and_offset(v2);
		line_primitive[1].color = color;
		target.draw(line_primitive);
	}

	void draw_line(
		CanvasWidget* canvas,
		const sf::Vector2f& v1,
		const sf::Vector2f& v2,
		const sf::Color& color
	) {
		line_primitive[0].position = quantize_and_offset(v1);
		line_primitive[0].color = color;
		line_primitive[1].position = quantize_and_offset(v2);
		line_primitive[1].color = color;
		canvas->draw(line_primitive);
	}

	void draw_line(
		sf::RenderTarget& target,
		const sf::Vector2f& v1,
		const sf::Vector2f& v2,
		const sf::Color& color,
		const sf::Transform& transform
	) {
		line_primitive[0].position = quantize_and_offset(transform.transformPoint(v1));
		line_primitive[0].color = color;
		line_primitive[1].position = quantize_and_offset(transform.transformPoint(v2));
		line_primitive[1].color = color;
		target.draw(line_primitive);
	}

	void draw_line(
		CanvasWidget* canvas,
		const sf::Vector2f& v1,
		const sf::Vector2f& v2,
		const sf::Color& color,
		const sf::Transform& transform
	) {
		line_primitive[0].position = quantize_and_offset(transform.transformPoint(v1));
		line_primitive[0].color = color;
		line_primitive[1].position = quantize_and_offset(transform.transformPoint(v2));
		line_primitive[1].color = color;
		canvas->draw(line_primitive);
	}

	void draw_rect(
		sf::RenderTarget& target,
		const sf::Vector2f& v1,
		const sf::Vector2f& v2,
		const sf::Vector2f& v3,
		const sf::Vector2f& v4,
		const sf::Color& color
	) {
		rect_primitive[0].position = quantize(v1);
		rect_primitive[0].color = color;
		rect_primitive[1].position = quantize(v2);
		rect_primitive[1].color = color;
		rect_primitive[2].position = quantize(v3);
		rect_primitive[2].color = color;
		rect_primitive[3].position = quantize(v4);
		rect_primitive[3].color = color;
		target.draw(rect_primitive);
	}

	void draw_rect(sf::RenderTarget& target,
		const sf::Vector2f& pos,
		const sf::Vector2f& size,
		const sf::Color& color
	) {
		sf::Vector2f v1 = pos;
		sf::Vector2f v2 = sf::Vector2f(pos.x + size.x, pos.y);
		sf::Vector2f v3 = sf::Vector2f(pos.x, pos.y + size.y);
		sf::Vector2f v4 = sf::Vector2f(pos.x + size.x, pos.y + size.y);
		draw_rect(target, v1, v2, v3, v4, color);
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
		CanvasWidget* canvas,
		const sf::FloatRect& bounds,
		const sf::Color& color
	) {
		sf::Vector2f topRight(bounds.left + bounds.width, bounds.top);
		sf::Vector2f topLeft(bounds.left, bounds.top);
		sf::Vector2f bottomLeft(bounds.left, bounds.top + bounds.height);
		sf::Vector2f bottomRight(bounds.left + bounds.width, bounds.top + bounds.height);
		draw_line(canvas, topRight, topLeft, color);
		draw_line(canvas, topLeft, bottomLeft, color);
		draw_line(canvas, bottomLeft, bottomRight, color);
		draw_line(canvas, bottomRight, topRight, color);
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

	void draw_wire_rect(
		CanvasWidget* canvas,
		const sf::FloatRect& bounds,
		const sf::Color& color,
		const sf::Transform& transform
	) {
		sf::Vector2f topRight(bounds.left + bounds.width, bounds.top);
		sf::Vector2f topLeft(bounds.left, bounds.top);
		sf::Vector2f bottomLeft(bounds.left, bounds.top + bounds.height);
		sf::Vector2f bottomRight(bounds.left + bounds.width, bounds.top + bounds.height);
		draw_line(canvas, topRight, topLeft, color, transform);
		draw_line(canvas, topLeft, bottomLeft, color, transform);
		draw_line(canvas, bottomLeft, bottomRight, color, transform);
		draw_line(canvas, bottomRight, topRight, color, transform);
	}

}
