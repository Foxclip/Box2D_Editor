#include "widgets/text_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	TextWidget::TextWidget(WidgetList& widget_list) : Widget(widget_list) {
		setName("text");
	}

	bool TextWidget::isVisualPositionQuantized() const {
		return true;
	}

	sf::FloatRect TextWidget::getLocalBounds() const {
		sf::FloatRect result;
		if (adjust_local_bounds) {
			sf::FloatRect local_bounds = text.getLocalBounds();
			sf::Vector2f size(local_bounds.left + local_bounds.width, (float)text.getCharacterSize());
			result = sf::FloatRect(sf::Vector2f(), size);
		} else {
			result = getVisualLocalBounds();
		}
		return result;
	}

	sf::FloatRect TextWidget::getVisualLocalBounds() const {
		sf::FloatRect local_bounds = text.getLocalBounds();
		sf::Vector2f offset = getRenderPositionOffset();
		sf::Vector2f offset_pos = local_bounds.getPosition() + offset;
		sf::FloatRect offset_bounds = sf::FloatRect(offset_pos, local_bounds.getSize());
		return offset_bounds;
	}

	sf::Vector2f TextWidget::getRenderPositionOffset() const {
		sf::Vector2f offset(0.0f, 0.0f);
		if (!adjust_local_bounds) {
			sf::FloatRect bounds = text.getLocalBounds();
			offset = sf::Vector2f(0.0f, bounds.getPosition().y);
		}
		return -offset;
	}

	const sf::Font* TextWidget::getFont() const {
		return text.getFont();
	}

	size_t TextWidget::getStringSize() const {
		return text.getString().getSize();
	}

	unsigned int TextWidget::getCharacterSize() const {
		return text.getCharacterSize();
	}

	const sf::Color& TextWidget::getFillColor() const {
		return text.getFillColor();
	}

	const sf::String& TextWidget::getString() const {
		return text.getString();
	}

	float TextWidget::getKerning(size_t index) const {
		if (index == 0 || index > getStringSize()) {
			return 0.0f;
		}
		sf::Uint32 left_char = getString()[index - 1];
		sf::Uint32 right_char = getString()[index];
		float kerning = getFont()->getKerning(left_char, right_char, getCharacterSize());
		return kerning;
	}

	sf::Vector2f TextWidget::getLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
		wAssert(getFont());
		// Transformable's position is not used,
		// so findCharacterPos returns position in local coordinates
		sf::Vector2f local_char_pos = text.findCharacterPos(index);
		if (with_kerning) {
			local_char_pos.x += getKerning(index);
		}
		if (top_aligned) {
			return sf::Vector2f(local_char_pos.x, 0.0f);
		} else {
			return local_char_pos;
		}
	}

	sf::Vector2f TextWidget::getParentLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
		sf::Vector2f local_char_pos = getLocalCharPos(index, top_aligned, with_kerning);
		sf::Vector2f parent_local_char_pos = getTransform() * local_char_pos;
		return parent_local_char_pos;
	}

	sf::Vector2f TextWidget::getGlobalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
		sf::Vector2f local_pos = getLocalCharPos(index, top_aligned, with_kerning);
		sf::Vector2f global_pos = toGlobal(local_pos);
		return global_pos;
	}

	size_t TextWidget::getCharAt(const sf::Vector2f& pos) const {
		ptrdiff_t result = 0;
		for (size_t i = 0; i <= getStringSize(); i++) {
			sf::Vector2f local_char_pos = getLocalCharPos(i, true, true);
			if (pos.x >= local_char_pos.x) {
				result = i;
			}
		}
		return result;
	}

	void TextWidget::setFont(const sf::Font& font) {
		text.setFont(font);
	}

	void TextWidget::setCharacterSize(unsigned int size) {
		text.setCharacterSize(size);
	}

	void TextWidget::setFillColor(const sf::Color& color) {
		text.setFillColor(color);
	}

	void TextWidget::setAdjustLocalBounds(bool value) {
		adjust_local_bounds = value;
	}

	void TextWidget::setString(const sf::String& string) {
		text.setString(string);
	}

	void TextWidget::insert(size_t pos, const sf::String& str) {
		wAssert(pos >= 0 && pos <= getStringSize());
		sf::String new_str = text.getString();
		new_str.insert(pos, str);
		text.setString(new_str);
	}

	void TextWidget::erase(size_t index_first, size_t count) {
		if (text.getString().isEmpty()) {
			return;
		}
		size_t index_last = index_first + count - 1;
		wAssert(index_first >= 0 && index_last < getStringSize());
		sf::String str = text.getString();
		str.erase(index_first, count);
		text.setString(str);
	}

	sf::Drawable& TextWidget::getDrawable() {
		return text;
	}

	const sf::Drawable& TextWidget::getDrawable() const {
		return text;
	}

	sf::Transformable& TextWidget::getTransformable() {
		return text;
	}

	const sf::Transformable& TextWidget::getTransformable() const {
		return text;
	}

	void TextWidget::internalUpdate() {
		wAssert(getFont(), "Font is not set for " + full_name);
	}

}
