#pragma once

#include "widgets/widgets.h"

class Application;

class CreatePanel : public ContainerWidget {
public:
	CreatePanel(WidgetList& widget_list, Application& p_app);

private:
	Application& app;
};
