#include "widgets/textbox_widget.h"
#include "widgets/widget_list.h"

TextBoxWidget::TextBoxWidget(WidgetList& widget_list) : RectangleWidget(widget_list) {
	setSize(DEFAULT_SIZE);
	setName("textbox");
	setClipChildren(true);
	text_widget = widget_list.createWidget<TextWidget>();
	text_widget->setFillColor(text_color);
	text_widget->setParentAnchor(CENTER_LEFT);
	text_widget->setAnchorOffset(TEXT_VIEW_ZERO_POS);
	text_widget->setOrigin(CENTER_LEFT);
	text_widget->setRenderLayer(RenderLayer::TEXTBOX_TEXT);
	text_widget->setParent(this);
	cursor_widget = widget_list.createWidget<RectangleWidget>();
	cursor_widget->setVisible(false);
	cursor_widget->setFillColor(editor_text_color);
	cursor_widget->setSize(sf::Vector2f(1.0f, text_widget->getCharacterSize()));
	cursor_widget->setRenderLayer(RenderLayer::TEXTBOX_CURSOR);
	cursor_widget->setParent(text_widget);
	cursor_widget->setName("cursor");
	selection_widget = widget_list.createWidget<RectangleWidget>();
	selection_widget->setVisible(false);
	selection_widget->setFillColor(selection_color);
	selection_widget->setSize(sf::Vector2f());
	selection_widget->setRenderLayer(RenderLayer::TEXTBOX_SELECTION);
	selection_widget->setParent(text_widget);
	selection_widget->setName("selection");
	setValueSilent("Text");
	setCursorPos(getStringSize());
	deselectAll();
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

const sf::Color& TextBoxWidget::getSelectionColor() const {
	return selection_color;
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

bool TextBoxWidget::isSelectionActive() const {
	return selection_pos >= 0;
}

sf::Vector2f TextBoxWidget::getLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
	sf::Vector2f local_pos = text_widget->getParentLocalCharPos(index, top_aligned, with_kerning);
	return local_pos;
}

sf::Vector2f TextBoxWidget::getGlobalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
	return text_widget->getGlobalCharPos(index, top_aligned, with_kerning);
}

ptrdiff_t TextBoxWidget::getSelectionLeft() const {
	if (!isSelectionActive()) {
		return -1;
	}
	return std::min(selection_pos, (ptrdiff_t)cursor_pos);
}

