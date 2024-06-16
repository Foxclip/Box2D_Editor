#include "editor/simulation/shapes.h"
#include "common/utils.h"
#include <numbers>

LineStripShape::LineStripShape() { }

LineStripShape::LineStripShape(sf::VertexArray& varray) {
	this->varray = varray;
}

void LineStripShape::setLineColor(sf::Color color) {
	for (size_t i = 0; i < varray.getVertexCount(); i++) {
		varray[i].color = color;
	}
	line_color = color;
}

void LineStripShape::drawMask(sf::RenderTarget& mask, sf::RenderStates states) {
	states.transform *= getTransform();
	sf::Color orig_color = line_color;
	setLineColor(sf::Color::White);
	mask.draw(varray, states);
	setLineColor(orig_color);
}

void LineStripShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(varray, states);
}

CircleNotchShape::CircleNotchShape(float radius, size_t point_count, size_t notch_segment_count) {
	varray_circle = sf::VertexArray(sf::TriangleFan, point_count + 1);
	varray_notch = sf::VertexArray(sf::TriangleFan, notch_segment_count + 2);
	float segment_angle = (float)(2 * std::numbers::pi / (float)point_count);
	float notch_angle = segment_angle * notch_segment_count;
	float angle_offset = -notch_angle / 2.0f;
	varray_circle[0] = sf::Vertex(sf::Vector2f(0.0f, 0.0f));
	for (size_t i = 0; i < point_count; i++) {
		sf::Vector2f pos = utils::get_circle_vertex<sf::Vector2f>(i, point_count, radius, angle_offset);
		varray_circle[i] = sf::Vertex(pos);
	}
	varray_notch[0] = sf::Vertex(sf::Vector2f(0.0f, 0.0f));
	for (size_t i = 0; i < notch_segment_count + 1; i++) {
		sf::Vector2f pos = utils::get_circle_vertex<sf::Vector2f>(i, point_count, radius, angle_offset);
		varray_notch[i] = sf::Vertex(pos);
	}
}

const sf::Color& CircleNotchShape::getCircleColor() const {
	return circle_color;
}

const sf::Color& CircleNotchShape::getNotchColor() const {
	return notch_color;
}

void CircleNotchShape::setCircleColor(const sf::Color& color) {
	for (size_t i = 0; i < varray_circle.getVertexCount(); i++) {
		varray_circle[i].color = color;
	}
	circle_color = color;
}

void CircleNotchShape::setNotchColor(const sf::Color& color) {
	for (size_t i = 0; i < varray_notch.getVertexCount(); i++) {
		varray_notch[i].color = color;
	}
	notch_color = color;
}

void CircleNotchShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(varray_circle, states);
	target.draw(varray_notch, states);
}

void CircleNotchShape::drawMask(sf::RenderTarget& mask, sf::RenderStates states) {
	states.transform *= getTransform();
	sf::Color orig_color = circle_color;
	setCircleColor(sf::Color::White);
	mask.draw(varray_circle, states);
	setCircleColor(orig_color);
}
