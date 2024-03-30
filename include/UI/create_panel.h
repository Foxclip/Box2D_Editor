#pragma once

#include "widgets/widgets.h"

class Editor;

class CreatePanel : public fw::ContainerWidget {
public:
	CreatePanel(fw::WidgetList& widget_list, Editor& p_app);

private:
	Editor& app;
};
