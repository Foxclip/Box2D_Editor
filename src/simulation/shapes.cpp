#include "simulation/shapes.h"
#include "common/utils.h"
#include <numbers>

LineStripShape::LineStripShape() { }

LineStripShape::LineStripShape(glvx::VertexArray& varray) {
	this->varray = varray;
}

void LineStripShape::setLineColor(glvx::Color color) {
	for (size_t i = 0; i < varray.getVertexCount(); i++) {
		varray[i].color = color;
	}
	line_color = color;
}

void LineStripShape::drawMask(glvx::RenderTarget& mask, const glvx::RenderStates& states) {
	glvx::RenderStates states_copy = states;
	states_copy.transform *= getTransform();
	glvx::Color orig_color = line_color;
	setLineColor(glvx::Color::White);
	mask.draw(varray, states_copy);
	setLineColor(orig_color);
}

glvx::Transform LineStripShape::getTransform() const {
	return Transformable::getTransform();
}

const glvx::VertexBuffer& LineStripShape::getVertexBuffer() const {
	return varray.getVertexBuffer();
}

void LineStripShape::render(const glvx::Matrix4& view, const glvx::Matrix4& projection, const glvx::RenderStates& states) const {
	glvx::RenderStates states_copy = states;
	states_copy.transform *= getTransform();
	varray.render(view, projection, states_copy);
}

CircleNotchShape::CircleNotchShape(float radius, size_t point_count, size_t notch_segment_count) {
	varray_circle = glvx::VertexArray(glvx::PrimitiveType::TriangleFan, point_count + 1);
	varray_notch = glvx::VertexArray(glvx::PrimitiveType::TriangleFan, notch_segment_count + 2);
	float segment_angle = (float)(2 * std::numbers::pi / (float)point_count);
	float notch_angle = segment_angle * notch_segment_count;
	float angle_offset = -notch_angle / 2.0f;
	auto make_vertex = [](glvx::Vector2f pos) {
		glvx::Vertex vertex;
		vertex.position = pos;
		vertex.color = glvx::Color::White;
		return vertex;
	};
	varray_circle[0] = make_vertex(glvx::Vector2f(0.0f, 0.0f));
	for (size_t i = 0; i < point_count; i++) {
		glvx::Vector2f pos = utils::get_circle_vertex<glvx::Vector2f>(i, point_count, radius, angle_offset);
		varray_circle[i] = make_vertex(pos);
	}
	varray_notch[0] = make_vertex(glvx::Vector2f(0.0f, 0.0f));
	for (size_t i = 0; i < notch_segment_count + 1; i++) {
		glvx::Vector2f pos = utils::get_circle_vertex<glvx::Vector2f>(i, point_count, radius, angle_offset);
		varray_notch[i] = make_vertex(pos);
	}
}

const glvx::Color& CircleNotchShape::getCircleColor() const {
	return circle_color;
}

const glvx::Color& CircleNotchShape::getNotchColor() const {
	return notch_color;
}

void CircleNotchShape::setCircleColor(const glvx::Color& color) {
	for (size_t i = 0; i < varray_circle.getVertexCount(); i++) {
		varray_circle[i].color = color;
	}
	circle_color = color;
}

void CircleNotchShape::setNotchColor(const glvx::Color& color) {
	for (size_t i = 0; i < varray_notch.getVertexCount(); i++) {
		varray_notch[i].color = color;
	}
	notch_color = color;
}

glvx::Transform CircleNotchShape::getTransform() const {
	return Transformable::getTransform();
}

const glvx::VertexBuffer& CircleNotchShape::getVertexBuffer() const {
	return varray_circle.getVertexBuffer();
}

void CircleNotchShape::render(const glvx::Matrix4& view, const glvx::Matrix4& projection, const glvx::RenderStates& states) const {
	glvx::RenderStates states_copy = states;
	states_copy.transform *= getTransform();
	varray_circle.render(view, projection, states_copy);
	varray_notch.render(view, projection, states_copy);
}

void CircleNotchShape::drawMask(glvx::RenderTarget& mask, const glvx::RenderStates& states) {
	glvx::RenderStates states_copy = states;
	states_copy.transform *= getTransform();
	glvx::Color orig_color = circle_color;
	setCircleColor(glvx::Color::White);
	mask.draw(varray_circle, states_copy);
	setCircleColor(orig_color);
}
