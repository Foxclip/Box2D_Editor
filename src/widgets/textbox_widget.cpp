#include "widgets/textbox_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	enum class TextBoxTextRenderLayers {
		TEXTBOX_SELECTION,
		TEXTBOX_CURSOR,
		TEXTBOX_TEXT,
	};

	TextBoxWidget::TextBoxWidget(WidgetList& widget_list, float width, float height)
		: RectangleWidget(widget_list, width, height) {
		type = WidgetType::Textbox;
		// textbox
		setName("textbox");
		setClipChildren(true);
		setClickThrough(false);
		setFocusableType(FocusableType::NORMAL);
		setForceCustomCursor(true);
		GetCursorType = []() {
			return sf::Cursor::Text;
		};
		// text
		text_widget = widget_list.createTextWidget();
		text_widget->setFillColor(text_color);
		text_widget->setParentAnchor(Anchor::CENTER_LEFT);
		text_widget->setAnchorOffset(TEXTBOX_TEXT_VIEW_ZERO_POS);
		text_widget->setOrigin(Anchor::CENTER_LEFT);
		text_widget->setLocalRenderLayer(static_cast<size_t>(TextBoxTextRenderLayers::TEXTBOX_TEXT));
		text_widget->setParent(this);
		// cursor
		cursor_widget = widget_list.createRectangleWidget(1.0f, 10.0f);
		cursor_widget->setVisible(false);
		cursor_widget->setFillColor(editor_text_color);
		updateCursorSize();
		cursor_widget->setParent(text_widget);
		cursor_widget->setParentLocalRenderLayer(static_cast<size_t>(TextBoxTextRenderLayers::TEXTBOX_CURSOR));
		cursor_widget->setName("cursor");
		// selection
		selection_widget = widget_list.createRectangleWidget(0.0f, 0.0f);
		selection_widget->setVisible(false);
		selection_widget->setFillColor(selection_color);
		selection_widget->setParent(text_widget);
		selection_widget->setParentLocalRenderLayer(static_cast<size_t>(TextBoxTextRenderLayers::TEXTBOX_SELECTION));
		selection_widget->setName("selection");

		setValueSilent("Text");
		setCursorPos(0);
		deselectAll();
		updateColors();
	}

	TextBoxWidget::TextBoxWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: TextBoxWidget(widget_list, size.x, size.y) { }

	TextBoxWidget::TextBoxWidget(WidgetList& widget_list)
		: TextBoxWidget(widget_list, TEXTBOX_DEFAULT_SIZE) { }

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

	const TextWidget* TextBoxWidget::getTextWidget() const {
		return text_widget;
	}

	const fw::Font& TextBoxWidget::getFont() const {
		return text_widget->getFont();
	}

	unsigned int TextBoxWidget::getCharacterSize() const {
		return text_widget->getCharacterSize();
	}

	const sf::String& TextBoxWidget::getValue() const {
		return text_widget->getString();
	}

	const sf::String TextBoxWidget::getSelectedText() const {
		if (!isSelectionActive()) {
			return "";
		}
		size_t left_char = getSelectionLeft();
		size_t count = getSelectionRight() - getSelectionLeft();
		sf::String str = getValue().substring(left_char, count);
		return str;
	}

	bool TextBoxWidget::isValidValue() const {
		bool result;
		if (textbox_type == TextBoxType::TEXT) {
			result = true;
		} else if (textbox_type == TextBoxType::INTEGER) {
			long long number;
			result = parseLL(getValue(), number);
		} else if (textbox_type == TextBoxType::FLOAT) {
			float number;
			result = parseFloat(getValue(), number);
			if (!std::isfinite(number)) {
				result = false;
			}
		} else {
			wAssert(false, "Unknown TextBoxType");
		}
		return result;
	}

	size_t TextBoxWidget::getStringSize() const {
		return text_widget->getStringSize();
	}

	TextBoxWidget::TextBoxType TextBoxWidget::getTextboxType() const {
		return textbox_type;
	}

	size_t TextBoxWidget::getCursorPos() const {
		return cursor_pos;
	}

	bool TextBoxWidget::isEditMode() const {
		return edit_mode;
	}

	bool TextBoxWidget::isSelectionActive() const {
		return selection_pos >= 0 && selection_pos != cursor_pos;
	}

	sf::Vector2f TextBoxWidget::getLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
		sf::Vector2f local_pos = text_widget->getParentLocalCharPos(index, top_aligned, with_kerning);
		return local_pos;
	}

	sf::Vector2f TextBoxWidget::getGlobalCharPos(size_t index, bool top_aligned, bool with_kerning) const {
		return text_widget->getGlobalCharPos(index, top_aligned, with_kerning);
	}

	const sf::Glyph& TextBoxWidget::getGlyph(size_t index) const {
		sf::Uint32 code = getValue()[index];
		const sf::Glyph& glyph = text_widget->getFont().getSfmlFont().getGlyph(code, getCharacterSize(), false);
		return glyph;
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

	void TextBoxWidget::setFont(const fw::Font& font) {
		this->font = font;
		text_widget->setFont(font);
	}

	void TextBoxWidget::setCharacterSize(unsigned int size) {
		text_widget->setCharacterSize(size);
		updateCursorSize();
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
		this->textbox_type = type;
		updateValid();
	}

	void TextBoxWidget::setCursorPos(size_t pos) {
		pos = std::clamp(pos, (size_t)0, getStringSize());
		this->cursor_pos = pos;
		updateTextScroll();
		if (cursor_pos == selection_pos) {
			selection_pos = -1;
		}
		if (edit_mode) {
			cursor_timer.reset();
			cursor_widget->setVisible(true);
		}
	}

	void TextBoxWidget::typeChar(sf::Uint32 code) {
		if (textbox_type == TextBoxType::INTEGER) {
			if (TEXTBOX_VALID_INTEGER_CHARS.find(code) == -1) {
				return;
			}
		} else if (textbox_type == TextBoxType::FLOAT) {
			if (TEXTBOX_VALID_FLOAT_CHARS.find(code) == -1) {
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
		if (!isSelectionActive()) {
			return;
		}
		size_t left_char = getSelectionLeft();
		size_t count = getSelectionRight() - getSelectionLeft();
		erase(left_char, count);
		setCursorPos(left_char);
		deselectAll();
	}

	TextBoxWidget* TextBoxWidget::clone(bool with_children) {
		return widget_list.duplicateWidget(this, with_children);
	}

	void TextBoxWidget::internalPreUpdate() {
		sf::Vector2f char_pos = text_widget->getLocalCharPos(cursor_pos, true, true);
		cursor_widget->setPosition(char_pos - sf::Vector2f(0.0f, TEXTBOX_CURSOR_MARGIN) + TEXTBOX_CURSOR_OFFSET);
		if (edit_mode) {
			if (cursor_timer.get() > TEXTBOX_CURSOR_BLINK_INTERVAL) {
				cursor_timer.reset();
				cursor_widget->toggleVisible();
			}
		}
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

	void TextBoxWidget::internalOnLeftPress(const sf::Vector2f& pos, bool became_focused) {
		drag_start_pos = pos;
		left_button_pressed = true;
		if (!became_focused) {
			trySetCursor(pos);
			deselectAll();
		}
		enableEditMode();
		size_t new_cursor_pos = calcCursorPos(pos);
		dragging_start_char = new_cursor_pos;
		history.updateCurrent();
	}

	void TextBoxWidget::internalOnGlobalLeftRelease(const sf::Vector2f& pos) {
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
		enableEditMode();
	}

	void TextBoxWidget::internalOnFocusLost() {
		disableEditMode(true);
	}

	void TextBoxWidget::processKeyPressedEvent(const sf::Event& event) {
		bool shift_pressed = widget_list.isLShiftPressed();
		bool ctrl_pressed = widget_list.isLCtrlPressed();
		if (event.key.code == sf::Keyboard::Escape) {
			disableEditMode(false);
			removeFocus();
		} else if (event.key.code == sf::Keyboard::Enter) {
			if (edit_mode) {
				disableEditMode(true);
			} else {
				enableEditMode();
			}
		}
		if (edit_mode) {
			if (event.key.code == sf::Keyboard::Left) {
				if (!isSelectionActive() && !shift_pressed) {
					if (cursor_pos > 0) {
						doCursorAction([&]() {
							setCursorPos(cursor_pos - 1);
						});
					}
				} else if (isSelectionActive() && !shift_pressed) {
					doCursorAction([&]() {
						setCursorPos(getSelectionLeft());
						deselectAll();
					});
				} else if (!isSelectionActive() && shift_pressed) {
					if (cursor_pos > 0) {
						size_t sel_pos = cursor_pos;
						doCursorAction([&]() {
							setCursorPos(cursor_pos - 1);
							setSelection(sel_pos);
						});
					}
				} else if (isSelectionActive() && shift_pressed) {
					if (cursor_pos > 0) {
						doCursorAction([&]() {
							setCursorPos(cursor_pos - 1);
						});
					}
				}
			} else if (event.key.code == sf::Keyboard::Right) {
				if (!isSelectionActive() && !shift_pressed) {
					if (cursor_pos < getStringSize()) {
						doCursorAction([&]() {
							setCursorPos(cursor_pos + 1);
						});
					}
				} else if (isSelectionActive() && !shift_pressed) {
					doCursorAction([&]() {
						setCursorPos(getSelectionRight());
						deselectAll();
					});
				} else if (!isSelectionActive() && shift_pressed) {
					if (cursor_pos < getStringSize()) {
						size_t sel_pos = cursor_pos;
						doCursorAction([&]() {
							setCursorPos(cursor_pos + 1);
							setSelection(sel_pos);
						});
					}
				} else if (isSelectionActive() && shift_pressed) {
					if (cursor_pos < getStringSize()) {
						doCursorAction([&]() {
							setCursorPos(cursor_pos + 1);
						});
					}
				}
			} else if (event.key.code == sf::Keyboard::Home) {
				if (!isSelectionActive() && !shift_pressed) {
					doCursorAction([&]() {
						setCursorPos(0);
					});
				} else if (isSelectionActive() && !shift_pressed) {
					doCursorAction([&]() {
						setCursorPos(0);
						deselectAll();
					});
				} else if (!isSelectionActive() && shift_pressed) {
					size_t sel_pos = cursor_pos;
					doCursorAction([&]() {
						setCursorPos(0);
						setSelection(sel_pos);
					});
				} else if (isSelectionActive() && shift_pressed) {
					doCursorAction([&]() {
						setCursorPos(0);
					});
				}
			} else if (event.key.code == sf::Keyboard::End) {
				if (!isSelectionActive() && !shift_pressed) {
					doCursorAction([&]() {
						setCursorPos(getStringSize());
					});
				} else if (isSelectionActive() && !shift_pressed) {
					doCursorAction([&]() {
						setCursorPos(getStringSize());
						deselectAll();
					});
				} else if (!isSelectionActive() && shift_pressed) {
					size_t sel_pos = cursor_pos;
					doCursorAction([&]() {
						setCursorPos(getStringSize());
						setSelection(sel_pos);
					});
				} else if (isSelectionActive() && shift_pressed) {
					doCursorAction([&]() {
						setCursorPos(getStringSize());
					});
				}
			} else if (event.key.code == sf::Keyboard::Delete) {
				if (isSelectionActive()) {
					doGroupAction(ActionType::ACTION_DELETE, [&]() {
						eraseSelection();
					});
				} else if (cursor_pos < getStringSize()) {
					doGroupAction(ActionType::ACTION_DELETE, [&]() {
						erase(cursor_pos, 1);
					});
				}
			} else if (event.key.code == sf::Keyboard::A) {
				if (ctrl_pressed) {
					doCursorAction([&]() {
						selectAll();
					});
					process_text_entered_event = false;
				}
			} else if (event.key.code == sf::Keyboard::C) {
				if (ctrl_pressed) {
					if (getSelectedText().getSize() > 0) {
						if (!clip::set_text(getSelectedText())) {
							throw std::runtime_error("Unable to copy text to clipboard");
						}
					}
					process_text_entered_event = false;
				}
			} else if (event.key.code == sf::Keyboard::V) {
				if (ctrl_pressed) {
					std::string pasted_text;
					if (!clip::get_text(pasted_text)) {
						throw std::runtime_error("Unable to paste text from clipboard");
					}
					std::string prev_value = getValue();
					doNormalAction(ActionType::ACTION_PASTE, [&]() {
						eraseSelection();
						insert(cursor_pos, pasted_text);
						setCursorPos(cursor_pos + pasted_text.size());
					});
					process_text_entered_event = false;
				}
			} else if (event.key.code == sf::Keyboard::X) {
				if (ctrl_pressed) {
					if (getSelectedText().getSize() > 0) {
						if (!clip::set_text(getSelectedText())) {
							throw std::runtime_error("Unable to copy text to clipboard");
						}
						doNormalAction(ActionType::ACTION_CUT, [&]() {
							eraseSelection();
						});
					}
					process_text_entered_event = false;
				}
			} else if (event.key.code == sf::Keyboard::Z) {
				if (ctrl_pressed && !shift_pressed) {
					history.undo();
					process_text_entered_event = false;
				} else if (ctrl_pressed && shift_pressed) {
					history.redo();
					process_text_entered_event = false;
				}
			}
		}
	}

	void TextBoxWidget::processTextEnteredEvent(const sf::Event& event) {
		if (process_text_entered_event && edit_mode) {
			sf::Uint32 code = event.text.unicode;
			if (code == '\n' || code == '\r') {
				//skip
			} else if (code == '\b') {
				if (isSelectionActive()) {
					doGroupAction(ActionType::ACTION_BACKSPACE, [&]() {
						eraseSelection();
					});
				} else if (cursor_pos > 0) {
					doGroupAction(ActionType::ACTION_BACKSPACE, [&]() {
						erase(cursor_pos - 1, 1);
						setCursorPos(cursor_pos - 1);
					});
				}
			} else {
				doGroupAction(ActionType::ACTION_TYPE, [&]() {
					eraseSelection();
					typeChar(code);
				});
			}
		}
	}

	void TextBoxWidget::internalProcessKeyboardEvent(const sf::Event& event) {
		try {
			if (event.type == sf::Event::KeyPressed) {
				processKeyPressedEvent(event);
			} else if (event.type == sf::Event::TextEntered) {
				processTextEnteredEvent(event);
			}
		} catch (std::exception exc) {
			throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
		}
	}

	void TextBoxWidget::internalProcessMouse(const sf::Vector2f& pos) {
		if (left_button_pressed) {
			if (dragging_begun) {
				size_t old_cursor_pos = cursor_pos;
				size_t new_cursor_pos = calcCursorPos(pos);
				if (new_cursor_pos != old_cursor_pos) {
					doCursorAction([&]() {
						trySetCursor(pos);
						setSelection(dragging_start_char);
					});
				}
			} else {
				size_t new_cursor_pos = calcCursorPos(pos);
				if (new_cursor_pos != dragging_start_char) {
					dragging_begun = true;
					doCursorAction([&]() {
						setCursorPos(new_cursor_pos);
						setSelection(dragging_start_char);
					});
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

	void TextBoxWidget::enableEditMode() {
		if (edit_mode) {
			return;
		}
		edit_mode = true;
		internalOnEditModeToggle(true);
		OnEditModeToggle(true);
		auto history_get = [&]() {
			TextBoxHistoryEntry entry;
			entry.str = getValue();
			entry.cursor_pos = cursor_pos;
			entry.last_action_pos = last_action_pos;
			entry.selection_pos = selection_pos;
			return entry;
		};
		auto history_set = [&](const TextBoxHistoryEntry& value) {
			setValue(value.str);
			setCursorPos(value.cursor_pos);
			last_action_pos = value.last_action_pos;
			setSelection(value.selection_pos);
		};
		last_action_pos = -1;
		last_action_tag = "";
		history = History<TextBoxHistoryEntry>("Textbox", history_get, history_set);
		history.save("Base");
	}

	void TextBoxWidget::disableEditMode(bool confirm) {
		if (!edit_mode) {
			return;
		}
		if (confirm) {
			internalOnConfirm(getValue());
			OnConfirm(getValue());
		} else {
			internalOnCancel();
			OnCancel();
		}
		edit_mode = false;
		internalOnEditModeToggle(false);
		OnEditModeToggle(false);
	}

	void TextBoxWidget::insertSilent(size_t pos, const sf::String& str) {
		text_widget->insert(pos, str);
	}

	void TextBoxWidget::eraseSilent(size_t index_first, size_t count) {
		text_widget->erase(index_first, count);
	}

	void TextBoxWidget::updateTextScroll() {
		if (!getFont().isLoaded()) {
			return;
		}
		float char_pos = getLocalCharPos(cursor_pos, true, true).x;
		float cursor_visual_pos = char_pos + TEXTBOX_CURSOR_OFFSET.x;
		float left_bound = TEXTBOX_CURSOR_MOVE_MARGIN;
		float right_bound = getWidth() - TEXTBOX_CURSOR_MOVE_MARGIN;
		if (left_bound > right_bound) {
			return;
		}
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

	void TextBoxWidget::updateCursorSize() {
		float cursor_height = text_widget->getCharacterSize() + TEXTBOX_CURSOR_MARGIN * 2;
		cursor_widget->setSize(sf::Vector2f(1.0f, cursor_height));
	}

	void TextBoxWidget::updateSelection() {
		if (selection_pos < 0 || selection_pos > (ptrdiff_t)getStringSize() || cursor_pos - selection_pos == 0) {
			selection_widget->setSize(sf::Vector2f());
			selection_widget->setVisible(false);
			return;
		}
		sf::Vector2f left_pos = text_widget->getLocalCharPos(getSelectionLeft(), true, true);
		sf::Vector2f right_pos = text_widget->getLocalCharPos(getSelectionRight(), true, true);
		sf::Vector2f selection_widget_pos = left_pos - sf::Vector2f(0.0f, TEXTBOX_SELECTION_MARGIN);
		selection_widget->setPosition(selection_widget_pos);
		float width = right_pos.x - left_pos.x;
		float height = getCharacterSize() + TEXTBOX_SELECTION_MARGIN * 2;
		selection_widget->setSize(width, height);
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
			sf::Vector2f pos_left = getGlobalCharPos(char_left);
			sf::Vector2f pos_right = getGlobalCharPos(char_right);
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

	std::string TextBoxWidget::getActionTag(ActionType action_type) {
		switch (action_type) {
			case ActionType::ACTION_DELETE: return "Delete";
			case ActionType::ACTION_PASTE: return "Paste";
			case ActionType::ACTION_CUT: return "Cut";
			case ActionType::ACTION_BACKSPACE: return "Backspace";
			case ActionType::ACTION_TYPE: return "Type";
			default: wAssert(false, "Unknown action_type: " + std::to_string(action_type)); return "Unknown";
		}
	}

	void TextBoxWidget::doNormalAction(ActionType action_type, const std::function<void()>& action) {
		std::string tag = getActionTag(action_type);
		action();
		last_action_pos = cursor_pos;
		last_action_tag = tag;
		history.save(tag);
	}

	void TextBoxWidget::doCursorAction(const std::function<void()>& action) {
		action();
		history.updateCurrent();
	}

	void TextBoxWidget::doGroupAction(ActionType action_type, const std::function<void()>& action) {
		std::string tag = getActionTag(action_type);
		bool do_update = cursor_pos == last_action_pos && tag == last_action_tag;
		action();
		last_action_pos = cursor_pos;
		last_action_tag = tag;
		if (do_update) {
			history.updateCurrent();
		} else {
			history.save(tag);
		}
	}

	Timer::Timer() {
		reset();
	}

	void Timer::reset() {
		begin = std::chrono::high_resolution_clock::now();
	}

	double Timer::get() const {
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = now - begin;
		return duration.count();
	}

}
