#include "editor/UI/outliner.h"
#include "widgets/tree_view_widget.h"
#include "widgets/text_widget.h"
#include "editor/editor.h"

Outliner::Outliner(fw::WidgetList& widget_list, float width, float height, Editor& p_app) 
	: ScrollAreaWidget(widget_list, width, height), app(p_app), object_list(p_app.getSimulation()) {
	// outliner
	setName("outliner");
	setOrigin(Anchor::TOP_RIGHT);
	setParentAnchor(Anchor::TOP_RIGHT);
	setBackgroundColor(OUTLINER_BACKGROUND_COLOR);
	setScrollbarColor(OUTLINER_SCROLLBAR_COLOR);
	// container
	treeview_widget = widget_list.createTreeViewWidget(width, height);
	treeview_widget->setFillColor(sf::Color::Transparent);
	treeview_widget->setSizeXPolicy(SizePolicy::PARENT);
	treeview_widget->OnEntryClicked += [&](fw::TreeViewWidget::Entry* entry) {
		GameObject* object = entry_object[entry];
		app.setActiveObject(object);
	};
	treeview_widget->OnEntrySelected += [&](fw::TreeViewWidget::Entry* entry) {
		GameObject* object = entry_object[entry];
		app.select_tool.selectObject(object);
	};
	treeview_widget->OnEntryDeselected += [&](fw::TreeViewWidget::Entry* entry) {
		GameObject* object = entry_object[entry];
		app.select_tool.deselectObject(object);
	};
	setScrolledWidget(treeview_widget);
	object_list.OnObjectAdded += [&](GameObject* object) {
		addObject(object);
	};
	object_list.OnBeforeObjectRemoved += [&](GameObject* object) {
		LoggerTag outlinerTag("outliner");
		logger << "RemoveObject: " << object->getId() << " \"" << object->getName() << "\"" << "\n";
	};
	object_list.OnAfterObjectRemoved += [&](GameObject* object) {
		removeObject(object);
	};
	object_list.OnSetParent += [&](GameObject* object, GameObject* parent) {
		setParentToObject(object, parent);
	};
	object_list.OnObjectMoved += [&](GameObject* object, size_t index) {
		moveObject(object, index);
	};
	object_list.OnClear += [&]() {
		clear();
	};
	app.select_tool.OnObjectSelected += [&](GameObject* object) {
		selectEntry(object);
	};
	app.select_tool.OnObjectDeselected += [&](GameObject* object) {
		deselectEntry(object);
	};
}

Outliner::Outliner(fw::WidgetList& widget_list, const sf::Vector2f& size, Editor& p_app)
	: Outliner(widget_list, size.x, size.y, p_app) { }

void Outliner::addObject(GameObject* object) {
	LoggerTag outlinerTag("outliner");
	logger << "AddObject: " << object->getId() << " \"" << object->getName() << "\"" << "\n";
	fw::TreeViewWidget::Entry* entry = treeview_widget->addEntry(object->getName());
	object_entry[object] = entry;
	entry_object[entry] = object;
}

void Outliner::moveObject(GameObject* object, size_t index) {
	LoggerTag outlinerTag("outliner");
	logger << "MoveObject: " << object->getId() << " \"" << object->getName() << "\": " << index << "\n";
	fw::TreeViewWidget::Entry* entry = object_entry[object];
	entry->moveToIndex(index);
}

void Outliner::removeObject(GameObject* object) {
	auto it = object_entry.find(object);
	if (it != object_entry.end()) {
		fw::TreeViewWidget::Entry* entry = it->second;
		treeview_widget->removeEntry(entry, false);
		object_entry.erase(object);
		entry_object.erase(entry);
	}
}

void Outliner::setParentToObject(GameObject* object, GameObject* parent) {
	LoggerTag outlinerTag("outliner");
	std::string child_str = std::to_string(object->getId()) + " \"" + object->getName() + "\"";
	std::string parent_str = "null";
	if (parent) {
		parent_str = std::to_string(parent->getId()) + " \"" + parent->getName() + "\"";
	}
	logger << "SetParent: " << child_str << " -> " << parent_str << "\n";
	fw::TreeViewWidget::Entry* entry = object_entry[object];
	fw::TreeViewWidget::Entry* parent_entry = object_entry[parent];
	entry->setParent(parent_entry);
}

void Outliner::selectEntry(GameObject* object) {
	auto it = object_entry.find(object);
	if (it != object_entry.end()) {
		it->second->selectSilent();
	}
}

void Outliner::deselectEntry(GameObject* object) {
	auto it = object_entry.find(object);
	if (it != object_entry.end()) {
		it->second->deselectSilent();
	}
}

void Outliner::clear() {
	treeview_widget->clear();
	object_entry = std::map<GameObject*, fw::TreeViewWidget::Entry*>();
	entry_object = std::map<fw::TreeViewWidget::Entry*, GameObject*>();
}
