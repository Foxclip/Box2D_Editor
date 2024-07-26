#include "widgets/tree_view_widget.h"
#include "widgets/widget_list.h"
#include "widgets/polygon_widget.h"
#include "widgets/text_widget.h"

namespace fw {

	TreeViewWidget::TreeViewWidget(WidgetList& widget_list, float width, float height)
		: ContainerWidget(widget_list, width, height) {
		type = WidgetType::TreeView;
		setName("treeview");
		setFillColor(TREEVIEW_BACKGROUND_COLOR);
		setClipChildren(true);
		setClickThrough(false);
		setHorizontal(false);
		setPadding(TREEVIEW_CONTAINER_PADDING);
		setSizeXPolicy(SizePolicy::NONE);
	}

	TreeViewWidget::TreeViewWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: TreeViewWidget(widget_list, size.x, size.y) { }

	void TreeViewWidget::deselectAllExceptEntry(Entry* except_entry) {
		for (size_t i = 0; i < all_entries.size(); i++) {
			Entry* entry = all_entries[i];
			if (entry != except_entry) {
				all_entries[i]->deselect();
			}
		}
	}

	void TreeViewWidget::deselectAllExceptSubtree(Entry* except_subtree) {
		std::function<void(Entry*)> deselect_subtree = [&](Entry* entry) {
			if (entry == except_subtree) {
				return;
			}
			for (size_t i = 0; i < entry->getChildrenCount(); i++) {
				Entry* child = entry->getChild(i);
				deselect_subtree(child);
			}
			entry->deselect();
		};
		for (size_t i = 0; i < top_entries.size(); i++) {
			Entry* entry = top_entries[i];
			deselect_subtree(entry);
		}
	}

	const CompVector<TreeViewWidget::Entry*>& TreeViewWidget::getAllEntries() const {
		return all_entries.getCompVector();
	}

	const CompVector<TreeViewWidget::Entry*>& TreeViewWidget::getTopEntries() const {
		return top_entries;
	}

	size_t TreeViewWidget::getAllEntryCount() const {
		return all_entries.size();
	}

	size_t TreeViewWidget::getTopEntryCount() const {
		return top_entries.size();
	}

	TreeViewWidget::Entry* TreeViewWidget::getFromAll(size_t index) const {
		return all_entries[index];
	}

	TreeViewWidget::Entry* TreeViewWidget::getFromTop(size_t index) const {
		return top_entries[index];
	}

	TreeViewWidget::Entry* TreeViewWidget::addEntry(const sf::String& name) {
		std::unique_ptr<Entry> entry_uptr = std::make_unique<Entry>(*this, name);
		Entry* ptr = entry_uptr.get();
		top_entries.add(ptr);
		all_entries.add(std::move(entry_uptr));
		return ptr;
	}

	void TreeViewWidget::selectAll() {
		for (size_t i = 0; i < top_entries.size(); i++) {
			top_entries[i]->select(true);
		}
	}

	void TreeViewWidget::deselectAll() {
		for (size_t i = 0; i < top_entries.size(); i++) {
			top_entries[i]->deselect(true);
		}
	}

	void TreeViewWidget::expandAll() {
		for (size_t i = 0; i < all_entries.size(); i++) {
			all_entries[i]->expand();
		}
	}

	void TreeViewWidget::collapseAll() {
		for (size_t i = 0; i < all_entries.size(); i++) {
			all_entries[i]->collapse();
		}
	}

	void TreeViewWidget::removeEntry(Entry* entry, bool with_children) {
		Entry* parent = entry->getParent();
		CompVector<Entry*> children_copy = entry->getChildren();
		for (size_t i = 0; i < children_copy.size(); i++) {
			Entry* child = children_copy[i];
			if (with_children) {
				child->remove(true);
			} else {
				child->setParent(parent);
			}
		}
		if (parent) {
			parent->removeChild(entry);
		}
		top_entries.remove(entry);
		all_entries.remove(entry);
	}

	void TreeViewWidget::clear() {
		top_entries.clear();
		all_entries.clear();
	}

