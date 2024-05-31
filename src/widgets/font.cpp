#include "widgets/font.h"
#include <widgets/widgets_common.h>

namespace fw {

	Font::Font() { }

	Font::Font(const sf::Font& font, bool double_render, bool smooth) {
		this->font = font;
		this->double_render = double_render;
		this->font.setSmooth(smooth);
		loaded = true;
	}

	Font::Font(const std::string& filename, bool double_render, bool smooth) {
		if (!font.loadFromFile(filename)) {
			throw std::runtime_error("Font loading error (" + filename + ")");
		}
		this->double_render = double_render;
		this->font.setSmooth(smooth);
		loaded = true;
	}

	bool Font::isLoaded() const {
		return loaded;
	}

	bool Font::isDoubleRendered() const {
		return double_render;
	}

	const sf::Font& Font::getSfmlFont() const {
		wAssert(isLoaded());
		return font;
	}

}
