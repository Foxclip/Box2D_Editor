#pragma once

#include <SFML/Graphics.hpp>

// If logical texture size is at least this smaller than physical size,
// texture is reallocated
const float RENDER_TEXTURE_DOWNSIZE_FACTOR = 0.75;
// Once texture grows to this size, it cannot become smaller than this size
const unsigned int RENDER_TEXTURE_NO_DEALLOC_BELOW = 512;

class RenderTexture {
public:
	void create(unsigned int new_width, unsigned int new_height);
	sf::Vector2u getSize() const;
	sf::Vector2u getPhysicalSize() const;
	sf::RenderTexture& getNormal();
	sf::RenderTexture& getPremultiplied();

private:
	unsigned int width = 0;
	unsigned int height = 0;
	sf::RenderTexture render_texture;
	sf::RenderTexture render_texture_premultiplied;

};
