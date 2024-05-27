#pragma once

#include "container_widget.h"
#include "empty_widget.h"
#include "rectangle_widget.h"
#include "text_widget.h"

namespace fw {

	const sf::Vector2f DROPDOWN_DEFAULT_SIZE = sf::Vector2f(100.0f, 20.0f);
	const sf::Color DROPDOWN_DEFAULT_MAIN_COLOR = sf::Color(128, 128, 128);
	const sf::Color DROPDOWN_DEFAULT_PANEL_COLOR = sf::Color(128, 128, 128);
	const sf::Color DROPDOWN_DEFAULT_MAIN_TEXT_COLOR = sf::Color(255, 255, 255);
	const sf::Color DROPDOWN_DEFAULT_PANEL_TEXT_COLOR = sf::Color(255, 255, 255);

	class WidgetList;

	class DropdownWidget : public EmptyWidget {
	public:
		std::function<void(size_t index)> OnOptionSelected = [](size_t index) { };

		DropdownWidget(WidgetList& widget_list);
		const sf::Color& getMainBackgroundColor() const;
		const sf::Color& getPanelBackgroundColor() const;
		const sf::Color& getMainTextColor() const;
		const sf::Color& getPanelTextColor() const;
		void setMainBackgroundColor(const sf::Color& color);
		void setPanelBackgroundColor(const sf::Color& color);
		void setMainTextColor(const sf::Color& color);
		void setPanelTextColor(const sf::Color& color);
		void setFont(const sf::Font& font);
		void setCharacterSize(unsigned int size);
		void setPanelPaddingX(float padding);
		void setPanelPaddingY(float padding);
		void togglePanel();
		void addOption(const sf::String& text, ptrdiff_t index = -1);
		void setOptionText(size_t index, const sf::String& text);
		void removeOption(size_t index);
		void removeOption(const sf::String& text);

	protected:
		RectangleWidget* main_widget = nullptr;
		TextWidget* text_widget = nullptr;
		ContainerWidget* panel_widget = nullptr;
		std::vector<ContainerWidget*> option_widgets;

	private:
		ptrdiff_t selected = -1;
		sf::Color panel_text_color = DROPDOWN_DEFAULT_PANEL_TEXT_COLOR;

		void updateOptions();
		void selectOption(size_t index);

	};

}
