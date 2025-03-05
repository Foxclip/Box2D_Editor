#pragma once

#include "rectangle_widget.h"

namespace fw {

	const sf::Color BUTTON_DEFAULT_NORMAL_COLOR = sf::Color(128, 128, 128);
	const sf::Color BUTTON_DEFAULT_PRESSED_COLOR = sf::Color(255, 255, 0);

	class WidgetList;

	class ButtonWidget : public RectangleWidget {
	public:
		ButtonWidget(WidgetList& widget_list, float width, float height);
		ButtonWidget(WidgetList& widget_list, const sf::Vector2f& size);
		ButtonWidget* clone(bool with_children = true) override;
		void setNormalColor(const sf::Color& color);
		void setPressedColor(const sf::Color& color);

	protected:
		sf::Color normal_color = BUTTON_DEFAULT_NORMAL_COLOR;
		sf::Color pressed_color = BUTTON_DEFAULT_PRESSED_COLOR;
		bool pressed = false;

		void internalOnLeftPress(const sf::Vector2f& pos, bool became_focused) override;
		void internalOnGlobalLeftRelease(const sf::Vector2f& pos) override;
		void updateColors();

	private:
		friend class WindowWidget;

	};

}
