#pragma once

#include "widget.h"

class Application;

class Toolbox : public ContainerWidget {
public:
	Toolbox(WidgetList& widget_list);

private:
	Application& app;
};
