#include "UI/edit_window.h"
#include "application.h"

EditWindow::EditWindow(WidgetList& widget_list, Application& p_app) : ContainerWidget(widget_list), app(p_app) {
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
    createParameters();
}

void EditWindow::updateParameters() {
    for (size_t i = 0; i < parameters.size(); i++) {
        parameters[i]->getValue();
    }
}

void EditWindow::createParameters() {
    createParameter<BoolParameter>(
        "dynamic parameter",
        "Dynamic:",
        [=]() {
            return app.active_object->getType() == b2_dynamicBody;
        },
        [=](bool value) {
            b2BodyType type = value ? b2_dynamicBody : b2_staticBody;
            app.active_object->setType(type, false);
            app.commit_action = true;
        }
    );
    createParameter<TextParameter>(
        "name parameter",
        "Name:",
        [=]() {
            return app.active_object->getName();
        },
        [=](const sf::String& str) {
            app.active_object->setName(str);
            app.commit_action = true;
        }
    );
    createParameter<FloatParameter>(
        "position x parameter",
        "Position x:",
        [=]() {
            return app.active_object->getGlobalPosition().x;
        },
        [=](float value) {
            b2Vec2 old_pos = app.active_object->getGlobalPosition();
            b2Vec2 new_pos = b2Vec2(value, old_pos.y);
            app.active_object->setGlobalPosition(new_pos);
            app.commit_action = true;
        }
    );
    createParameter<FloatParameter>(
        "position y parameter",
        "Position y:",
        [=]() {
            return app.active_object->getGlobalPosition().y;
        },
        [=](float value) {
            b2Vec2 old_pos = app.active_object->getGlobalPosition();
            b2Vec2 new_pos = b2Vec2(old_pos.x, value);
            app.active_object->setGlobalPosition(new_pos);
            app.commit_action = true;
        }
    );
    createParameter<FloatParameter>(
        "angle parameter",
        "Angle:",
        [=]() {
            return utils::to_degrees(app.active_object->getGlobalRotation());
        },
        [=](float value) {
            app.active_object->setGlobalAngle(utils::to_radians(value));
            app.commit_action = true;
        }
    );
    createParameter<FloatParameter>(
        "velocity x parameter",
        "Velocity x:",
        [=]() {
            return app.active_object->getLinearVelocity().x;
        },
        [=](float value) {
            b2Vec2 old_vel = app.active_object->getLinearVelocity();
            b2Vec2 new_vel = b2Vec2(value, old_vel.y);
            app.active_object->setLinearVelocity(new_vel, false);
            app.commit_action = true;
        }
    );
    createParameter<FloatParameter>(
        "velocity y parameter",
        "Velocity y:",
        [=]() {
            return app.active_object->getLinearVelocity().y;
        },
        [=](float value) {
            b2Vec2 old_vel = app.active_object->getLinearVelocity();
            b2Vec2 new_vel = b2Vec2(old_vel.x, value);
            app.active_object->setLinearVelocity(new_vel, false);
            app.commit_action = true;
        }
    );
    createParameter<FloatParameter>(
        "angular velocity parameter",
        "Angular velocity:",
        [=]() {
            return utils::to_degrees(app.active_object->getAngularVelocity());
        },
        [=](float value) {
            app.active_object->setAngularVelocity(utils::to_radians(value), false);
            app.commit_action = true;
        }
    );
}

EditWindowParameter::EditWindowParameter(EditWindow& p_edit_window)
    : edit_window(p_edit_window), app(p_edit_window.app) { }

ContainerWidget* EditWindowParameter::createParameterWidget(const std::string& name, const std::string& text) {
    ContainerWidget* parameter_widget = app.widgets.createWidget<ContainerWidget>();
    parameter_widget->setFillColor(sf::Color::Transparent);
    parameter_widget->setVerticalAlignment(Widget::ALIGN_CENTER);
    parameter_widget->setParent(&edit_window);
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

TextBoxWidget* EditWindowParameter::createTextBoxWidget() {
    TextBoxWidget* textbox_widget = app.widgets.createWidget<TextBoxWidget>();
    textbox_widget->setFont(app.textbox_font);
    textbox_widget->setSize(100.0f, 20.0f);
    textbox_widget->setCharacterSize(12);
    return textbox_widget;
}

CheckboxWidget* EditWindowParameter::createCheckboxWidget() {
    return app.widgets.createWidget<CheckboxWidget>();
}

TextParameter::TextParameter(
    EditWindow& p_edit_window,
    const std::string& name,
    const std::string& text,
    std::function<sf::String(void)> get_value,
    std::function<void(const sf::String&)> set_value
) : EditWindowParameter(p_edit_window) {
    this->get_value = get_value;
    this->set_value = set_value;
    this->widget = createParameterWidget(name, text);
    this->textbox_widget = createTextBoxWidget();
    this->textbox_widget->OnConfirm = set_value;
    this->textbox_widget->setParent(widget);
}

void TextParameter::getValue() const {
    if (!textbox_widget->isEditMode()) {
        textbox_widget->setValueSilent(get_value());
    }
}

BoolParameter::BoolParameter(
    EditWindow& p_edit_window,
    const std::string& name,
    const std::string& text,
    std::function<bool(void)> get_value,
    std::function<void(bool)> set_value
) : EditWindowParameter(p_edit_window) {
    this->get_value = get_value;
    this->set_value = set_value;
    this->widget = createParameterWidget(name, text);
    this->checkbox_widget = createCheckboxWidget();
    this->checkbox_widget->OnValueChanged = set_value;
    this->checkbox_widget->setParent(widget);
}

void BoolParameter::getValue() const {
    checkbox_widget->setValueSilent(get_value());
}

FloatParameter::FloatParameter(
    EditWindow& p_edit_window,
    const std::string& name,
    const std::string& text,
    std::function<float(void)> get_value,
    std::function<void(float)> set_value
) : EditWindowParameter(p_edit_window) {
    this->get_value = get_value;
    this->set_value = set_value;
    this->widget = createParameterWidget(name, text);
    this->textbox_widget = createTextBoxWidget();
    this->textbox_widget->setType(TextBoxWidget::TextBoxType::FLOAT);
    this->textbox_widget->OnConfirm = [=](const sf::String& str) {
        if (textbox_widget->isValidValue()) {
            float value = std::stof(str.toAnsiString());
            set_value(value);
        }
    };
    this->textbox_widget->setParent(widget);
}

void FloatParameter::getValue() const {
    if (!textbox_widget->isEditMode()) {
        std::string str = utils::floatToStr(get_value(), 9);
        this->textbox_widget->setValueSilent(str);
    }
}
