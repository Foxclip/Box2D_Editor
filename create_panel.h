#pragma once

#include "widget.h"

class Application;

class CreatePanel : public ContainerWidget {
public:
	CreatePanel(WidgetList& widget_list);

private:
	Application& app;
};
