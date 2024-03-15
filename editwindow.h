#pragma once

#include "widget.h"
#include <functional>

class Application;

class EditWindow : public ContainerWidget {
public:
	EditWindow(WidgetList& widget_list, Application& p_app);

private:
	ContainerWidget* createParameterWidget(const std::string& name, const std::string& text);
	TextBoxWidget* createTextBoxWidget();
	ContainerWidget* createFloatParameter(
		const std::string& name,
		const std::string& text,
		std::function<void(float)> set_value
	);
	Application& app;
};
