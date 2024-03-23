#pragma once

#include <functional>
#include "widgets/widgets.h"

class Application;
class EditWindow;

class EditWindowParameter {
public:
	EditWindowParameter(
		EditWindow& p_edit_window
	);
	virtual void getValue() const = 0;

protected:
	Application& app;
	ContainerWidget* widget = nullptr;

	ContainerWidget* createParameterWidget(
		const std::string& name,
		const std::string& text
	);
	TextBoxWidget* createTextBoxWidget();
	CheckboxWidget* createCheckboxWidget();

private:
	EditWindow& edit_window;
};

class TextParameter : public EditWindowParameter {
public:
	TextParameter(
		EditWindow& p_edit_window,
		const std::string& name,
		const std::string& text,
		std::function<sf::String(void)> get_value,
		std::function<void(const sf::String&)> set_value
	);
	void getValue() const override;

private:
	std::function<sf::String(void)> get_value;
	std::function<void(const sf::String&)> set_value;
	TextBoxWidget* textbox_widget = nullptr;
};

class BoolParameter : public EditWindowParameter {
public:
	BoolParameter(
		EditWindow& p_edit_window,
		const std::string& name,
		const std::string& text,
		std::function<bool(void)> get_value,
		std::function<void(bool)> set_value
	);
	void getValue() const override;

private:
	std::function<bool(void)> get_value;
	std::function<void(bool)> set_value;
	CheckboxWidget* checkbox_widget = nullptr;
};

class FloatParameter : public EditWindowParameter {
public:
	FloatParameter(
		EditWindow& p_edit_window,
		const std::string& name,
		const std::string& text,
		std::function<float(void)> get_value,
		std::function<void(float)> set_value
	);
	void getValue() const override;

private:
	std::function<float(void)> get_value;
	std::function<void(float)> set_value;
	TextBoxWidget* textbox_widget = nullptr;
};

class EditWindow : public ContainerWidget {
public:
	EditWindow(WidgetList& widget_list, Application& p_app);
	void updateParameters();

private:
	friend class EditWindowParameter;
	Application& app;
	CompoundVectorUptr<EditWindowParameter> parameters;

	void createParameters();
	template<typename T, typename... Args>
	requires std::derived_from<T, EditWindowParameter>
	void createParameter(Args&&... args);
};

template<typename T, typename... Args>
requires std::derived_from<T, EditWindowParameter>
inline void EditWindow::createParameter(Args&&... args) {
	std::unique_ptr<T> parameter = std::make_unique<T>(*this, args...);
	parameters.add(std::move(parameter));
}
