#pragma once

#include <glvx/vertex_array.h>
#include <glvx/transformable.h>
#include <glvx/render_target.h>
#include <glvx/render_states.h>
#include <glvx/color.h>
#include <glvx/float_rect.h>
#include <glvx/transform.h>
#include <glvx/matrix.h>
#include <glvx/text.h>
#include "logger/logger.h"
#include "common/utils.h"

extern glvx::Text vertex_text;

struct CutInfo {
	size_t from;
	size_t to;
	glvx::Vector2f pos_from;
	glvx::Vector2f pos_to;
	bool to_concave;
	bool green_zone;
	bool has_reciprocal;
	bool green_reciprocal;
	float sqr_dist;
	float angle_diff;
	float angle_diff_sum;
	float score;

	enum CutType {
		GG,
		GY,
		YY,
		G,
		Y,
		Unknown,
	};
	bool isGG() const;
	bool isGY() const;
	bool isYY() const;
	bool isG() const;
	bool isY() const;
	CutType getType() const;
	std::string toStr() const;
};

class SplittablePolygon : public glvx::Drawable, public glvx::Transformable {
public:
	enum BestCutCriterion {
		MIN_DIST,
		MAX_DIST,
		MIN_ANGLE,
	};
	bool draw_varray = false;

	SplittablePolygon();
	SplittablePolygon(size_t count);
	SplittablePolygon(const glvx::VertexArray& varray);
	size_t getPointCount() const;
	glvx::Vector2f getPoint(size_t index) const;
	glvx::Vector2f getLocalCenter() const;
	glvx::Vector2f getGlobalCenter() const;
	glvx::FloatRect getLocalBounds() const;
	glvx::FloatRect getGlobalBounds() const;
	glvx::Color getFillColor() const;
	std::vector<SplittablePolygon> getConvexPolygons() const;
	glvx::Transform getParentGlobalTransform() const;
	glvx::Transform getGlobalTransform() const;
	bool isConvex() const;
	void setPoint(size_t index, const glvx::Vector2f& point);
	void setLineColor(const glvx::Color& color);
	void setFillColor(const glvx::Color& color);
	void calcPotentialCuts(bool consider_convex_vertices);
	size_t getPotentialCutsCount() const;
	void drawPotentialCuts(glvx::RenderTarget& target);
	CutInfo getBestCut(BestCutCriterion criterion) const;
	std::vector<SplittablePolygon> getCutPolygons(const CutInfo& cut) const;
	std::vector<SplittablePolygon> cutWithBestCut(bool cut_convex);
	std::vector<SplittablePolygon> cutIntoConvex(size_t max_vertices = 0);
	void resetVarray(size_t vertex_count);
	void recenter();
	void recut();
	static SplittablePolygon createRect(glvx::Vector2f size);
	glvx::Transform getTransform() const override;
	const glvx::VertexBuffer& getVertexBuffer() const override;
private:
	glvx::VertexArray varray;
	glvx::VertexArray triangle_fan;
	std::vector<SplittablePolygon> convex_polygons;
	glvx::VertexArray cuts_varray;
	SplittablePolygon* parent = nullptr;
	glvx::Color line_color;
	glvx::Color fill_color;
	std::vector<CutInfo> potential_cuts;
	bool cuts_valid = false;
	bool is_convex = false;

	void render(const glvx::Matrix4& view, const glvx::Matrix4& projection, const glvx::RenderStates& states) const override;
	void drawGeometry(const glvx::Matrix4& view, const glvx::Matrix4& projection, const glvx::RenderStates& states) const;
	bool isConvexVertex(size_t index) const;
	bool intersectsEdge(const glvx::Vector2f& v1, const glvx::Vector2f& v2, size_t& intersect) const;
	size_t indexLoop(ptrdiff_t index) const;
	void createCutsVarray();
	void setCutsValid(bool value);
};
