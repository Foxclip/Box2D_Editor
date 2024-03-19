#pragma once

#include "../widgets/widgets.h"

class Application;

class Toolbox : public ContainerWidget {
public:
	Toolbox(WidgetList& widget_list, Application& p_app);

private:
	Application& app;
};