ptrdiff_t TextBoxWidget::getSelectionRight() const {
	if (!isSelectionActive()) {
		return -1;
	}
	return std::max(selection_pos, (ptrdiff_t)cursor_pos);
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

void TextBoxWidget::setSelectionColor(const sf::Color& color) {
	this->selection_color = color;
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
	deselectAll();
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
	updateVisualCursorPos();
	if (cursor_pos == selection_pos) {
		selection_pos = -1;
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

void TextBoxWidget::eraseSelection() {
	size_t left_char = getSelectionLeft();
	size_t count = getSelectionRight() - getSelectionLeft();
	erase(left_char, count);
	setCursorPos(left_char);
	deselectAll();
}

void TextBoxWidget::processKeyboardEvent(const sf::Event& event) {
	bool shift_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
	bool ctrl_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);
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
					if (!isSelectionActive() && !shift_pressed) {
						if (cursor_pos > 0) {
							setCursorPos(cursor_pos - 1);
						}
					} else if (isSelectionActive() && !shift_pressed) {
						setCursorPos(getSelectionLeft());
						deselectAll();
					} else if (!isSelectionActive() && shift_pressed) {
						if (cursor_pos > 0) {
							size_t sel_pos = cursor_pos;
							setCursorPos(cursor_pos - 1);
							setSelection(sel_pos);
						}
					} else if (isSelectionActive() && shift_pressed) {
						if (cursor_pos > 0) {
							setCursorPos(cursor_pos - 1);
						}
					}
					break;
				case sf::Keyboard::Right:
					if (!isSelectionActive() && !shift_pressed) {
						if (cursor_pos < getStringSize()) {
							setCursorPos(cursor_pos + 1);
						}
					} else if (isSelectionActive() && !shift_pressed) {
						setCursorPos(getSelectionRight());
						deselectAll();
					} else if (!isSelectionActive() && shift_pressed) {
						if (cursor_pos < getStringSize()) {
							size_t sel_pos = cursor_pos;
							setCursorPos(cursor_pos + 1);
							setSelection(sel_pos);
						}
					} else if (isSelectionActive() && shift_pressed) {
						if (cursor_pos < getStringSize()) {
							setCursorPos(cursor_pos + 1);
						}
					}
					break;
				case sf::Keyboard::Home:
					if (!isSelectionActive() && !shift_pressed) {
						setCursorPos(0);
					} else if (isSelectionActive() && !shift_pressed) {
						setCursorPos(0);
						deselectAll();
					} else if (!isSelectionActive() && shift_pressed) {
						size_t sel_pos = cursor_pos;
						setCursorPos(0);
						setSelection(sel_pos);
					} else if (isSelectionActive() && shift_pressed) {
						setCursorPos(0);
					}
					break;
				case sf::Keyboard::End:
					if (!isSelectionActive() && !shift_pressed) {
						setCursorPos(getStringSize());
					} else if (isSelectionActive() && !shift_pressed) {
						setCursorPos(getStringSize());
						deselectAll();
					} else if (!isSelectionActive() && shift_pressed) {
						size_t sel_pos = cursor_pos;
						setCursorPos(getStringSize());
						setSelection(sel_pos);
					} else if (isSelectionActive() && shift_pressed) {
						setCursorPos(getStringSize());
					}
					break;
				case sf::Keyboard::Delete:
					if (isSelectionActive()) {
						eraseSelection();
					} else if (cursor_pos < getStringSize()) {
						erase(cursor_pos, 1);
					}
					break;
				case sf::Keyboard::A:
					if (ctrl_pressed) {
						selectAll();
						process_text_entered_event = false;
					}
					break;
			}
		}
	} else if (event.type == sf::Event::TextEntered) {
		if (process_text_entered_event && edit_mode) {
			sf::Uint32 code = event.text.unicode;
			switch (code) {
				case '\n':
				case '\r':
					break;
				case '\b':
					if (isSelectionActive()) {
						eraseSelection();
					} else if (cursor_pos > 0) {
						erase(cursor_pos - 1, 1);
						setCursorPos(cursor_pos - 1);
					}
					break;
				default:
					if (isSelectionActive()) {
						eraseSelection();
					}
					typeChar(code);
					break;
			}
		}
	}
}

void TextBoxWidget::update() {
	Widget::update();
	sf::Vector2f char_pos = text_widget->getLocalCharPos(cursor_pos, true, true);
	cursor_widget->setPosition(char_pos + CURSOR_OFFSET);
	updateSelection();
	process_text_entered_event = true;
}

void TextBoxWidget::updateColors() {
	sf::Color rect_col;
	sf::Color text_col;
	sf::Color slct_col;
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
	slct_col = selection_color;
	RectangleWidget::setFillColor(rect_col);
	text_widget->setFillColor(text_col);
	cursor_widget->setFillColor(text_col);
	selection_widget->setFillColor(slct_col);
}

void TextBoxWidget::internalOnClick(const sf::Vector2f& pos) {
	drag_start_pos = pos;
	left_button_pressed = true;
	trySetCursor(pos);
	dragging_start_char = cursor_pos;
	deselectAll();
	setEditMode(true);
}

void TextBoxWidget::internalOnRelease(const sf::Vector2f& pos) {
	left_button_pressed = false;
	dragging_begun = false;
}

