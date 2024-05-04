#include "widgets/window_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	WindowWidget::WindowWidget(WidgetList& widget_list, float width, float height) : RectangleWidget(widget_list) {
		setName("window");
		setSize(width, height);
		setFillColor(DEFAULT_WINDOW_COLOR);
		//setClipChildren(true);
		setClickThrough(false);
		setForceCustomCursor(true);
		header_widget = widget_list.createWidget<RectangleWidget>();
		header_widget->setSize(getWidth() / 2.0f, HEADER_HEIGHT);
		header_widget->setOrigin(Anchor::BOTTOM_LEFT);
		header_widget->setParentAnchor(Anchor::TOP_LEFT);
		header_widget->setName("header");
		header_widget->setParent(this);
		header_text_widget = widget_list.createWidget<TextWidget>();
		header_text_widget->setOrigin(Anchor::TOP_LEFT);
		header_text_widget->setParentAnchor(Anchor::TOP_LEFT);
		header_text_widget->setAnchorOffset(HEADER_TEXT_PADDING, 0.0f);
		header_text_widget->setName("header text");
		header_text_widget->setParent(header_widget);
		setHeaderText("New Window");
		setHeaderColor(DEFAULT_HEADER_COLOR);
		setHeaderTextColor(DEFAULT_HEADER_TEXT_COLOR);
		setHeaderTextCharacterSize(DEFAULT_HEADER_TEXT_CHARACTER_SIZE);
	}

	bool WindowWidget::getHeaderVisible() const {
		return header_widget->isVisible();
	}

	void WindowWidget::getHeaderColor() const {
		header_widget->getFillColor();
	}

	const sf::String& WindowWidget::getHeaderText() const {
		return header_text_widget->getString();
	}

	const sf::Color& WindowWidget::getHeaderTextColor() const {
		return header_text_widget->getFillColor();
	}

	const sf::Font* WindowWidget::getHeaderFont() const {
		return header_text_widget->getFont();
	}

	unsigned int WindowWidget::getHeaderTextCharacterSize() const {
		return header_text_widget->getCharacterSize();
	}

	void WindowWidget::setHeaderVisible(bool value) {
		header_widget->setVisible(value);
	}

	void WindowWidget::setHeaderColor(const sf::Color& color) {
		header_widget->setFillColor(color);
	}

	void WindowWidget::setHeaderText(const sf::String& text) {
		header_text_widget->setString(text);
	}

	void WindowWidget::setHeaderTextColor(const sf::Color& color) {
		header_text_widget->setFillColor(color);
	}

	void WindowWidget::setHeaderFont(const sf::Font& font) {
		header_text_widget->setFont(font);
	}

	void WindowWidget::setHeaderTextCharacterSize(unsigned int size) {
		header_text_widget->setCharacterSize(size);
	}

}