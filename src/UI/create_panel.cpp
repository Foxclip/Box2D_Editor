#include "UI/create_panel.h"
#include "application.h"

CreatePanel::CreatePanel(WidgetList& widget_list, Application& p_app) : ContainerWidget(widget_list), app(p_app) {
    setVisible(false);
    setFillColor(sf::Color(255, 0, 0, 0));
    setOrigin(Widget::CENTER_LEFT);
    setParentAnchor(Widget::CENTER_LEFT);
    setHorizontal(false);
    setPadding(CREATE_PANEL_PADDING);
    setClickThrough(false);
    setName("create panel");
    for (size_t i = 0; i < CreateTool::mode_count; i++) {
        RectangleWidget* button_widget = widget_list.createWidget<RectangleWidget>();
        button_widget->setSize(sf::Vector2f(CREATE_RECT_WIDTH, CREATE_RECT_HEIGHT));
        button_widget->setFillColor(sf::Color(128, 128, 128));
        button_widget->setOutlineColor(sf::Color(0, 175, 255));
        button_widget->OnClick = [=](const sf::Vector2f& pos) {
            app.select_create_type(i);
        };
        button_widget->OnMouseEnter = [=](const sf::Vector2f pos) {
            button_widget->setOutlineThickness(-1.0f);
        };
        button_widget->OnMouseExit = [=](const sf::Vector2f pos) {
            button_widget->setOutlineThickness(0.0f);
        };
        button_widget->setName("button " + std::to_string(i));
        TextWidget* text_widget = widget_list.createWidget<TextWidget>();
        text_widget->setFont(app.ui_font);
        text_widget->setCharacterSize(TOOL_TEXT_SIZE);
        text_widget->setString(CreateTool::create_type_name(static_cast<CreateTool::ObjectType>(i)));
        text_widget->setFillColor(sf::Color::Black);
        text_widget->setOrigin(Widget::CENTER);
        text_widget->setParentAnchor(Widget::CENTER);
        text_widget->setParent(button_widget);
        create_tool.create_buttons.add(button_widget);
        button_widget->setParent(this);
    }
}
