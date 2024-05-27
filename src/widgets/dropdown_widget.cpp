#include "widgets/dropdown_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	DropdownWidget::DropdownWidget(WidgetList& widget_list) : EmptyWidget(widget_list) {
		// empty
		type = WidgetType::Dropdown;
		setName("dropdown");
		// main
		setName("main");
		main_widget = widget_list.createWidget<RectangleWidget>(DROPDOWN_DEFAULT_SIZE);
		main_widget->setClickThrough(false);
		main_widget->setFillColor(DROPDOWN_DEFAULT_MAIN_COLOR);
		main_widget->setSizePolicy(Widget::SizePolicy::PARENT);
		main_widget->OnLeftPress = [&](const sf::Vector2f& pos) {
			togglePanel();
		};
		main_widget->setParent(this);
		// main text
		text_widget = widget_list.createWidget<TextWidget>();
		text_widget->setString("Dropdown");
		text_widget->setFillColor(DROPDOWN_DEFAULT_MAIN_TEXT_COLOR);
		text_widget->setParent(main_widget);
		// options panel
		panel_widget = widget_list.createWidget<ContainerWidget>(100.0f, 100.0f);
		panel_widget->setClickThrough(false);
		panel_widget->setFillColor(DROPDOWN_DEFAULT_PANEL_COLOR);
		panel_widget->setParentAnchor(Anchor::BOTTOM_LEFT);
		panel_widget->setHorizontal(false);
		panel_widget->setParent(main_widget);
	}

	const sf::Color& DropdownWidget::getMainBackgroundColor() const {
		return main_widget->getFillColor();
	}

	const sf::Color& DropdownWidget::getPanelBackgroundColor() const {
		return panel_widget->getFillColor();
	}

	const sf::Color& DropdownWidget::getMainTextColor() const {
		return text_widget->getFillColor();
	}

	const sf::Color& DropdownWidget::getPanelTextColor() const {
		return panel_text_color;
	}

	void DropdownWidget::addOption(const sf::String& text, ptrdiff_t index) {
		wAssert(index == -1 || index < (ptrdiff_t)option_widgets.size() - 1);
		if (index < 0) {
			index = option_widgets.size();
		}
		ContainerWidget* option_widget = widget_list.createWidget<ContainerWidget>(100.0f, 20.0f);
		option_widget->setParent(panel_widget);
		option_widget->setFillColor(sf::Color::Transparent);
		option_widgets.push_back(option_widget);
		TextWidget* option_text_widget = widget_list.createWidget<TextWidget>();
		if (text_widget->getFont()) {
			option_text_widget->setFont(*text_widget->getFont());
		}
		option_text_widget->setCharacterSize(text_widget->getCharacterSize());
		option_text_widget->setFillColor(panel_text_color);
		option_text_widget->setString(text);
		option_text_widget->setParent(option_widget);
		updateOptions();
	}

	void DropdownWidget::setOptionText(size_t index, const sf::String& text) {
		wAssert(index >= 0 && index < option_widgets.size());
		ContainerWidget* option_widget = option_widgets[index];
		TextWidget* option_text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
		option_text_widget->setString(text);
	}

	void DropdownWidget::removeOption(size_t index) {
		wAssert(index >= 0 && index < option_widgets.size());
		option_widgets[index]->remove();
		updateOptions();
	}

	void DropdownWidget::removeOption(const sf::String& text) {
		for (size_t i = 0; i < option_widgets.size(); i++) {
			ContainerWidget* option_widget = option_widgets[i];
			TextWidget* option_text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
			if (option_text_widget->getString() == text) {
				removeOption(i);
			}
			return;
		}
		wAssert(false, "Option not found: " + text); 
	}

	void DropdownWidget::updateOptions() {
		for (size_t i = 0; i < option_widgets.size(); i++) {
			ContainerWidget* option_widget = option_widgets[i];
			option_widget->setName("option" + std::to_string(i));
			option_widget->OnLeftPress = [=](const sf::Vector2f& pos) {
				selectOption(i);
			};
		}
	}

	void DropdownWidget::selectOption(size_t index) {
		wAssert(index >= 0 && index < option_widgets.size());
		selected = index;
		OnOptionSelected(index);
		ContainerWidget* option = option_widgets[index];
		std::string text = dynamic_cast<TextWidget*>(option->find("text"))->getString();
		text_widget->setString(text);
	}

	void DropdownWidget::setMainBackgroundColor(const sf::Color& color) {
		main_widget->setFillColor(color);
	}

	void DropdownWidget::setPanelBackgroundColor(const sf::Color& color) {
		panel_widget->setFillColor(color);
	}

	void DropdownWidget::setMainTextColor(const sf::Color& color) {
		text_widget->setFillColor(color);
	}

	void DropdownWidget::setPanelTextColor(const sf::Color& color) {
		panel_text_color = color;
		for (size_t i = 0; i < option_widgets.size(); i++) {
			ContainerWidget* option_widget = option_widgets[i];
			TextWidget* option_text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
			option_text_widget->setFillColor(color);
		}
	}

	void DropdownWidget::setFont(const sf::Font& font) {
		text_widget->setFont(font);
		for (size_t i = 0; i < option_widgets.size(); i++) {
			ContainerWidget* option_widget = option_widgets[i];
			TextWidget* option_text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
			option_text_widget->setFont(font);
		}
	}

	void DropdownWidget::setCharacterSize(unsigned int size) {
		text_widget->setCharacterSize(size);
		for (size_t i = 0; i < option_widgets.size(); i++) {
			ContainerWidget* option_widget = option_widgets[i];
			TextWidget* option_text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
			option_text_widget->setCharacterSize(size);
		}
	}

	void DropdownWidget::setPanelPaddingX(float padding) {
		panel_widget->setPaddingX(padding);
	}

	void DropdownWidget::setPanelPaddingY(float padding) {
		panel_widget->setPaddingY(padding);
	}

	void DropdownWidget::togglePanel() {
		panel_widget->toggleVisible();
	}

}