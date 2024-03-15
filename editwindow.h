#pragma once

#include "widget.h"
#include <functional>

class Application;

class EditWindow : public ContainerWidget {
public:
	EditWindow(WidgetList& widget_list, Application& p_app);
	void updateParameters();

private:
	Application& app;

	void createParameters();
	ContainerWidget* createParameterWidget(const std::string& name, const std::string& text);
	TextBoxWidget* createTextBoxWidget();
	ContainerWidget* createTextParameter(
		const std::string& name,
		const std::string& text,
		std::function<void(const sf::String&)> set_value
	);
	ContainerWidget* createBoolParameter(
		const std::string& name,
		const std::string& text,
		std::function<void(bool)> set_value
	);
	ContainerWidget* createFloatParameter(
		const std::string& name,
		const std::string& text,
		std::function<void(float)> set_value
	);
	void setCheckBoxValue(const std::string& name, bool value);
	void setTextBoxValue(const std::string& name, const sf::String& value);
	void setFloatValue(const std::string& name, float value);
};
