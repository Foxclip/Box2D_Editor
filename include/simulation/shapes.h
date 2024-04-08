#pragma once

#include <SFML/Graphics.hpp>

class LineStripShape : public sf::Drawable, public sf::Transformable {
public:
	explicit LineStripShape();
	explicit LineStripShape(sf::VertexArray& varray);
	void setLineColor(sf::Color color);
	sf::VertexArray varray;
	void drawMask(sf::RenderTarget& mask, sf::RenderStates states = sf::RenderStates::Default);
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	sf::Color line_color;
};

class CircleNotchShape : public sf::Drawable, public sf::Transformable {
public:
	explicit CircleNotchShape(float radius, size_t point_count, size_t notch_segment_count);
	const sf::Color& getCircleColor() const;
	const sf::Color& getNotchColor() const;
	void setCircleColor(const sf::Color& color);
	void setNotchColor(const sf::Color& color);
	void drawMask(sf::RenderTarget& mask, sf::RenderStates states = sf::RenderStates::Default);
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	sf::VertexArray varray_circle;
	sf::VertexArray varray_notch;
	sf::Color circle_color;
	sf::Color notch_color;
};
