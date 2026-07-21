#pragma once

#include <SFML/Graphics.hpp>

namespace fw {

	class Font {
	public:
		Font();
		explicit Font(const sf::Font& font, bool double_render = false, bool smooth = false);
		explicit Font(const std::string& filename, bool double_render = false, bool smooth = false);
		bool isLoaded() const;
		bool isDoubleRendered() const;
		const sf::Font& getSfmlFont() const;

	private:
		bool loaded = false;
		sf::Font font;
		bool double_render = false;
	};

}
