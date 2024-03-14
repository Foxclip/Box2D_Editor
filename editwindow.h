#pragma once

#include "widget.h"

class Application;

class EditWindow : public ContainerWidget {
public:
	EditWindow(WidgetList& widget_list, Application& p_app);

private:
	ContainerWidget* createParameterWidget(const std::string& name, const std::string& text);
	TextBoxWidget* createTextBoxWidget();
	Application& app;
};