	TreeViewWidget::Entry::Entry(TreeViewWidget& treeview, const sf::String& name) : treeview(treeview) {
#ifndef NDEBUG
		this->debug_name = name;
#endif
		this->name = name;
		// container
		entry_widget = treeview.widget_list.createWidget<fw::ContainerWidget>(20.0f, TREEVIEW_ENTRY_HEIGHT);
		entry_widget->setName(name + " entry");
		entry_widget->setHorizontal(false);
		entry_widget->setInnerPaddingY(TREEVIEW_CONTAINER_PADDING);
		entry_widget->setFillColor(sf::Color::Transparent);
		entry_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
		entry_widget->setParent(&treeview);
		// rectangle
		rectangle_widget = treeview.widget_list.createWidget<fw::RectangleWidget>(20.0f, TREEVIEW_ENTRY_HEIGHT);
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_BACKGROUND_COLOR);
		rectangle_widget->setClipChildren(true);
		rectangle_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
		rectangle_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
			bool with_children = treeview.widget_list.isLCtrlPressed();
			if (treeview.widget_list.isLShiftPressed()) {
				toggleSelect(with_children);
			} else {
				if (with_children) {
					treeview.deselectAllExceptSubtree(this);
				} else {
					treeview.deselectAllExceptEntry(this);
				}
				select(with_children);
			}
		};
		rectangle_widget->setParent(entry_widget);
		// arrow area
		arrow_area_widget = treeview.widget_list.createWidget<fw::RectangleWidget>(15.0f, 20.0f);
		arrow_area_widget->setName("arrow area");
		arrow_area_widget->setVisible(false);
		arrow_area_widget->setSizeYPolicy(fw::Widget::SizePolicy::PARENT);
		arrow_area_widget->setFillColor(TREEVIEW_ENTRY_ARROW_AREA_COLOR);
		arrow_area_widget->setClickThrough(false);
		arrow_area_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
			toggle();
		};
		arrow_area_widget->setParent(rectangle_widget);
		// arrow
		std::vector<sf::Vector2f> vertices = {
			sf::Vector2f(5.0, 0.0f),
			sf::Vector2f(-3.0, 5.0f),
			sf::Vector2f(-3.0, -5.0f),
		};
		arrow_widget = treeview.widget_list.createWidget<fw::PolygonWidget>(vertices);
		arrow_widget->setName("arrow");
		arrow_widget->setFillColor(TREEVIEW_ENTRY_ARROW_COLOR);
		arrow_widget->setParentAnchor(fw::Widget::Anchor::CENTER);
		arrow_widget->setParent(arrow_area_widget);
		// text
		fw::TextWidget* text_widget = treeview.widget_list.createWidget<fw::TextWidget>();
		text_widget->setCharacterSize(TREEVIEW_ENTRY_FONT_SIZE);
		text_widget->setFillColor(TREEVIEW_ENTRY_TEXT_COLOR);
		text_widget->setString(name);
		text_widget->setOrigin(Anchor::CENTER_LEFT);
		text_widget->setParentAnchor(Anchor::CENTER_LEFT);
		text_widget->setAnchorOffsetX(18.0f);
		text_widget->setParent(rectangle_widget);
		// children box
		children_box_widget = treeview.widget_list.createWidget<fw::ContainerWidget>(20.0f, TREEVIEW_ENTRY_HEIGHT);
		children_box_widget->setName("children box");
		children_box_widget->setVisible(false);
		children_box_widget->setRenderable(false);
		children_box_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
		children_box_widget->setParent(entry_widget);
		// spacing
		children_spacing_widget = treeview.widget_list.createWidget<fw::EmptyWidget>();
		children_spacing_widget->setName("spacing");
		children_spacing_widget->setSize(TREEVIEW_ENTRY_CHILDREN_OFFSET, 1.0f);
		children_spacing_widget->setParent(children_box_widget);
		// children
		children_widget = treeview.widget_list.createWidget<fw::ContainerWidget>(20.0f, TREEVIEW_ENTRY_HEIGHT);
		children_widget->setName("children");
		children_widget->setHorizontal(false);
		children_widget->setInnerPaddingY(TREEVIEW_CONTAINER_PADDING);
		children_widget->setRenderable(false);
		children_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
		children_widget->setParent(children_box_widget);
	}

	TreeViewWidget::Entry::~Entry() {
		entry_widget->remove();
	}

	bool TreeViewWidget::Entry::isExpanded() const {
		return expanded;
	}

	TreeViewWidget::Entry* TreeViewWidget::Entry::getParent() const {
		return parent;
	}

	const CompVector<TreeViewWidget::Entry*>& TreeViewWidget::Entry::getChildren() const {
		return children;
	}

	TreeViewWidget::Entry* TreeViewWidget::Entry::getChild(size_t index) const {
		return children[index];
	}

	size_t TreeViewWidget::Entry::getChildrenCount() const {
		return children.size();
	}

	fw::ContainerWidget* TreeViewWidget::Entry::getWidget() const {
		return entry_widget;
	}

	fw::RectangleWidget* TreeViewWidget::Entry::getRectangleWidget() const {
		return rectangle_widget;
	}

	fw::RectangleWidget* TreeViewWidget::Entry::getArrowAreaWidget() const {
		return arrow_area_widget;
	}

	fw::PolygonWidget* TreeViewWidget::Entry::getArrowWidget() const {
		return arrow_widget;
	}

	fw::ContainerWidget* TreeViewWidget::Entry::getChildrenBoxWidget() const {
		return children_box_widget;
	}

	fw::EmptyWidget* TreeViewWidget::Entry::getChildrenSpacingWidget() const {
		return children_spacing_widget;
	}

	fw::ContainerWidget* TreeViewWidget::Entry::getChildrenWidget() const {
		return children_widget;
	}

	void TreeViewWidget::Entry::selectSilent(bool with_children) {
		selected = true;
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_SELECTION_COLOR);
		if (with_children) {
			for (size_t i = 0; i < children.size(); i++) {
				Entry* child = children[i];
				child->select(true);
			}
		}
	}

	void TreeViewWidget::Entry::deselectSilent(bool with_children) {
		selected = false;
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_BACKGROUND_COLOR);
		if (with_children) {
			for (size_t i = 0; i < children.size(); i++) {
				Entry* child = children[i];
				child->deselect(true);
			}
		}
	}

	void TreeViewWidget::Entry::toggleSelectSilent(bool with_children) {
		if (selected) {
			deselectSilent(with_children);
		} else {
			selectSilent(with_children);
		}
	}

	void TreeViewWidget::Entry::select(bool with_children) {
		bool was_selected = selected;
		selectSilent(with_children);
		if (!was_selected) {
			treeview.OnEntrySelected(this);
		}
	}

	void TreeViewWidget::Entry::deselect(bool with_children) {
		bool was_selected = selected;
		deselectSilent(with_children);
		if (was_selected) {
			treeview.OnEntryDeselected(this);
		}
	}

	void TreeViewWidget::Entry::toggleSelect(bool with_children) {
		if (selected) {
			deselect(with_children);
		} else {
			select(with_children);
		}
	}

	void TreeViewWidget::Entry::setParent(Entry* new_parent) {
		Entry* old_parent = parent;
		if (old_parent) {
			old_parent->removeChild(this);
		}
		if (new_parent) {
			new_parent->addChild(this);
			fw::ContainerWidget* parent_children_widget = new_parent->getChildrenWidget();
			entry_widget->setParent(parent_children_widget);
			treeview.top_entries.remove(this);
		} else {
			entry_widget->setParent(&treeview);
			treeview.top_entries.add(this);
		}
		this->parent = new_parent;
	}

	void TreeViewWidget::Entry::expand() {
		if (children.size() > 0) {
			expanded = true;
			arrow_widget->setRotation(90.0f);
			children_box_widget->setVisible(true);
		}
	}

	void TreeViewWidget::Entry::collapse() {
		expanded = false;
		arrow_widget->setRotation(0.0f);
		children_box_widget->setVisible(false);
	}

	void TreeViewWidget::Entry::toggle() {
		if (expanded) {
			collapse();
		} else {
			expand();
		}
	}

	void TreeViewWidget::Entry::remove(bool with_children) {
		treeview.removeEntry(this, with_children);
	}

	void TreeViewWidget::Entry::updateWidgets() {
		if (getChildrenCount() == 0) {
			getArrowAreaWidget()->setVisible(false);
			getChildrenBoxWidget()->setVisible(false);
		} else {
			getArrowAreaWidget()->setVisible(true);
			getChildrenBoxWidget()->setVisible(expanded);
		}
	}

	void TreeViewWidget::Entry::addChild(Entry* entry) {
		children.add(entry);
		updateWidgets();
	}

	void TreeViewWidget::Entry::removeChild(Entry* entry) {
		children.remove(entry);
		updateWidgets();
	}

}
