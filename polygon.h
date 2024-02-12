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
	void setPoint(size_t index, const sf::Vector2f& point);
	void setLineColor(const sf::Color& color);
	void drawIndices(sf::RenderTarget& target, const sf::Color& color) const;
	void calcPotentialCuts();
	size_t getPotentialCutsCount() const;
	void drawPotentialCuts(sf::RenderTarget& target);
	CutInfo getBestCut() const;
	std::vector<PolygonObject> getCutPolygons(const CutInfo& cut) const;
	std::vector<PolygonObject> cutWithBestCut();
	static PolygonObject createRect(sf::Vector2f size);
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	bool isConvexVertex(size_t index);
	bool intersectsEdge(const sf::Vector2f& v1, const sf::Vector2f& v2, size_t& intersect);
	size_t indexLoop(ptrdiff_t index);
	void createCutsVarray();
	void setPotentialCutsValid(bool value);
	sf::VertexArray varray;
	sf::VertexArray cuts_varray;
	sf::Color line_color;
	std::vector<CutInfo> potential_cuts;
	bool potential_cuts_valid = false;
};
