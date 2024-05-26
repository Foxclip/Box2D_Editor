#include "UI/edit_window.h"
#include "editor.h"

EditWindow::EditWindow(fw::WidgetList& widget_list, Editor& p_app)
    : fw::WindowWidget(widget_list, 200.0f, 300.0f), app(p_app) {
    setName("edit window");
    setVisible(false);
    setOrigin(Anchor::CENTER);
    setFillColor(sf::Color(128, 128, 128));
    setClickThrough(false);
    setHeaderFont(app.console_font);
    setHeaderTextCharacterSize(15);
    setHeaderText("Edit Window");
    container_widget = widget_list.createWidget<fw::ContainerWidget>(main_widget->getSize());
    container_widget->setHorizontal(false);
    container_widget->setPadding(EDIT_WINDOW_PARAMETER_PADDING);
    container_widget->setSizeXPolicy(SizePolicy::PARENT);
    container_widget->setSizeYPolicy(SizePolicy::CHILDREN);
    container_widget->setFillColor(sf::Color(75, 75, 75));
    container_widget->setName("container");
    container_widget->setParent(this);
    lockChildren();
    createParameters();
    setSpacingWidgets();
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
            return app.active_object->getBodyType() == b2_dynamicBody;
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

void EditWindow::setSpacingWidgets() {
    // this is to sort the text labels and checkboxes/textboxes into two columns
    float max_text_width = 0.0f;
    for (size_t i = 0; i < parameters.size(); i++) {
        EditWindowParameter* parameter = parameters[i];
        fw::TextWidget* text_widget = dynamic_cast<fw::TextWidget*>(parameter->getWidget()->find("text"));
        mAssert(text_widget);
        max_text_width = std::max(max_text_width, text_widget->getWidth());
    }
    for (size_t i = 0; i < parameters.size(); i++) {
        EditWindowParameter* parameter = parameters[i];
        fw::TextWidget* text_widget = dynamic_cast<fw::TextWidget*>(parameter->getWidget()->find("text"));
        EmptyWidget* spacing_widget = dynamic_cast<fw::EmptyWidget*>(parameter->getWidget()->find("spacing"));
        mAssert(spacing_widget);
        float padding = container_widget->getHorizontalPadding();
        spacing_widget->setWidth(max_text_width + padding - text_widget->getWidth());
    }
}

EditWindowParameter::EditWindowParameter(EditWindow& p_edit_window)
    : edit_window(p_edit_window), app(p_edit_window.app) { }

fw::ContainerWidget* EditWindowParameter::getWidget() const {
    return widget;
}

fw::ContainerWidget* EditWindowParameter::createParameterWidget(const std::string& name, const std::string& text) {
    fw::ContainerWidget* parameter_widget = app.widgets.createWidget<fw::ContainerWidget>(100.0f, 20.0f);
    parameter_widget->setName(name);
    parameter_widget->setFillColor(sf::Color::Transparent);
    parameter_widget->setVerticalAlignment(fw::Widget::Alignment::ALIGN_CENTER);
    parameter_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
    parameter_widget->setSizeYPolicy(fw::Widget::SizePolicy::CHILDREN);
    parameter_widget->setParent(edit_window.container_widget);
    fw::TextWidget* parameter_text_widget = app.widgets.createWidget<fw::TextWidget>();
    parameter_text_widget->setFont(app.ui_font);
    parameter_text_widget->setCharacterSize(16);
    parameter_text_widget->setString(text);
    parameter_text_widget->setParent(parameter_widget);
    fw::EmptyWidget* spacing_widget = app.widgets.createWidget<fw::EmptyWidget>();
    spacing_widget->setName("spacing");
    spacing_widget->setSize(10.0f, 0.0f);
    spacing_widget->setFillColor(sf::Color::Transparent);
    spacing_widget->setParent(parameter_widget);
    return parameter_widget;
}

fw::TextBoxWidget* EditWindowParameter::createTextBoxWidget() {
    fw::TextBoxWidget* textbox_widget = app.widgets.createWidget<fw::TextBoxWidget>(100.0f, 20.0f);
    textbox_widget->setFont(app.textbox_font);
    textbox_widget->setCharacterSize(12);
    textbox_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
    return textbox_widget;
}

fw::CheckboxWidget* EditWindowParameter::createCheckboxWidget() {
    return app.widgets.createWidget<fw::CheckboxWidget>();
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
    this->textbox_widget->OnConfirm = [&](const sf::String& value) {
        if (this->get_value() != value) {
            this->set_value(value);
        }
    };
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
    this->checkbox_widget->OnValueChanged = [&](bool new_value) {
        if (this->get_value() != new_value) {
            this->set_value(new_value);
        }
    };
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
    this->textbox_widget->setType(fw::TextBoxWidget::TextBoxType::FLOAT);
    this->textbox_widget->OnConfirm = [&](const sf::String& str) {
        if (textbox_widget->isValidValue()) {
            float new_value = std::stof(str.toAnsiString());
            if (this->get_value() != new_value) {
                this->set_value(new_value);
            }
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
