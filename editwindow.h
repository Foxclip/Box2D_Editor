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

	ContainerWidget* createParameterWidget(const std::string& name, const std::string& text);
	TextBoxWidget* createTextBoxWidget();
	ContainerWidget* createFloatParameter(
		const std::string& name,
		const std::string& text,
		std::function<void(float)> set_value
	);
	void setCheckBoxValue(const std::string& name, bool value);
	void setTextBoxValue(const std::string& name, const sf::String& value);
	void setFloatValue(const std::string& name, float value);
};
