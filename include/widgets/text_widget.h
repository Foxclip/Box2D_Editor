#pragma once

#include "widget.h"

namespace fw {

	class WidgetList;

	class TextWidget : public Widget {
	public:
		TextWidget(WidgetList& widget_list);
		bool isVisualPositionQuantized() const override;
		sf::FloatRect getLocalBounds() const override;
		sf::FloatRect getVisualLocalBounds() const override;
		sf::Vector2f getRenderPositionOffset() const override;
		const fw::Font& getFont() const;
		size_t getStringSize() const;
		unsigned int getCharacterSize() const;
		const sf::Color& getFillColor() const override;
		const sf::String& getString() const;
		float getKerning(size_t index) const;
		sf::Vector2f getLocalCharPos(size_t index, bool top_aligned = true, bool with_kerning = true) const;
		sf::Vector2f getParentLocalCharPos(size_t index, bool top_aligned = true, bool with_kerning = true) const;
		sf::Vector2f getGlobalCharPos(size_t index, bool top_aligned = true, bool with_kerning = true) const;
		size_t getCharAt(const sf::Vector2f& pos) const;
		void setFont(const fw::Font& font);
		void setCharacterSize(unsigned int size);
		void setFillColor(const sf::Color& color) override;
		void setAdjustLocalBounds(bool value);
		void setString(const sf::String& string);
		void insert(size_t pos, const sf::String& str);
		void erase(size_t index_first, size_t count = 1);

	protected:
		bool adjust_local_bounds = true;

		sf::Drawable* getDrawable() override;
		const sf::Drawable* getDrawable() const override;
		sf::Transformable* getTransformable() override;
		const sf::Transformable* getTransformable() const override;
		void setSizeInternal(float width, float height) override;
		void internalUpdate() override;

	private:
		sf::Text text;
		fw::Font font;
	};

}
