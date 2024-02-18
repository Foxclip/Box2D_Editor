#pragma once

#include <SFML/Graphics.hpp>
#include "global.h"
#include "logger.h"

extern sf::Text vertex_text;

Logger& operator<<(Logger& logger, const sf::Vector2f& value);

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

class PolygonObject : public sf::Drawable, public sf::Transformable {
public:
	PolygonObject();
	PolygonObject(size_t count);
	PolygonObject(const sf::VertexArray& varray);
	size_t getPointCount() const;
	sf::Vector2f getPoint(size_t index) const;
	sf::Vector2f getLocalCenter() const;
	sf::Vector2f getGlobalCenter() const;
	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;
	sf::Color getFillColor() const;
	std::vector<PolygonObject> getConvexPolygons() const;
	sf::Transform getParentGlobalTransform() const;
	sf::Transform getGlobalTransform() const;
	bool isConvex() const;
	void setPoint(size_t index, const sf::Vector2f& point);
	void setLineColor(const sf::Color& color);
	void setFillColor(const sf::Color& color);
	void drawIndices(
		sf::RenderTarget& target,
		const sf::Color& color,
		unsigned int size,
		bool include_convex = false
	) const;
	void calcPotentialCuts();
	size_t getPotentialCutsCount() const;
	void drawPotentialCuts(sf::RenderTarget& target);
	CutInfo getBestCut() const;
	std::vector<PolygonObject> getCutPolygons(const CutInfo& cut) const;
	std::vector<PolygonObject> cutWithBestCut();
	std::vector<PolygonObject> cutIntoConvex();
	void resetVarray(size_t vertex_count);
	void recenter();
	void recut();
	static PolygonObject createRect(sf::Vector2f size);
private:
	sf::VertexArray varray;
	sf::VertexArray triangle_fan;
	std::vector<PolygonObject> convex_polygons;
	sf::VertexArray cuts_varray;
	PolygonObject* parent = nullptr;
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
