#include "widgets/button_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	ButtonWidget::ButtonWidget(WidgetList& widget_list, float width, float height) : RectangleWidget(widget_list, width, height) {
		type = WidgetType::Button;
		setName("button");
		setClickThrough(false);
		updateColors();
	}

	ButtonWidget::ButtonWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: RectangleWidget(widget_list, size.x, size.y) { }

	ButtonWidget* ButtonWidget::clone(bool with_children) {
		return widget_list.duplicateWidget(this, with_children);
	}

	void ButtonWidget::setNormalColor(const sf::Color& color) {
		normal_color = color;
		updateColors();
	}

	void ButtonWidget::setPressedColor(const sf::Color& color) {
		pressed_color = color;
		updateColors();
	}

	void ButtonWidget::internalOnLeftPress(const sf::Vector2f& pos, bool became_focused) {
		pressed = true;
		updateColors();
		OnPress();
	}

	void ButtonWidget::internalOnGlobalLeftRelease(const sf::Vector2f& pos) {
		pressed = false;
		updateColors();
		OnRelease();
	}

	void ButtonWidget::updateColors() {
		if (pressed) {
			setFillColor(pressed_color);
		} else {
			setFillColor(normal_color);
		}
	}

}
