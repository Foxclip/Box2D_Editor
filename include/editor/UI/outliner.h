#pragma once

#include "widgets/rectangle_widget.h"
#include <widgets/polygon_widget.h>
#include "widgets/container_widget.h"
#include "widgets/scroll_area_widget.h"
#include "simulation/objectlist.h"
#include <map>

const sf::Vector2f OUTLINER_DEFAULT_SIZE = sf::Vector2f(300.0f, 200.0f);
const float OUTLINER_ENTRY_HEIGHT = 20.0f;
const float OUTLINER_ENTRY_CHILDREN_OFFSET = 20.0f;
const unsigned int OUTLINER_ENTRY_FONT_SIZE = 12;
const sf::Color OUTLINER_BACKGROUND_COLOR = sf::Color(128, 128, 128);
const sf::Color OUTLINER_SCROLLBAR_COLOR = sf::Color(110, 110, 110);
const sf::Color OUTLINER_ENTRY_BACKGROUND_COLOR = sf::Color(100, 100, 100);
const sf::Color OUTLINER_ENTRY_TEXT_COLOR = sf::Color(255, 255, 255);
const sf::Color OUTLINER_ENTRY_SELECTION_COLOR = sf::Color(200, 100, 0);
const float OUTLINER_CONTAINER_PADDING = 2.0f;

class Editor;

class Outliner : public fw::ScrollAreaWidget {
public:
	class Entry {
	public:
		Entry(Outliner& outliner, GameObject* object);
		~Entry();
		fw::ContainerWidget* getWidget() const;
		fw::RectangleWidget* getRectangleWidget() const;
		fw::PolygonWidget* getArrowWidget() const;
		fw::ContainerWidget* getChildrenBoxWidget() const;
		fw::EmptyWidget* getChildrenSpacingWidget() const;
		fw::ContainerWidget* getChildrenWidget() const;
		void select();
		void deselect();
	private:
		Outliner& outliner;
		GameObject* object = nullptr;
		fw::ContainerWidget* entry_widget = nullptr;
		fw::RectangleWidget* rectangle_widget = nullptr;
		fw::PolygonWidget* arrow_widget = nullptr;
		fw::ContainerWidget* children_box_widget = nullptr;
		fw::EmptyWidget* children_spacing_widget = nullptr;
		fw::ContainerWidget* children_widget = nullptr;

	};

	Outliner(fw::WidgetList& widget_list, Editor& p_app);

private:
	friend class Entry;
	Editor& app;
	GameObjectList& object_list;
	fw::ContainerWidget* container_widget = nullptr;
	CompVectorUptr<Entry> entries;
	std::map<GameObject*, Entry*> object_entry;

	void addObject(GameObject* object);
	void removeObject(GameObject* object);
	void setParentToObject(GameObject* object, GameObject* parent);
	void selectObject(GameObject* object);
	void deselectObject(GameObject* object);
	void clear();
};
