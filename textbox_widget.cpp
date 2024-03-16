#include "textbox_widget.h"
#include "widget_list.h"

TextBoxWidget::TextBoxWidget(WidgetList& widget_list) : RectangleWidget(widget_list) {
	setSize(DEFAULT_SIZE);
	setName("textbox");
	setClipChildren(true);
	text_widget = widget_list.createWidget<TextWidget>();
	text_widget->setFillColor(text_color);
	text_widget->setParentAnchor(CENTER_LEFT);
	text_widget->setAnchorOffset(TEXT_VIEW_ZERO_POS);
	text_widget->setOrigin(CENTER_LEFT);
	text_widget->setParent(this);
	cursor_widget = widget_list.createWidget<RectangleWidget>();
	cursor_widget->setVisible(false);
	cursor_widget->setFillColor(editor_text_color);
	cursor_widget->setSize(sf::Vector2f(1.0f, text_widget->getCharacterSize()));
	cursor_widget->setParent(this);
	cursor_widget->setName("cursor");
	setValueSilent("Text");
	cursor_pos = getStringSize();
	updateColors();
}

bool TextBoxWidget::isFocusable() const {
	return true;
}

const sf::Color& TextBoxWidget::getFillColor() const {
	return background_color;
}

const sf::Color& TextBoxWidget::getHighlightColor() const {
	return highlight_color;
}

const sf::Color& TextBoxWidget::getTextColor() const {
	return text_color;
}

const sf::Color& TextBoxWidget::getEditorColor() const {
	return editor_color;
}

const sf::Color& TextBoxWidget::getEditorTextColor() const {
	return editor_text_color;
}

const sf::Color& TextBoxWidget::getFailFillColor() const {
	return fail_background_color;
}

const sf::Color& TextBoxWidget::getEditFailFillColor() const {
	return editor_fail_background_color;
}

const sf::Font* TextBoxWidget::getFont() const {
	return text_widget->getFont();
}

unsigned int TextBoxWidget::getCharacterSize() const {
	return text_widget->getCharacterSize();
}

const sf::String& TextBoxWidget::getValue() const {
	return text_widget->getString();
}

bool TextBoxWidget::isValidValue() const {
	bool result;
	if (type == TextBoxType::TEXT) {
		result = true;
	} else if (type == TextBoxType::INTEGER) {
		long long number;
		result = utils::parseLL(getValue(), number);
	} else if (type == TextBoxType::FLOAT) {
		float number;
		result = utils::parseFloat(getValue(), number);
	} else {
		assert(false, "Unknown TextBoxType");
	}
	return result;
}

size_t TextBoxWidget::getStringSize() const {
	return text_widget->getStringSize();
}

TextBoxWidget::TextBoxType TextBoxWidget::getType() const {
	return type;
}

size_t TextBoxWidget::getCursorPos() const {
	return cursor_pos;
}

bool TextBoxWidget::isEditMode() const {
	return edit_mode;
}

sf::Vector2f TextBoxWidget::getLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
	sf::Vector2f local_pos = text_widget->getParentLocalCharPos(index, top_aligned, with_kerning);
	return local_pos;
}

sf::Vector2f TextBoxWidget::getGlobalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
	return text_widget->getGlobalCharPos(index, top_aligned, with_kerning);
}

void TextBoxWidget::setFillColor(const sf::Color& color) {
	this->background_color = color;
	updateColors();
}

void TextBoxWidget::setHighlightColor(const sf::Color& color) {
	this->highlight_color = color;
	updateColors();
}

void TextBoxWidget::setTextColor(const sf::Color& color) {
	this->text_color = color;
	updateColors();
}

void TextBoxWidget::setEditorColor(const sf::Color& color) {
	this->editor_color = color;
	updateColors();
}

void TextBoxWidget::setEditorTextColor(const sf::Color& color) {
	this->editor_text_color = color;
	updateColors();
}

void TextBoxWidget::setFailFillColor(const sf::Color& color) {
	this->fail_background_color = color;
}

void TextBoxWidget::setEditFailFillColor(const sf::Color& color) {
	this->editor_fail_background_color = color;
}

void TextBoxWidget::setFont(const sf::Font& font) {
	text_widget->setFont(font);
}

void TextBoxWidget::setCharacterSize(unsigned int size) {
	text_widget->setCharacterSize(size);
	cursor_widget->setSize(sf::Vector2f(1.0f, size));
}

void TextBoxWidget::setValueSilent(const sf::String& value) {
	text_widget->setString(value);
	setCursorPos(cursor_pos);
	internalOnValueChanged(value);
}

void TextBoxWidget::setValue(const sf::String& value) {
	setValueSilent(value);
	OnValueChanged(value);
}

void TextBoxWidget::setType(TextBoxType type) {
	this->type = type;
	updateValid();
}

void TextBoxWidget::setCursorPos(size_t pos) {
	pos = std::clamp(pos, (size_t)0, getStringSize());
	this->cursor_pos = pos;
	float char_pos = getLocalCharPos(pos, true, true).x;
	float cursor_visual_pos = char_pos + CURSOR_OFFSET.x;
	float left_bound = CURSOR_MOVE_MARGIN;
	float right_bound = getWidth() - CURSOR_MOVE_MARGIN;
	float offset_left = cursor_visual_pos - left_bound;
	float offset_right = cursor_visual_pos - right_bound;
	if (offset_left < 0) {
		text_view_pos -= sf::Vector2f(offset_left, 0.0f);
	}
	if (offset_right > 0) {
		text_view_pos -= sf::Vector2f(offset_right, 0.0f);
	}
}

