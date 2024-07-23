#pragma once

#include "widgets/scroll_area_widget.h"
#include "widgets/tree_view_widget.h"
#include "simulation/objectlist.h"
#include <map>

class Editor;

class Outliner : public fw::ScrollAreaWidget {
public:
	
	const sf::Color OUTLINER_BACKGROUND_COLOR = sf::Color(128, 128, 128);
	const sf::Color OUTLINER_SCROLLBAR_COLOR = sf::Color(110, 110, 110);

	Outliner(fw::WidgetList& widget_list, float width, float height, Editor& p_app);
	Outliner(fw::WidgetList& widget_list, const sf::Vector2f& size, Editor& p_app);

private:
	Editor& app;
	GameObjectList& object_list;
	fw::TreeViewWidget* treeview_widget = nullptr;
	std::map<GameObject*, fw::TreeViewWidget::Entry*> object_entry;
	std::map<fw::TreeViewWidget::Entry*, GameObject*> entry_object;

	void addObject(GameObject* object);
	void removeObject(GameObject* object);
	void setParentToObject(GameObject* object, GameObject* parent);
	void selectEntry(GameObject* object);
	void deselectEntry(GameObject* object);
	void clear();
};
