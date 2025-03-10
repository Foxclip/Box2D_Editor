#include "widgets/dropdown_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	DropdownWidget::DropdownWidget(WidgetList& widget_list) : EmptyWidget(widget_list) {
		// empty
		type = WidgetType::Dropdown;
		setName("dropdown");
		setClickThrough(false);
		setFocusableType(FocusableType::MODAL);
		OnLeftPress += [&](const sf::Vector2f& pos) {
			togglePanel();
		};
		OnFocused += [&]() {
			LoggerTag dropdown_tag("dropdown");
			logger << "OnFocused\n";
		};
		OnFocusLost += [&]() {
			LoggerTag dropdown_tag("dropdown");
			logger << "OnFocusLost\n";
			if (panel_widget->isVisible()) {
				hidePanel();
				logger << "Hiding panel\n";
			}
		};
		// main
		main_widget = widget_list.createRectangleWidget(DROPDOWN_DEFAULT_SIZE);
		main_widget->setName("main");
		main_widget->setClipChildren(true);
		main_widget->setFillColor(DROPDOWN_DEFAULT_MAIN_COLOR);
		main_widget->setSizePolicy(Widget::SizePolicy::PARENT);
		main_widget->setParent(this);
		// main text
		text_widget = widget_list.createTextWidget();
		text_widget->setString("Dropdown");
		text_widget->setFillColor(DROPDOWN_DEFAULT_MAIN_TEXT_COLOR);
		text_widget->setParent(main_widget);
		// main square
		square_widget = widget_list.createRectangleWidget(main_widget->getHeight(), main_widget->getHeight());
		square_widget->setName("square");
		square_widget->setFillColor(DROPDOWN_DEFAULT_SQUARE_COLOR);
		square_widget->setParentAnchor(Anchor::TOP_RIGHT);
		square_widget->setOrigin(Anchor::TOP_RIGHT);
		square_widget->setSizeYPolicy(SizePolicy::PARENT);
		square_widget->setParent(main_widget);
		// main triangle
		std::vector<sf::Vector2f> vertices = get_regular_polygon<sf::Vector2f>(3, 5.0f, to_radians(90.0f));
		triangle_widget = widget_list.createPolygonWidget(vertices);
		triangle_widget->setName("triangle");
		triangle_widget->setFillColor(DROPDOWN_DEFAULT_TRIANGLE_COLOR);
		triangle_widget->setParentAnchor(Anchor::CENTER);
		triangle_widget->setParent(square_widget);
		// options panel
		panel_widget = widget_list.createRectangleWidget(100.0f, 100.0f);
		panel_widget->setName("panel");
		panel_widget->setVisible(false);
		panel_widget->setClickThrough(false);
		panel_widget->setFillColor(DROPDOWN_DEFAULT_PANEL_COLOR);
		panel_widget->setParentAnchor(Anchor::BOTTOM_LEFT);
		panel_widget->setGlobalRenderLayer(GlobalRenderLayer::DROPDOWN_PANEL);
		panel_widget->setParent(this);
	}

	const sf::Color& DropdownWidget::getMainBackgroundColor() const {
		return main_widget->getFillColor();
	}

	const sf::Color& DropdownWidget::getOptionHoverBackgroundColor() const {
		return option_hover_background_color;
	}

	const sf::Color& DropdownWidget::getTriangleColor() const {
		return triangle_widget->getFillColor();
	}

	const sf::Color& DropdownWidget::getSquareColor() const {
		return square_widget->getFillColor();
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

	RectangleWidget* DropdownWidget::getMainWidget() const {
		return main_widget;
	}

	RectangleWidget* DropdownWidget::getPanelWidget() const {
		return panel_widget;
	}

	RectangleWidget* DropdownWidget::getOptionWidget(size_t index) const {
		wAssert(index >= 0 && index < option_widgets.size());
		return option_widgets[index];
	}

	TextWidget* DropdownWidget::getOptionTextWidget(size_t index) const {
		wAssert(index >= 0 && index < option_widgets.size());
		RectangleWidget* option_widget = option_widgets[index];
		TextWidget* text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
		return text_widget;
	}

	const sf::String& DropdownWidget::getOptionText(size_t index) const {
		return getOptionTextWidget(index)->getString();
	}

	ptrdiff_t DropdownWidget::getValue() const {
		return selected;
	}

	bool DropdownWidget::isPanelOpen() const {
		return panel_widget->isVisible();
	}

	void DropdownWidget::setSize(float width, float height) {
		setSizeInternal(width, height);
		updateOptionSize();
	}

	void DropdownWidget::addOption(const sf::String& text, ptrdiff_t index) {
		wAssert(index == -1 || index < (ptrdiff_t)option_widgets.size());
		if (index < 0) {
			index = option_widgets.size();
		}
		RectangleWidget* option_widget = widget_list.createRectangleWidget(100.0f, 20.0f);
		option_widget->setParent(panel_widget);
		option_widget->moveToIndex(index);
		option_widget->setFillColor(sf::Color::Transparent);
		option_widget->setClipChildren(true);
		option_widget->OnMouseEnter += [&, option_widget](const sf::Vector2f& pos) {
			option_widget->setFillColor(option_hover_background_color);
		};
		option_widget->OnMouseExit += [&, option_widget](const sf::Vector2f& pos) {
			option_widget->setFillColor(sf::Color::Transparent);
		};
		option_widget->setGlobalRenderLayer(GlobalRenderLayer::DROPDOWN_PANEL);
		option_widgets.insert(option_widgets.begin() + index, option_widget);
		TextWidget* option_text_widget = widget_list.createTextWidget();
		if (text_widget->getFont().isLoaded()) {
			option_text_widget->setFont(text_widget->getFont());
		}
		option_text_widget->setCharacterSize(text_widget->getCharacterSize());
		option_text_widget->setParentAnchor(text_widget->getParentAnchor());
		option_text_widget->setOrigin(text_widget->getOriginAnchor());
		option_text_widget->setFillColor(panel_text_color);
		option_text_widget->setString(text);
		option_text_widget->setParent(option_widget);
		option_text_widget->setGlobalRenderLayer(GlobalRenderLayer::DROPDOWN_PANEL);
		updateOptions();
	}

	void DropdownWidget::selectOption(size_t index) {
		wAssert(index >= 0 && index < option_widgets.size());
		selected = index;
		OnValueChanged(index);
		RectangleWidget* option = option_widgets[index];
		std::string text = dynamic_cast<TextWidget*>(option->find("text"))->getString();
		text_widget->setString(text);
	}

	void DropdownWidget::setOptionText(size_t index, const sf::String& text) {
		wAssert(index >= 0 && index < option_widgets.size());
		RectangleWidget* option_widget = option_widgets[index];
		TextWidget* option_text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
		option_text_widget->setString(text);
	}

	void DropdownWidget::removeOption(size_t index) {
		wAssert(index >= 0 && index < option_widgets.size());
		option_widgets[index]->remove();
		option_widgets.removeAt(index);
		updateOptions();
	}

	void DropdownWidget::removeOption(const sf::String& text) {
		for (size_t i = 0; i < option_widgets.size(); i++) {
			RectangleWidget* option_widget = option_widgets[i];
			TextWidget* option_text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
			if (option_text_widget->getString() == text) {
				removeOption(i);
				return;
			}
		}
		wAssert(false, "Option not found: " + text); 
	}

	DropdownWidget* DropdownWidget::clone(bool with_children) {
		return widget_list.duplicateWidget(this, with_children);
	}

	void DropdownWidget::updateOptions() {
		for (size_t i = 0; i < option_widgets.size(); i++) {
			RectangleWidget* option_widget = option_widgets[i];
			option_widget->setName("option" + std::to_string(i));
			option_widget->OnLeftPress += [=](const sf::Vector2f& pos) {
				selectOption(i);
				hidePanel();
			};
		}
		updateOptionSize();
	}

	void DropdownWidget::updateOptionSize() {
		float max_text_width = 0.0f;
		for (size_t i = 0; i < option_widgets.size(); i++) {
			RectangleWidget* option_widget = option_widgets[i];
			TextWidget* option_text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
			float text_width = option_text_widget->getWidth();
			max_text_width = std::max(max_text_width, text_width);
		}
		float options_width = std::max(main_widget->getWidth(), max_text_width);
		for (size_t i = 0; i < option_widgets.size(); i++) {
			RectangleWidget* option_widget = option_widgets[i];
			option_widget->setPosition(0.0f, i * main_widget->getHeight());
			option_widget->setWidth(options_width);
			option_widget->setHeight(main_widget->getHeight());
		}
		panel_widget->setWidth(options_width);
		panel_widget->setHeight(main_widget->getHeight() * option_widgets.size());
	}

	void DropdownWidget::setMainBackgroundColor(const sf::Color& color) {
		main_widget->setFillColor(color);
	}

	void DropdownWidget::setOptionHoverBackgroundColor(const sf::Color& color) {
		option_hover_background_color = color;
	}

	void DropdownWidget::setTriangleColor(const sf::Color& color) {
		triangle_widget->setFillColor(color);
	}

	void DropdownWidget::setSquareColor(const sf::Color& color) {
		square_widget->setFillColor(color);
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
			RectangleWidget* option_widget = option_widgets[i];
			TextWidget* option_text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
			option_text_widget->setFillColor(color);
		}
	}

	void DropdownWidget::setFont(const fw::Font& font) {
		text_widget->setFont(font);
		for (size_t i = 0; i < option_widgets.size(); i++) {
			RectangleWidget* option_widget = option_widgets[i];
			TextWidget* option_text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
			option_text_widget->setFont(font);
		}
	}

	void DropdownWidget::setCharacterSize(unsigned int size) {
		text_widget->setCharacterSize(size);
		for (size_t i = 0; i < option_widgets.size(); i++) {
			RectangleWidget* option_widget = option_widgets[i];
			TextWidget* option_text_widget = dynamic_cast<TextWidget*>(option_widget->find("text"));
			option_text_widget->setCharacterSize(size);
		}
	}

	void DropdownWidget::setTextAnchor(Anchor anchor) {
		text_widget->setParentAnchor(anchor);
		for (size_t i = 0; i < option_widgets.size(); i++) {
			TextWidget* option_text_widget = getOptionTextWidget(i);
			option_text_widget->setParentAnchor(anchor);
		}
	}

	void DropdownWidget::setTextOriginAnchor(Anchor anchor) {
		text_widget->setOrigin(anchor);
		for (size_t i = 0; i < option_widgets.size(); i++) {
			TextWidget* option_text_widget = getOptionTextWidget(i);
			option_text_widget->setOrigin(anchor);
		}
	}

	void DropdownWidget::showPanel() {
		panel_widget->setVisible(true);
	}

	void DropdownWidget::hidePanel() {
		if (main_widget->isFocused()) {
			main_widget->removeFocus();
		}
		panel_widget->setVisible(false);
	}

	void DropdownWidget::togglePanel() {
		if (panel_widget->isVisible()) {
			hidePanel();
		} else {
			showPanel();
		}
		
	}

}