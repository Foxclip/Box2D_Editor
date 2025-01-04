#pragma once

#include "rectangle_widget.h"
#include "text_widget.h"
#include "clip/clip.h"
#include "common/history.h"
#include <chrono>

namespace fw {

	const sf::Vector2f TEXTBOX_DEFAULT_SIZE = sf::Vector2f(40.0f, 20.0f);
	const sf::Vector2f TEXTBOX_TEXT_VIEW_ZERO_POS = sf::Vector2f(2.0f, 0.0f);
	const sf::Vector2f TEXTBOX_CURSOR_OFFSET = sf::Vector2f(0.0f, 0.0f);
	const float TEXTBOX_CURSOR_MARGIN = 2.0f; // cursor sticking out above and below text
	const float TEXTBOX_CURSOR_BLINK_INTERVAL = 0.5f;
	const float TEXTBOX_SELECTION_MARGIN = 2.0f; // selection sticking out above and below text
	const std::string TEXTBOX_VALID_INTEGER_CHARS = "+-0123456789";
	const std::string TEXTBOX_VALID_FLOAT_CHARS = "+-.e0123456789";
	const float TEXTBOX_CURSOR_MOVE_MARGIN = 2.0f; // scroll text when cursor approaches margin

	class Timer {
	public:
		Timer();
		void reset();
		double get() const;
	private:
		std::chrono::steady_clock::time_point begin;
	};

	class WidgetList;

	class TextBoxWidget : public RectangleWidget {
	public:
		std::function<void(bool)> OnEditModeToggle = [](bool new_value) { };
		std::function<void(const sf::String&)> OnValueChanged = [](const sf::String& new_value) { };
		std::function<void(const sf::String&)> OnConfirm = [](const sf::String& value) { };
		std::function<void()> OnCancel = []() { };
		enum TextBoxType {
			TEXT,
			INTEGER,
			FLOAT,
		};

		TextBoxWidget(WidgetList& widget_list);
		TextBoxWidget(WidgetList& widget_list, float width, float height);
		TextBoxWidget(WidgetList& widget_list, const sf::Vector2f& size);
		const sf::Color& getFillColor() const override;
		const sf::Color& getHighlightColor() const;
		const sf::Color& getTextColor() const;
		const sf::Color& getEditorColor() const;
		const sf::Color& getEditorTextColor() const;
		const sf::Color& getSelectionColor() const;
		const sf::Color& getFailFillColor() const;
		const sf::Color& getEditFailFillColor() const;
		const TextWidget* getTextWidget() const;
		const fw::Font& getFont() const;
		unsigned int getCharacterSize() const;
		const sf::String& getValue() const;
		const sf::String getSelectedText() const;
		bool isValidValue() const;
		size_t getStringSize() const;
		TextBoxType getTextboxType() const;
		size_t getCursorPos() const;
		bool isEditMode() const;
		bool isSelectionActive() const;
		sf::Vector2f getLocalCharPos(size_t index, bool top_aligned = true, bool with_kerning = true) const;
		sf::Vector2f getGlobalCharPos(size_t index, bool top_aligned = true, bool with_kerning = true) const;
		const sf::Glyph& getGlyph(size_t index) const;
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
		void setFont(const fw::Font& font);
		void setCharacterSize(unsigned int size);
		void setValueSilent(const sf::String& value);
		void setValue(const sf::String& value);
		void setType(TextBoxType type);
		void setCursorPos(size_t pos);
		void typeChar(sf::Uint32 code);
		void insert(size_t pos, const sf::String& str);
		void erase(size_t index_first, size_t count);
		void eraseSelection();
		TextBoxWidget* clone(bool with_children = true) override;


	protected:
		enum ActionType {
			ACTION_DELETE,
			ACTION_PASTE,
			ACTION_CUT,
			ACTION_BACKSPACE,
			ACTION_TYPE,
		};
		void internalPreUpdate() override;
		void updateColors();
		void internalOnLeftPress(const sf::Vector2f& pos, bool became_focused) override;
		void internalOnGlobalLeftRelease(const sf::Vector2f& pos) override;
		void internalOnEditModeToggle(bool value);
		void internalOnFocused() override;
		void internalOnFocusLost() override;
		void processKeyPressedEvent(const sf::Event& event);
		void processTextEnteredEvent(const sf::Event& event);
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
		std::string getActionTag(ActionType action_type);
		void doNormalAction(ActionType action_type, const std::function<void()>& action);
		void doCursorAction(const std::function<void()>& action);
		void doGroupAction(ActionType action_type, const std::function<void()>& action);

	private:
		TextWidget* text_widget = nullptr;
		RectangleWidget* cursor_widget = nullptr;
		RectangleWidget* selection_widget = nullptr;
		TextBoxType textbox_type = TextBoxType::TEXT;
		fw::Font font;
		struct TextBoxHistoryEntry {
			sf::String str;
			size_t cursor_pos = 0;
			ptrdiff_t last_action_pos = -1;
			std::string last_action_tag;
			ptrdiff_t selection_pos = -1;
		};
		History<TextBoxHistoryEntry> history;
		Timer cursor_timer;
		size_t cursor_pos = 0;
		ptrdiff_t last_action_pos = -1;
		std::string last_action_tag;
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

}
