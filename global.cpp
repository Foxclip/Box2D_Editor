#include "global.h"

sf::RenderWindow window;
sf::View window_view;
sf::View world_view;
sf::View ui_view;
sf::RenderTexture world_texture;
sf::RenderTexture ui_texture;
sf::RenderTexture selection_mask;
sf::VertexArray line_primitive = sf::VertexArray(sf::Lines, 2);

sf::Vector2f sfScreenToWorld(const sf::Vector2i& screen_pos) {
	sf::Vector2f pos = window.mapPixelToCoords(screen_pos, world_view);
	return pos;
}

sf::Vector2f world_to_screenf(sf::Vector2f world_pos) {
	sf::Vector2i pos = window.mapCoordsToPixel(world_pos, world_view);
	return utils::to2f(pos);
}

void drawLine(sf::RenderTarget& target, const sf::Vector2f& v1, const sf::Vector2f& v2, const sf::Color& color, const sf::Transform& transform) {
	line_primitive[0].position = transform.transformPoint(v1);
	line_primitive[0].color = color;
	line_primitive[1].position = transform.transformPoint(v2);
	line_primitive[1].color = color;
	target.draw(line_primitive);
}
