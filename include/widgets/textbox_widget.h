#pragma once

#include "rectangle_widget.h"
#include "text_widget.h"
#include "clip/clip.h"
#include "history.h"

class WidgetList;

class TextBoxWidget : public RectangleWidget {
public:
	enum TextBoxType {
		TEXT,
		INTEGER,
		FLOAT,
	};
	std::function<void(bool)> OnEditModeToggle = [](bool new_value) { };
	std::function<void(const sf::String&)> OnValueChanged = [](const sf::String& new_value) { };
	std::function<void(const sf::String&)> OnConfirm = [](const sf::String& value) { };
	std::function<void()> OnCancel = []() { };

	TextBoxWidget(WidgetList& widget_list);
	bool isFocusable() const override;
	sf::Cursor::Type getCursorType() const override;
	const sf::Color& getFillColor() const override;
	const sf::Color& getHighlightColor() const;
	const sf::Color& getTextColor() const;
	const sf::Color& getEditorColor() const;
	const sf::Color& getEditorTextColor() const;
	const sf::Color& getSelectionColor() const;
	const sf::Color& getFailFillColor() const;
	const sf::Color& getEditFailFillColor() const;
	const sf::Font* getFont() const;
	unsigned int getCharacterSize() const;
	const sf::String& getValue() const;
	const sf::String getSelectedText() const;
	bool isValidValue() const;
	size_t getStringSize() const;
	TextBoxType getType() const;
	size_t getCursorPos() const;
	bool isEditMode() const;
	bool isSelectionActive() const;
	sf::Vector2f getLocalCharPos(size_t index, bool top_aligned, bool with_kerning) const;
	sf::Vector2f getGlobalCharPos(size_t index, bool top_aligned, bool with_kerning) const;
	ptrdiff_t getSelectionLeft() const;
	ptrdiff_t getSelectionRight() const;
	void setFillColor(const sf::Color& color) override;
	void setHighlightColor(const sf::Color& color);
	void setTextColor(const sf::Color& color);
	void setEditorColor(const sf::Color& color);
	void setEditorTextColor(const sf::Color& color);
	void setSelectionColor(const sf::Color& color);
	void setFailFillColor(const sf::Color& color);
	void setEditFailFillColor(const sf::Color& color);
	void setFont(const sf::Font& font);
	void setCharacterSize(unsigned int size);
	void setValueSilent(const sf::String& value);
	void setValue(const sf::String& value);
	void setType(TextBoxType type);
	void setCursorPos(size_t pos);
	void typeChar(sf::Uint32 code);
	void insert(size_t pos, const sf::String& str);
	void erase(size_t index_first, size_t count);
	void eraseSelection();

protected:
	void update() override;
	void updateColors();
	void internalOnClick(const sf::Vector2f& pos) override;
	void internalOnRelease(const sf::Vector2f& pos) override;
	void internalOnEditModeToggle(bool value);
	void internalOnFocused() override;
	void internalOnFocusLost() override;
	void internalProcessKeyboardEvent(const sf::Event& event) override;
	void internalProcessMouse(const sf::Vector2f& pos) override;
	void internalOnMouseEnter(const sf::Vector2f& pos) override;
	void internalOnMouseExit(const sf::Vector2f& pos) override;
	void internalOnValueChanged(const sf::String& new_value);
	void internalOnConfirm(const sf::String& value);
	void internalOnCancel();
	void updateValid();
	void enableEditMode();
	void disableEditMode(bool confirm);
	void insertSilent(size_t pos, const sf::String& str);
	void eraseSilent(size_t index_first, size_t count);
	void updateTextScroll();
	void updateCursorSize();
	void updateSelection();
	void setSelection(ptrdiff_t pos);
	size_t calcCursorPos(const sf::Vector2f& pos);
	void trySetCursor(const sf::Vector2f& pos);
	void selectAll();
	void deselectAll();

private:
	const sf::Vector2f DEFAULT_SIZE = sf::Vector2f(40.0f, 20.0f);
	const sf::Vector2f TEXT_VIEW_ZERO_POS = sf::Vector2f(2.0f, 0.0f);
	const sf::Vector2f CURSOR_OFFSET = sf::Vector2f(0.0f, 0.0f);
	const float CURSOR_MARGIN = 2.0f;
	const float SELECTION_MARGIN = 2.0f;
	const std::string VALID_INTEGER_CHARS = "+-0123456789";
	const std::string VALID_FLOAT_CHARS = "+-.e0123456789";
	const float CURSOR_MOVE_MARGIN = 2.0f;
	TextWidget* text_widget = nullptr;
	RectangleWidget* cursor_widget = nullptr;
	RectangleWidget* selection_widget = nullptr;
	TextBoxType type = TextBoxType::TEXT;
	History<std::string> history;
	size_t cursor_pos = 0;
	ptrdiff_t selection_pos = -1;
	sf::Vector2f drag_start_pos;
	size_t dragging_start_char = 0;
	bool edit_mode = false;
	bool highlighted = false;
	bool left_button_pressed = false;
	bool dragging_begun = false;
	bool fail_state = false;
	bool process_text_entered_event = true;
	sf::Color background_color = sf::Color(50, 50, 50);
	sf::Color highlight_color = sf::Color(100, 100, 100);
	sf::Color text_color = sf::Color(255, 255, 255);
	sf::Color editor_color = sf::Color(255, 255, 255);
	sf::Color editor_text_color = sf::Color(0, 0, 0);
	sf::Color selection_color = sf::Color(128, 200, 255);
	sf::Color fail_background_color = sf::Color(128, 0, 0);
	sf::Color editor_fail_background_color = sf::Color(255, 128, 128);

};
