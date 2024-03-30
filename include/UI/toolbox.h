#pragma once

#include "widgets/widgets.h"

class Editor;

class Toolbox : public fw::ContainerWidget {
public:
	Toolbox(fw::WidgetList& widget_list, Editor& p_app);

private:
	Editor& app;
};
