#include "editor/UI/outliner.h"
#include "editor/editor.h"

Outliner::Outliner(fw::WidgetList& widget_list, Editor& p_app) 
	: ScrollAreaWidget(widget_list, OUTLINER_DEFAULT_SIZE), app(p_app), object_list(p_app.getSimulation()) {
	// outliner
	setName("outliner");
	setOrigin(Anchor::TOP_RIGHT);
	setParentAnchor(Anchor::TOP_RIGHT);
	setBackgroundColor(OUTLINER_BACKGROUND_COLOR);
	// container
	container_widget = widget_list.createWidget<fw::ContainerWidget>(OUTLINER_DEFAULT_SIZE);
	setScrolledWidget(container_widget);
	container_widget->setFillColor(sf::Color::Transparent);
	container_widget->setHorizontal(false);
	container_widget->setPadding(OUTLINER_CONTAINER_PADDING);
	container_widget->setSizeXPolicy(SizePolicy::PARENT);

	object_list.OnObjectAdded += [&](GameObject* object) {
		addObject(object);
	};
	object_list.OnObjectRemoved += [&](GameObject* object) {
		removeObject(object);
	};
	object_list.OnClear += [&]() {
		clear();
	};
}

void Outliner::addObject(GameObject* object) {
	fw::RectangleWidget* entry_widget = createEntryWidget(object);
	entry_widget->setParent(container_widget);
	widgets.add(entry_widget);
	object_widget[object] = entry_widget;
}

void Outliner::removeObject(GameObject* object) {
	Widget* entry_widget = object_widget[object];
	entry_widget->remove();
	widgets.remove(entry_widget);
	object_widget.erase(object);
}

void Outliner::clear() {
	for (size_t i = 0; i < widgets.size(); i++) {
		Widget* widget = widgets[i];
		widget->remove();
	}
	widgets.clear();
	object_widget.clear();
}

void Outliner::updateList() {
	for (size_t i = 0; i < object_list.getTopSize(); i++) {
		GameObject* object = object_list.getFromTop(i);
		addObject(object);
	}
}

fw::RectangleWidget* Outliner::createEntryWidget(GameObject* object) {
	sf::String name = object->getName();
	fw::RectangleWidget* entry_widget = widget_list.createWidget<fw::RectangleWidget>(20.0f, OUTLINER_ENTRY_HEIGHT);
	entry_widget->setName(name + " entry");
	entry_widget->setFillColor(OUTLINER_ENTRY_BACKGROUND_COLOR);
	entry_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
	fw::TextWidget* text_widget = widget_list.createWidget<fw::TextWidget>();
	text_widget->setCharacterSize(15);
	text_widget->setFillColor(OUTLINER_ENTRY_TEXT_COLOR);
	text_widget->setString(name);
	text_widget->setOrigin(Anchor::CENTER_LEFT);
	text_widget->setParentAnchor(Anchor::CENTER_LEFT);
	text_widget->setParent(entry_widget);
	return entry_widget;
}
