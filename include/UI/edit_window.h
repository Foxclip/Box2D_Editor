#pragma once

#include <functional>
#include "widgets/widgets.h"

class Editor;
class EditWindow;

class EditWindowParameter {
public:
	EditWindowParameter(
		EditWindow& p_edit_window
	);
	virtual void getValue() const = 0;

protected:
	Editor& app;
	fw::ContainerWidget* widget = nullptr;

	fw::ContainerWidget* createParameterWidget(
		const std::string& name,
		const std::string& text
	);
	fw::TextBoxWidget* createTextBoxWidget();
	fw::CheckboxWidget* createCheckboxWidget();

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
	fw::TextBoxWidget* textbox_widget = nullptr;
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
	fw::CheckboxWidget* checkbox_widget = nullptr;
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
	fw::TextBoxWidget* textbox_widget = nullptr;
};

class EditWindow : public fw::ContainerWidget {
public:
	EditWindow(fw::WidgetList& widget_list, Editor& p_app);
	void updateParameters();

private:
	friend class EditWindowParameter;
	Editor& app;
	CompVectorUptr<EditWindowParameter> parameters;

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
