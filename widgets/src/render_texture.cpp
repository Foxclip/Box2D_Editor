#include "widgets/render_texture.h"
#include "widgets/widgets_common.h"

float MIN_DEALLOC_PO2 = (float)ceil(log2(RENDER_TEXTURE_NO_DEALLOC_BELOW));

void RenderTexture::create(unsigned int new_width, unsigned int new_height) {
	unsigned int width_po2 = (unsigned int)ceil(log2(new_width));
	unsigned int height_po2 = (unsigned int)ceil(log2(new_height));
	unsigned int prev_width = (unsigned int)pow(2, width_po2 - 1);
	unsigned int prev_height = (unsigned int)pow(2, height_po2 - 1);
	unsigned int next_width = (unsigned int)pow(2, width_po2);
	unsigned int next_height = (unsigned int)pow(2, height_po2);
	bool width_oversized = getPhysicalSize().x > next_width;
	bool height_oversized = getPhysicalSize().y > next_height;
	bool width_below_factor = (float)new_width / next_width <= RENDER_TEXTURE_DOWNSIZE_FACTOR;
	bool height_below_factor = (float)new_height / next_height <= RENDER_TEXTURE_DOWNSIZE_FACTOR;
	unsigned int physical_width_po2 = (unsigned int)ceil(log2(getPhysicalSize().x));
	unsigned int physical_height_po2 = (unsigned int)ceil(log2(getPhysicalSize().y));
	unsigned int width_can_dealloc = physical_width_po2 > MIN_DEALLOC_PO2;
	unsigned int height_can_dealloc = physical_height_po2 > MIN_DEALLOC_PO2;
	bool width_dealloc = width_oversized && width_below_factor && width_can_dealloc;
	bool height_dealloc = height_oversized && height_below_factor && height_can_dealloc;
	bool dealloc = width_dealloc || height_dealloc;
	bool reallocation_needed = new_width > getPhysicalSize().x || new_height > getPhysicalSize().y || dealloc;
	if (reallocation_needed) {
		unsigned int new_physical_width = next_width;
		unsigned int new_physical_height = next_height;
		render_texture.create(new_physical_width, new_physical_height);
		render_texture_premultiplied.create(new_physical_width, new_physical_height);
	}
	width = new_width;
	height = new_height;
}

sf::Vector2u RenderTexture::getSize() const {
	return sf::Vector2u(width, height);
}

sf::Vector2u RenderTexture::getPhysicalSize() const {
	return render_texture.getSize();
}

sf::RenderTexture& RenderTexture::getNormal() {
	return render_texture;
}

sf::RenderTexture& RenderTexture::getPremultiplied() {
	return render_texture_premultiplied;
}
