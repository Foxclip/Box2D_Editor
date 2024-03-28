#pragma once

#include "widgets/widgets.h"

class Editor;

class Toolbox : public ContainerWidget {
public:
	Toolbox(WidgetList& widget_list, Editor& p_app);

private:
	Editor& app;
};
