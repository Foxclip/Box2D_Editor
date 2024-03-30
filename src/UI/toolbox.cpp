#include "UI/toolbox.h"
#include "editor.h"

Toolbox::Toolbox(fw::WidgetList& widget_list, Editor& p_app)
    : fw::ContainerWidget(widget_list), app(p_app) {
    setFillColor(sf::Color(255, 0, 0, 0));
    setOrigin(Widget::TOP_CENTER);
    setParentAnchor(Widget::TOP_CENTER);
    setPadding(TOOLBOX_PADDING);
    setClickThrough(false);
    setName("toolbox");
    for (size_t i = 0; i < app.tools.size(); i++) {
        Tool* tool = app.tools[i];
        if (!tool->showInToolPanel()) {
            continue;
        }
        RectangleWidget* tool_widget = app.widgets.createWidget<RectangleWidget>();
        tool_widget->setSize(sf::Vector2f(TOOL_RECT_WIDTH, TOOL_RECT_HEIGHT));
        tool_widget->setFillColor(sf::Color(128, 128, 128));
        tool_widget->setOutlineColor(sf::Color::Yellow);
        tool_widget->OnClick = [=](const sf::Vector2f& pos) {
            app.try_select_tool(tool);
        };
        tool_widget->OnMouseEnter = [=](const sf::Vector2f pos) {
            tool_widget->setOutlineThickness(-1.0f);
        };
        tool_widget->OnMouseExit = [=](const sf::Vector2f pos) {
            tool_widget->setOutlineThickness(0.0f);
        };
        tool_widget->setParent(this);
        tool_widget->setName("tool " + tool->name);
        fw::TextWidget* text_widget = app.widgets.createWidget<fw::TextWidget>();
        text_widget->setFont(app.ui_font);
        text_widget->setCharacterSize(TOOL_TEXT_SIZE);
        text_widget->setString(app.tools[i]->name);
        text_widget->setFillColor(sf::Color::Black);
        text_widget->setOrigin(Widget::CENTER);
        text_widget->setParentAnchor(Widget::CENTER);
        text_widget->setParent(tool_widget);
        tool->widget = tool_widget;
        app.tools_in_tool_panel.push_back(tool);
    }
}