void TextBoxWidget::typeChar(sf::Uint32 code) {
	if (type == TextBoxType::INTEGER) {
		if (VALID_INTEGER_CHARS.find(code) == -1) {
			return;
		}
	} else if (type == TextBoxType::FLOAT) {
		if (VALID_FLOAT_CHARS.find(code) == -1) {
			return;
		}
	}
	insert(cursor_pos, sf::String(code));
	setCursorPos(cursor_pos + 1);
}

void TextBoxWidget::insert(size_t pos, const sf::String& str) {
	insertSilent(pos, str);
	internalOnValueChanged(getValue());
	OnValueChanged(getValue());
}

void TextBoxWidget::erase(size_t index_first, size_t count) {
	eraseSilent(index_first, count);
	internalOnValueChanged(getValue());
	OnValueChanged(getValue());
}

void TextBoxWidget::processKeyboardEvent(const sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
			case sf::Keyboard::Escape:
				if (edit_mode) {
					internalOnCancel();
					OnCancel();
				}
				removeFocus();
				break;
			case sf::Keyboard::Enter:
				if (edit_mode) {
					internalOnConfirm(getValue());
					OnConfirm(getValue());
				}
				setEditMode(!edit_mode);
				break;
		}
		if (edit_mode) {
			switch (event.key.code) {
				case sf::Keyboard::Left:
					if (cursor_pos > 0) {
						setCursorPos(cursor_pos - 1);
					}
					break;
				case sf::Keyboard::Right:
					if (cursor_pos < getStringSize()) {
						setCursorPos(cursor_pos + 1);
					}
					break;
				case sf::Keyboard::Home:
					setCursorPos(0);
					break;
				case sf::Keyboard::End:
					setCursorPos(getStringSize());
					break;
				case sf::Keyboard::Delete:
					if (cursor_pos < getStringSize()) {
						erase(cursor_pos, 1);
					}
					break;
			}
		}
	} else if (event.type == sf::Event::TextEntered) {
		if (edit_mode) {
			sf::Uint32 code = event.text.unicode;
			switch (code) {
				case '\n':
				case '\r':
					break;
				case '\b':
					if (cursor_pos > 0) {
						erase(cursor_pos - 1, 1);
						setCursorPos(cursor_pos - 1);
					}
					break;
				default:
					typeChar(code);
					break;
			}
		}
	}
}

void TextBoxWidget::update() {
	Widget::update();
	text_widget->setAnchorOffset(TEXT_VIEW_ZERO_POS + text_view_pos);
	sf::Vector2f char_pos = getLocalCharPos(cursor_pos, true, true);
	cursor_widget->setPosition(char_pos + CURSOR_OFFSET);
}

void TextBoxWidget::updateColors() {
	sf::Color rect_col;
	sf::Color text_col;
	if (edit_mode) {
		if (fail_state) {
			rect_col = editor_fail_background_color;
			text_col = editor_text_color;
		} else {
			rect_col = editor_color;
			text_col = editor_text_color;
		}
	} else {
		if (highlighted) {
			if (fail_state) {
				rect_col = highlight_color;
			} else {
				rect_col = highlight_color;
			}
		} else {
			if (fail_state) {
				rect_col = fail_background_color;
			} else {
				rect_col = background_color;
			}
		}
		text_col = text_color;
	}
	RectangleWidget::setFillColor(rect_col);
	text_widget->setFillColor(text_col);
	cursor_widget->setFillColor(text_col);
}

void TextBoxWidget::internalOnClick(const sf::Vector2f& pos) {
	setEditMode(true);
	sf::Vector2f local_pos = text_widget->toLocal(pos);
	size_t char_left = text_widget->getCharAt(local_pos);
	if (char_left >= getStringSize()) {
		setCursorPos(getStringSize());
	} else {
		size_t char_right = char_left + 1;
		sf::Vector2f pos_left = getGlobalCharPos(char_left, true, true);
		sf::Vector2f pos_right = getGlobalCharPos(char_right, true, true);
		float dist_left = abs(pos.x - pos_left.x);
		float dist_right = abs(pos.x - pos_right.x);
		sf::Vector2f cursor_visual_pos;
		if (dist_left <= dist_right) {
			setCursorPos(char_left);
		} else {
			setCursorPos(char_right);
		}
	}
}

void TextBoxWidget::internalOnEditModeToggle(bool value) {
	cursor_widget->setVisible(value);
	if (!value) {
		setCursorPos(0);
	}
	updateColors();
}

void TextBoxWidget::internalOnFocused() {
	setEditMode(true);
}

void TextBoxWidget::internalOnFocusLost() {
	setEditMode(false);
}

void TextBoxWidget::internalOnMouseEnter(const sf::Vector2f& pos) {
	highlighted = true;
	if (!edit_mode) {
		updateColors();
	}
}

void TextBoxWidget::internalOnMouseExit(const sf::Vector2f& pos) {
	highlighted = false;
	if (!edit_mode) {
		updateColors();
	}
}

void TextBoxWidget::internalOnValueChanged(const sf::String& new_value) {
	updateValid();
}

void TextBoxWidget::internalOnConfirm(const sf::String& value) { }

void TextBoxWidget::internalOnCancel() { }

void TextBoxWidget::updateValid() {
	fail_state = !isValidValue();
	updateColors();
}

void TextBoxWidget::setEditMode(bool value) {
	if (edit_mode == value) {
		return;
	}
	edit_mode = value;
	internalOnEditModeToggle(value);
	OnEditModeToggle(value);
}

void TextBoxWidget::insertSilent(size_t pos, const sf::String& str) {
	text_widget->insert(pos, str);
}

void TextBoxWidget::eraseSilent(size_t index_first, size_t count) {
	text_widget->erase(index_first, count);
}
