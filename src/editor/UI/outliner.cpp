#include "editor/UI/outliner.h"
#include "widgets/text_widget.h"
#include "editor/editor.h"

Outliner::Outliner(fw::WidgetList& widget_list, Editor& p_app) 
	: ScrollAreaWidget(widget_list, OUTLINER_DEFAULT_SIZE), app(p_app), object_list(p_app.getSimulation()) {
	// outliner
	setName("outliner");
	setOrigin(Anchor::TOP_RIGHT);
	setParentAnchor(Anchor::TOP_RIGHT);
	setBackgroundColor(OUTLINER_BACKGROUND_COLOR);
	setScrollbarColor(OUTLINER_SCROLLBAR_COLOR);
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
	object_list.OnAfterObjectRemoved += [&](GameObject* object) {
		removeObject(object);
	};
	object_list.OnParentSet += [&](GameObject* object, GameObject* parent) {
		setParentToObject(object, parent);
	};
	object_list.OnClear += [&]() {
		clear();
	};
	app.select_tool.OnObjectSelected += [&](GameObject* object) {
		selectObject(object);
	};
	app.select_tool.OnObjectDeselected += [&](GameObject* object) {
		deselectObject(object);
	};
}

void Outliner::addObject(GameObject* object) {
	logger << "AddObject: " << object->getId() << " \"" << object->getName() << "\"" << "\n";
	fw::ContainerWidget* entry_widget = createEntryWidget(object);
	entry_widget->setParent(container_widget);
	std::unique_ptr<Entry> entry_uptr = std::make_unique<Entry>(object, entry_widget);
	object_entry[object] = entry_uptr.get();
	entries.add(std::move(entry_uptr));
}

void Outliner::removeObject(GameObject* object) {
	auto it = object_entry.find(object);
	if (it != object_entry.end()) {
		Entry* entry = it->second;
		entries.remove(entry);
		object_entry.erase(object);
	}
}

void Outliner::setParentToObject(GameObject* object, GameObject* parent) {
	std::string child_str = std::to_string(object->getId()) + " \"" + object->getName() + "\"";
	std::string parent_str = "null";
	if (parent) {
		parent_str = std::to_string(parent->getId()) + " \"" + parent->getName() + "\"";
	}
	logger << "SetParent: " << child_str << " -> " << parent_str << "\n";
	Entry* entry = object_entry[object];
	fw::ContainerWidget* object_widget = entry->getWidget();
	if (parent) {
		Entry* parent_entry = object_entry[parent];
		fw::ContainerWidget* children_widget = parent_entry->getChildrenWidget();
		object_widget->setParent(children_widget);
	} else {
		object_widget->setParent(container_widget);
	}
}

void Outliner::selectObject(GameObject* object) {
	auto it = object_entry.find(object);
	if (it != object_entry.end()) {
		it->second->select();
	}
}

void Outliner::deselectObject(GameObject* object) {
	auto it = object_entry.find(object);
	if (it != object_entry.end()) {
		it->second->deselect();
	}
}

void Outliner::clear() {
	entries.clear();
	object_entry.clear();
}

fw::ContainerWidget* Outliner::createEntryWidget(GameObject* object) {
	sf::String name = object->getName();
	// container
	fw::ContainerWidget* entry_container_widget = widget_list.createWidget<fw::ContainerWidget>(20.0f, OUTLINER_ENTRY_HEIGHT);
	entry_container_widget->setName(name + " entry");
	entry_container_widget->setHorizontal(false);
	entry_container_widget->setFillColor(sf::Color::Transparent);
	entry_container_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
	// rectangle
	fw::RectangleWidget* entry_rectangle_widget = widget_list.createWidget<fw::RectangleWidget>(20.0f, OUTLINER_ENTRY_HEIGHT);
	entry_rectangle_widget->setFillColor(OUTLINER_ENTRY_BACKGROUND_COLOR);
	entry_rectangle_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
	entry_rectangle_widget->OnLeftPress += [&, object](const sf::Vector2f& pos) {
		app.setActiveObject(object);
		bool with_children = app.isLCtrlPressed();
		if (app.isLShiftPressed()) {
			app.select_tool.toggleSelect(object, with_children);
		} else {
			app.select_tool.selectSingleObject(object, with_children);
		}
	};
	entry_rectangle_widget->setParent(entry_container_widget);
	// text
	fw::TextWidget* text_widget = widget_list.createWidget<fw::TextWidget>();
	text_widget->setCharacterSize(OUTLINER_ENTRY_FONT_SIZE);
	text_widget->setFillColor(OUTLINER_ENTRY_TEXT_COLOR);
	text_widget->setString(name);
	text_widget->setOrigin(Anchor::CENTER_LEFT);
	text_widget->setParentAnchor(Anchor::CENTER_LEFT);
	text_widget->setParent(entry_rectangle_widget);
	// children
	fw::ContainerWidget* entry_children_widget = widget_list.createWidget<fw::ContainerWidget>(20.0f, OUTLINER_ENTRY_HEIGHT);
	entry_children_widget->setName("children");
	entry_children_widget->setHorizontal(false);
	entry_children_widget->setFillColor(sf::Color::Red);
	entry_children_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
	entry_children_widget->setParent(entry_container_widget);
	return entry_container_widget;
}

Outliner::Entry::Entry(GameObject* object, fw::ContainerWidget* widget) {
	this->object = object;
	this->widget = widget;
	this->rectangle_widget = dynamic_cast<fw::RectangleWidget*>(widget->find("rectangle"));
	mAssert(this->rectangle_widget);
	this->children_widget = dynamic_cast<fw::ContainerWidget*>(widget->find("children"));
	mAssert(this->children_widget);
}

Outliner::Entry::~Entry() {
	widget->remove();
}

fw::ContainerWidget* Outliner::Entry::getWidget() const {
	return widget;
}

fw::RectangleWidget* Outliner::Entry::getRectangleWidget() const {
	return rectangle_widget;
}

fw::ContainerWidget* Outliner::Entry::getChildrenWidget() const {
	return children_widget;
}

void Outliner::Entry::select() {
	rectangle_widget->setFillColor(OUTLINER_ENTRY_SELECTION_COLOR);
}

void Outliner::Entry::deselect() {
	rectangle_widget->setFillColor(OUTLINER_ENTRY_BACKGROUND_COLOR);
}
