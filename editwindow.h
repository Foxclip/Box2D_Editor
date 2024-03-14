#pragma once

#include "widget.h"

class Application;

class EditWindow : public ContainerWidget {
public:
	EditWindow(WidgetList& widget_list);

private:
	ContainerWidget* createParameterWidget(const std::string& name, const std::string& text);
	Application& app;
};
