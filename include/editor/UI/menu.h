#pragma once

#include "widgets/container_widget.h"

const int MENU_PADDING = 10;
const float MENU_BUTTON_WIDTH = 60.0f;
const float MENU_BUTTON_HEIGHT = 30.0f;
const int MENU_BUTTON_TEXT_SIZE = 16;

class Editor;
namespace fw {
	class RectangleWidget;
}

class Menu : public fw::ContainerWidget {
public:
	Menu(fw::WidgetList& widget_list, Editor& p_app);

private:
	Editor& app;

	fw::RectangleWidget* createButton(const std::string& name, const std::string& display_name);
};
