#pragma once

#include "widgets/container_widget.h"

const int TOOLBOX_PADDING = 10;

class Editor;

class Toolbox : public fw::ContainerWidget {
public:
	Toolbox(fw::WidgetList& widget_list, Editor& p_app);

private:
	Editor& app;
};
