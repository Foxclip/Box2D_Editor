#include "editor/UI/menu.h"
#include "widgets/text_widget.h"
#include "editor/editor.h"

Menu::Menu(fw::WidgetList& widget_list, Editor& p_app)
    : fw::ContainerWidget(widget_list, 500.0f, 100.0f), app(p_app) {
    setFillColor(sf::Color(255, 0, 0, 0));
    setOrigin(Anchor::TOP_LEFT);
    setParentAnchor(Anchor::TOP_LEFT);
    setPadding(MENU_PADDING);
    setClickThrough(false);
    setName("menu");

    RectangleWidget* open_file_button = createButton("open", "Open");
    open_file_button->OnLeftPress += [=](const sf::Vector2f& pos) {
        app.showOpenFileMenu();
    };
    RectangleWidget* save_file_button = createButton("save", "Save");
    save_file_button->OnLeftPress += [=](const sf::Vector2f& pos) {
        app.save();
    };
    RectangleWidget* save_as_file_button = createButton("save_as", "Save As");
    save_as_file_button->OnLeftPress += [=](const sf::Vector2f& pos) {
        app.showSaveFileMenu();
    };
}

fw::RectangleWidget* Menu::createButton(const std::string& name, const std::string& display_name) {
    RectangleWidget* button_widget = app.widgets.createRectangleWidget(
        MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT
    );
    button_widget->setFillColor(sf::Color(128, 128, 128));
    button_widget->setOutlineColor(sf::Color::Yellow);
    button_widget->OnMouseEnter += [=](const sf::Vector2f pos) {
        button_widget->setOutlineThickness(-1.0f);
    };
    button_widget->OnMouseExit += [=](const sf::Vector2f pos) {
        button_widget->setOutlineThickness(0.0f);
    };
    button_widget->setParent(this);
    button_widget->setName(name);
    fw::TextWidget* text_widget = app.widgets.createTextWidget();
    text_widget->setFont(app.ui_font);
    text_widget->setCharacterSize(MENU_BUTTON_TEXT_SIZE);
    text_widget->setString(display_name);
    text_widget->setFillColor(sf::Color::Black);
    text_widget->setOrigin(Anchor::CENTER);
    text_widget->setParentAnchor(Anchor::CENTER);
    text_widget->setParent(button_widget);
    return button_widget;
}
