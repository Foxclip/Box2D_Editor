#pragma once

#include "rectangle_widget.h"

namespace fw {

	const sf::Vector2f CHECKBOX_DEFAULT_SIZE = sf::Vector2f(20.0f, 20.0f);

	class WidgetList;

	class CheckboxWidget : public RectangleWidget {
	public:
		std::function<void(bool)> OnValueChanged = [](bool new_value) { };

		CheckboxWidget(WidgetList& widget_list);
		bool getValue() const;
		const sf::Color& getFillColor() const override;
		const sf::Color& getHighlightFillColor() const;
		const sf::Color& getCheckFillColor() const;
		void setFillColor(const sf::Color& color) override;
		void setHighlightFillColor(const sf::Color& color);
		void setCheckFillColor(const sf::Color& color);
		void setValueSilent(bool value);
		void setValue(bool value);
		void toggleValue();

	protected:
		void internalUpdate() override;
		void internalOnLeftPress(const sf::Vector2f& pos, bool became_focused) override;
		void internalOnMouseEnter(const sf::Vector2f& pos) override;
		void internalOnMouseExit(const sf::Vector2f& pos) override;

	private:
		bool checked = false;
		float check_size = 0.6f;
		RectangleWidget* check_widget = nullptr;
		sf::Color background_fill_color = sf::Color(50, 50, 50);
		sf::Color highlight_fill_color = sf::Color(100, 100, 100);
		sf::Color check_fill_color = sf::Color(255, 128, 0);

	};

}
