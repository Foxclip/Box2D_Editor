#include "widgets/checkbox_widget.h"
#include "widgets/widget_list.h"

CheckboxWidget::CheckboxWidget(WidgetList& widget_list) : RectangleWidget(widget_list) {
	setSize(DEFAULT_SIZE);
	RectangleWidget::setFillColor(background_fill_color);
	setName("checkbox");
	check_widget = widget_list.createWidget<RectangleWidget>();
	check_widget->setVisible(checked);
	check_widget->setSize(rect.getSize() * check_size);
	check_widget->setFillColor(check_fill_color);
	check_widget->setOrigin(CENTER);
	check_widget->setParentAnchor(CENTER);
	check_widget->setParent(this);
	check_widget->setName("check");
}

bool CheckboxWidget::isFocusable() const {
	return true;
}

bool CheckboxWidget::getValue() const {
	return checked;
}

const sf::Color& CheckboxWidget::getFillColor() const {
	return background_fill_color;
}

const sf::Color& CheckboxWidget::getHighlightFillColor() const {
	return highlight_fill_color;
}

const sf::Color& CheckboxWidget::getCheckFillColor() const {
	return check_fill_color;
}

void CheckboxWidget::setFillColor(const sf::Color& color) {
	background_fill_color = color;
}

void CheckboxWidget::setHighlightFillColor(const sf::Color& color) {
	highlight_fill_color = color;
}

void CheckboxWidget::setCheckFillColor(const sf::Color& color) {
	check_fill_color = color;
}

void CheckboxWidget::setValueSilent(bool value) {
	checked = value;
	check_widget->setVisible(value);
}

void CheckboxWidget::setValue(bool value) {
	setValueSilent(value);
	OnValueChanged(value);
}

void CheckboxWidget::toggleValue() {
	setValueSilent(!checked);
	OnValueChanged(checked);
}

void CheckboxWidget::update() {
	Widget::update();
	check_widget->setSize(rect.getSize() * check_size);
}

void CheckboxWidget::internalOnClick(const sf::Vector2f& pos) {
	toggleValue();
}

void CheckboxWidget::internalOnMouseEnter(const sf::Vector2f& pos) {
	RectangleWidget::setFillColor(highlight_fill_color);
}

void CheckboxWidget::internalOnMouseExit(const sf::Vector2f& pos) {
	RectangleWidget::setFillColor(background_fill_color);
}
