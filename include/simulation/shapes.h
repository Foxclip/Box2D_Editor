#pragma once

#include <glvx/vertex_array.h>
#include <glvx/transformable.h>
#include <glvx/render_target.h>
#include <glvx/render_states.h>
#include <glvx/color.h>
#include <glvx/transform.h>
#include <glvx/matrix.h>

class LineStripShape : public glvx::Drawable, public glvx::Transformable {
public:
	explicit LineStripShape();
	explicit LineStripShape(glvx::VertexArray& varray);
	void setLineColor(glvx::Color color);
	glvx::VertexArray varray;
	void drawMask(glvx::RenderTarget& mask, const glvx::RenderStates& states = glvx::RenderStates());
	glvx::Transform getTransform() const override;
	const glvx::VertexBuffer& getVertexBuffer() const override;
 private:
	void render(const glvx::Matrix4& view, const glvx::Matrix4& projection, const glvx::RenderStates& states) const override;
	glvx::Color line_color;
};

class CircleNotchShape : public glvx::Drawable, public glvx::Transformable {
public:
	explicit CircleNotchShape(float radius, size_t point_count, size_t notch_segment_count);
	const glvx::Color& getCircleColor() const;
	const glvx::Color& getNotchColor() const;
	void setCircleColor(const glvx::Color& color);
	void setNotchColor(const glvx::Color& color);
	void drawMask(glvx::RenderTarget& mask, const glvx::RenderStates& states = glvx::RenderStates());
	glvx::Transform getTransform() const override;
	const glvx::VertexBuffer& getVertexBuffer() const override;
 private:
	void render(const glvx::Matrix4& view, const glvx::Matrix4& projection, const glvx::RenderStates& states) const override;
	glvx::VertexArray varray_circle;
	glvx::VertexArray varray_notch;
	glvx::Color circle_color;
	glvx::Color notch_color;
};
