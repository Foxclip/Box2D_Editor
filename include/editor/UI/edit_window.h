#pragma once

#include <functional>
#include "widgets/window_widget.h"

const float EDIT_WINDOW_PARAMETER_PADDING = 10.0f;

class Editor;
class EditWindow;
namespace fw {
	class ContainerWidget;
	class TextBoxWidget;
	class CheckboxWidget;
	class DropdownWidget;
}

class EditWindowParameter {
public:
	EditWindowParameter(
		EditWindow& p_edit_window
	);
	virtual void getValue() const = 0;
	fw::ContainerWidget* getWidget() const;

protected:
	Editor& app;
	fw::ContainerWidget* widget = nullptr;

	fw::ContainerWidget* createParameterWidget(
		const std::string& name,
		const std::string& text
	);
	fw::TextBoxWidget* createTextBoxWidget();
	fw::CheckboxWidget* createCheckboxWidget();
	fw::DropdownWidget* createDropdownWidget();

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

class ListParameter : public EditWindowParameter {
public:
	ListParameter(
		EditWindow& p_edit_window,
		const std::string& name,
		const std::string& text,
		const std::vector<std::string>& value_list,
		std::function<ptrdiff_t(void)> get_value,
		std::function<void(ptrdiff_t)> set_value
	);
	void getValue() const override;

private:
	std::function<ptrdiff_t(void)> get_value;
	std::function<void(ptrdiff_t)> set_value;
	fw::DropdownWidget* dropdown_widget = nullptr;
};

class EditWindow : public fw::WindowWidget {
public:
	EditWindow(fw::WidgetList& widget_list, float width, float height, Editor& p_app);
	EditWindow(fw::WidgetList& widget_list, const sf::Vector2f& size, Editor& p_app);
	void updateParameters();

private:
	friend class EditWindowParameter;
	Editor& app;
	fw::ContainerWidget* container_widget = nullptr;
	CompVectorUptr<EditWindowParameter> parameters;

	void createParameters();
	void setSpacingWidgets();
	template<typename T, typename... Args>
	requires std::derived_from<T, EditWindowParameter>
	void createParameter(Args&&... args);
};

template<typename T, typename... Args>
requires std::derived_from<T, EditWindowParameter>
inline void EditWindow::createParameter(Args&&... args) {
	DataPointerUnique<T> parameter = make_data_pointer<T>("EditWindowParameter", *this, args...);
	parameters.add(std::move(parameter));
}
