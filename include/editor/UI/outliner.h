#pragma once

#include "widgets/widgets.h"
#include "editor/simulation/objectlist.h"
#include <map>

const sf::Vector2f OUTLINER_DEFAULT_SIZE = sf::Vector2f(300.0f, 200.0f);
const float OUTLINER_ENTRY_HEIGHT = 30.0f;
const sf::Color OUTLINER_BACKGROUND_COLOR = sf::Color(128, 128, 128);
const sf::Color OUTLINER_ENTRY_BACKGROUND_COLOR = sf::Color(100, 100, 100);
const sf::Color OUTLINER_ENTRY_TEXT_COLOR = sf::Color(255, 255, 255);
const float OUTLINER_CONTAINER_PADDING = 5.0f;

class Editor;

class Outliner : public fw::ScrollAreaWidget {
public:
	Outliner(fw::WidgetList& widget_list, Editor& p_app);

private:
	Editor& app;
	GameObjectList& object_list;
	fw::ContainerWidget* container_widget = nullptr;
	CompVector<Widget*> widgets;
	std::map<GameObject*, Widget*> object_widget;

	void addObject(GameObject* object);
	void removeObject(GameObject* object);
	void clear();
	void updateList();
	fw::RectangleWidget* createEntryWidget(GameObject* object);
};
