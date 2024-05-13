#pragma once

#include "empty_widget.h"
#include "rectangle_widget.h"
#include "text_widget.h"

namespace fw {

	class WidgetList;

	class WindowWidget : public EmptyWidget {
	public:
		const float HEADER_HEIGHT = 20.0f;
		const float HEADER_TEXT_PADDING = 5.0f;
		const sf::Color DEFAULT_WINDOW_COLOR = sf::Color(128, 128, 128);
		const sf::Color DEFAULT_HEADER_COLOR = sf::Color(150, 150, 150);
		const sf::Color DEFAULT_HEADER_TEXT_COLOR = sf::Color(255, 255, 255);
		const unsigned int DEFAULT_HEADER_TEXT_CHARACTER_SIZE = 13;
		const float ONSCREEN_MARGIN = 20.0f; // part of the window that cannot be dragged off the screen
		const float RESIZE_WIDGET_MARGIN = 10.0f;
		const sf::Vector2f MIN_WINDOW_SIZE = sf::Vector2f(20.0f, 20.0f);

		WindowWidget(WidgetList& widget_list, float width, float height);
		WindowWidget(WidgetList& widget_list, const sf::Vector2f& size);
		bool getHeaderVisible() const;
		void getHeaderColor() const;
		const sf::String& getHeaderText() const;
		const sf::Color& getHeaderTextColor() const;
		const sf::Font* getHeaderFont() const;
		unsigned int getHeaderTextCharacterSize() const;
		const sf::Color& getOutlineColor() const;
		const CompVector<Widget*>& getWindowChildren() const;
		RectangleWidget* getHeaderWidget() const;
		RectangleWidget* getMainWidget() const;
		RectangleWidget* getResizeWidget() const;
		RectangleWidget* getOutlineWidget() const;
		void setHeaderVisible(bool value);
		void setHeaderColor(const sf::Color& color);
		void setHeaderText(const sf::String& text);
		void setHeaderTextColor(const sf::Color& color);
		void setHeaderFont(const sf::Font& font);
		void setHeaderTextCharacterSize(unsigned int size);
		void setOutlineColor(const sf::Color& color);
		void addChild(Widget* child) override;
		void addWindowChild(Widget* child);
		void internalUpdate() override;

	protected:

	private:
		RectangleWidget* header_widget = nullptr;
		TextWidget* header_text_widget = nullptr;
		RectangleWidget* main_widget = nullptr;
		RectangleWidget* resize_widget = nullptr;
		RectangleWidget* outline_widget = nullptr;
		bool is_grabbed = false;
		enum class Resizing {
			NONE,
			TOP_LEFT,
			TOP,
			TOP_RIGHT,
			LEFT,
			RIGHT,
			BOTTOM_LEFT,
			BOTTOM,
			BOTTOM_RIGHT,
		};
		Resizing active_resizing_type = Resizing::NONE;
		sf::Vector2f resizing_anchor;
		sf::Vector2f resizing_cursor_offset;
		sf::Vector2f header_click_offset;
		sf::Vector2f header_size;
		sf::Color outline_color = sf::Color::White;

		Resizing getResizingType() const;
	};

}
