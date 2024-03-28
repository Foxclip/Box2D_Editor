#pragma once

#include "widgets/widgets.h"

class Editor;

class CreatePanel : public ContainerWidget {
public:
	CreatePanel(WidgetList& widget_list, Editor& p_app);

private:
	Editor& app;
};
