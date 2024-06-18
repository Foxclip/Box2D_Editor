#include "editor/UI/outliner.h"
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
	object_list.OnObjectRemoved += [&](GameObject* object) {
		removeObject(object);
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
	fw::RectangleWidget* entry_widget = createEntryWidget(object);
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

fw::RectangleWidget* Outliner::createEntryWidget(GameObject* object) {
	sf::String name = object->getName();
	fw::RectangleWidget* entry_widget = widget_list.createWidget<fw::RectangleWidget>(20.0f, OUTLINER_ENTRY_HEIGHT);
	entry_widget->setName(name + " entry");
	entry_widget->setFillColor(OUTLINER_ENTRY_BACKGROUND_COLOR);
	entry_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
	entry_widget->OnLeftPress += [&, object](const sf::Vector2f& pos) {
		app.setActiveObject(object);
		bool with_children = app.isLCtrlPressed();
		if (app.isLShiftPressed()) {
			app.select_tool.toggleSelect(object, with_children);
		} else {
			app.select_tool.selectSingleObject(object, with_children);
		}
	};
	fw::TextWidget* text_widget = widget_list.createWidget<fw::TextWidget>();
	text_widget->setCharacterSize(OUTLINER_ENTRY_FONT_SIZE);
	text_widget->setFillColor(OUTLINER_ENTRY_TEXT_COLOR);
	text_widget->setString(name);
	text_widget->setOrigin(Anchor::CENTER_LEFT);
	text_widget->setParentAnchor(Anchor::CENTER_LEFT);
	text_widget->setParent(entry_widget);
	return entry_widget;
}

Outliner::Entry::Entry(GameObject* object, fw::RectangleWidget* widget) {
	this->object = object;
	this->widget = widget;
}

Outliner::Entry::~Entry() {
	widget->remove();
}

void Outliner::Entry::select() {
	widget->setFillColor(OUTLINER_ENTRY_SELECTION_COLOR);
}

void Outliner::Entry::deselect() {
	widget->setFillColor(OUTLINER_ENTRY_BACKGROUND_COLOR);
}
