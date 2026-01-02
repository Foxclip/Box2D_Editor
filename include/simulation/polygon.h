#pragma once

#include <SFML/Graphics.hpp>
#include "logger/logger.h"
#include "common/utils.h"

extern sf::Text vertex_text;

struct CutInfo {
	size_t from;
	size_t to;
	sf::Vector2f pos_from;
	sf::Vector2f pos_to;
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

class SplittablePolygon : public sf::Drawable, public sf::Transformable {
public:
	enum BestCutCriterion {
		MIN_DIST,
		MAX_DIST,
		MIN_ANGLE,
	};
	bool draw_varray = false;

	SplittablePolygon();
	SplittablePolygon(size_t count);
	SplittablePolygon(const sf::VertexArray& varray);
	size_t getPointCount() const;
	sf::Vector2f getPoint(size_t index) const;
	sf::Vector2f getLocalCenter() const;
	sf::Vector2f getGlobalCenter() const;
	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;
	sf::Color getFillColor() const;
	std::vector<SplittablePolygon> getConvexPolygons() const;
	sf::Transform getParentGlobalTransform() const;
	sf::Transform getGlobalTransform() const;
	bool isConvex() const;
	void setPoint(size_t index, const sf::Vector2f& point);
	void setLineColor(const sf::Color& color);
	void setFillColor(const sf::Color& color);
	void calcPotentialCuts(bool consider_convex_vertices);
	size_t getPotentialCutsCount() const;
	void drawPotentialCuts(sf::RenderTarget& target);
	CutInfo getBestCut(BestCutCriterion criterion) const;
	std::vector<SplittablePolygon> getCutPolygons(const CutInfo& cut) const;
	std::vector<SplittablePolygon> cutWithBestCut(bool cut_convex);
	std::vector<SplittablePolygon> cutIntoConvex(size_t max_vertices = 0);
	void resetVarray(size_t vertex_count);
	void recenter();
	void recut();
	static SplittablePolygon createRect(sf::Vector2f size);
private:
	sf::VertexArray varray;
	sf::VertexArray triangle_fan;
	std::vector<SplittablePolygon> convex_polygons;
	sf::VertexArray cuts_varray;
	SplittablePolygon* parent = nullptr;
	sf::Color line_color;
	sf::Color fill_color;
	std::vector<CutInfo> potential_cuts;
	bool cuts_valid = false;
	bool is_convex = false;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	bool isConvexVertex(size_t index) const;
	bool intersectsEdge(const sf::Vector2f& v1, const sf::Vector2f& v2, size_t& intersect) const;
	size_t indexLoop(ptrdiff_t index) const;
	void createCutsVarray();
	void setCutsValid(bool value);
};
