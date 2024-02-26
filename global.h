#pragma once

#include <SFML/Graphics.hpp>
#include "utils.h"

extern sf::RenderWindow window;
extern sf::View window_view;
extern sf::View world_view;
extern sf::View ui_view;
extern sf::RenderTexture world_texture;
extern sf::RenderTexture ui_texture;
extern sf::RenderTexture selection_mask;
extern sf::VertexArray line_primitive;

sf::Vector2f sfScreenToWorld(const sf::Vector2i& screen_pos);
sf::Vector2f world_to_screen(sf::Vector2f world_pos);
void drawLine(
	sf::RenderTarget& target,
	const sf::Vector2f& v1,
	const sf::Vector2f& v2,
	const sf::Color& color,
	const sf::Transform& transform = sf::Transform::Identity
);