void TextBoxWidget::internalOnEditModeToggle(bool value) {
	cursor_widget->setVisible(value);
	if (value) {
		selectAll();
	} else {
		setCursorPos(0);
		deselectAll();
	}
	updateColors();
}

void TextBoxWidget::internalOnFocused() {
	setEditMode(true);
}

void TextBoxWidget::internalOnFocusLost() {
	setEditMode(false);
}

void TextBoxWidget::internalProcessMouse(const sf::Vector2f& pos) {
	if (left_button_pressed) {
		if (dragging_begun) {
			trySetCursor(pos);
			setSelection(dragging_start_char);
		} else {
			size_t new_cursor_pos = calcCursorPos(pos);
			if (new_cursor_pos != dragging_start_char) {
				dragging_begun = true;
				setCursorPos(new_cursor_pos);
				setSelection(dragging_start_char);
			}
		}
	}
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

void TextBoxWidget::updateVisualCursorPos() {
	if (!getFont()) {
		return;
	}
	float char_pos = getLocalCharPos(cursor_pos, true, true).x;
	float cursor_visual_pos = char_pos + CURSOR_OFFSET.x;
	float left_bound = CURSOR_MOVE_MARGIN;
	float right_bound = getWidth() - CURSOR_MOVE_MARGIN;
	float offset_left = cursor_visual_pos - left_bound;
	float offset_right = cursor_visual_pos - right_bound;
	if (offset_left < 0) {
		sf::Vector2f anchor_offset = text_widget->getAnchorOffset() - sf::Vector2f(offset_left, 0.0f);
		text_widget->setAnchorOffset(anchor_offset);
	} else if (offset_right > 0) {
		sf::Vector2f anchor_offset = text_widget->getAnchorOffset() - sf::Vector2f(offset_right, 0.0f);
		text_widget->setAnchorOffset(anchor_offset);
	}
}

void TextBoxWidget::updateSelection() {
	if (selection_pos < 0 || selection_pos > getStringSize() || cursor_pos - selection_pos == 0) {
		selection_widget->setSize(sf::Vector2f());
		selection_widget->setVisible(false);
		return;
	}
	sf::Vector2f left_pos = text_widget->getLocalCharPos(getSelectionLeft(), true, true);
	sf::Vector2f right_pos = text_widget->getLocalCharPos(getSelectionRight(), true, true);
	float width = right_pos.x - left_pos.x;
	selection_widget->setPosition(left_pos);
	selection_widget->setSize(width, getCharacterSize());
	selection_widget->setVisible(true);
}

void TextBoxWidget::setSelection(ptrdiff_t pos) {
	selection_pos = std::clamp(pos, (ptrdiff_t)-1, (ptrdiff_t)getStringSize());
	if (selection_pos == cursor_pos) {
		selection_pos = -1;
	}
}

size_t TextBoxWidget::calcCursorPos(const sf::Vector2f& pos) {
	sf::Vector2f local_pos = text_widget->toLocal(pos);
	size_t char_left = text_widget->getCharAt(local_pos);
	if (char_left >= getStringSize()) {
		return getStringSize();
	} else {
		size_t char_right = char_left + 1;
		sf::Vector2f pos_left = getGlobalCharPos(char_left, true, true);
		sf::Vector2f pos_right = getGlobalCharPos(char_right, true, true);
		float dist_left = abs(pos.x - pos_left.x);
		float dist_right = abs(pos.x - pos_right.x);
		sf::Vector2f cursor_visual_pos;
		if (dist_left <= dist_right) {
			return char_left;
		} else {
			return char_right;
		}
	}
}

void TextBoxWidget::trySetCursor(const sf::Vector2f& pos) {
	size_t new_pos = calcCursorPos(pos);
	setCursorPos(new_pos);
}

void TextBoxWidget::selectAll() {
	setCursorPos(getStringSize());
	setSelection(0);
}

void TextBoxWidget::deselectAll() {
	setSelection(-1);
}
