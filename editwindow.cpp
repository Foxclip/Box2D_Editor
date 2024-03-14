#include "editwindow.h"
#include "application.h"

EditWindow::EditWindow(WidgetList* widget_list) : app(*widget_list->app) {
    this->widget_list = widget_list;
    setVisible(false);
    setSize(sf::Vector2f(100.0f, 200.0f));
    setHorizontal(false);
    setFillColor(sf::Color(128, 128, 128));
    setOrigin(Widget::CENTER);
    setParentAnchor(Widget::CENTER);
    setAnchorOffset(-20.0f, 20.0f);
    setPadding(TOOLBOX_PADDING);
    setClickThrough(false);
    setName("edit window");
    {
        ContainerWidget* dynamic_parameter_widget = createParameterWidget("dynamic parameter", "Dynamic:");
        CheckboxWidget* checkbox_widget = app.widgets.createWidget<CheckboxWidget>();
        checkbox_widget->setOrigin(Widget::TOP_LEFT);
        checkbox_widget->setHighlightFillColor(sf::Color(100, 100, 100));
        checkbox_widget->OnValueChanged = [&](bool value) {
            assert(app.active_object);
            b2BodyType type = value ? b2_dynamicBody : b2_staticBody;
            app.active_object->setType(type, false);
        };
        checkbox_widget->setParent(dynamic_parameter_widget);
    }
    {
        ContainerWidget* name_parameter_widget = createParameterWidget("name parameter", "Name:");
        TextBoxWidget* textbox_widget = app.widgets.createWidget<TextBoxWidget>();
        textbox_widget->setFont(app.textbox_font);
        textbox_widget->setSize(40.0f, 20.0f);
        textbox_widget->setCharacterSize(12);
        textbox_widget->setValue("<name>");
        textbox_widget->OnValueChanged = [&](const sf::String& str) {
            assert(app.active_object);
            app.active_object->setName(str);
        };
        textbox_widget->setParent(name_parameter_widget);
    }
}

ContainerWidget* EditWindow::createParameterWidget(const std::string& name, const std::string& text) {
    ContainerWidget* parameter_widget = app.widgets.createWidget<ContainerWidget>();
    parameter_widget->setFillColor(sf::Color::Transparent);
    parameter_widget->setVerticalAlignment(Widget::ALIGN_CENTER);
    parameter_widget->setParent(this);
    parameter_widget->setName(name);
    TextWidget* parameter_text_widget = app.widgets.createWidget<TextWidget>();
    parameter_text_widget->setFont(app.ui_font);
    parameter_text_widget->setCharacterSize(16);
    parameter_text_widget->setString(text);
    parameter_text_widget->setParent(parameter_widget);
    RectangleWidget* spacing_widget = app.widgets.createWidget<RectangleWidget>();
    spacing_widget->setSize(sf::Vector2f(10.0f, 1.0f));
    spacing_widget->setFillColor(sf::Color::Transparent);
    spacing_widget->setParent(parameter_widget);
    spacing_widget->setName("spacing");
    return parameter_widget;
}
