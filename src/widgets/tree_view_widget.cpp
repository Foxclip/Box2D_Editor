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

		// target highlight
		target_highlight_widget = widget_list.createRectangleWidget(100.0f, TREEVIEW_CONTAINER_PADDING);
		target_highlight_widget->setName("target highlight");
		target_highlight_widget->setVisible(false);
		target_highlight_widget->setFillColor(TREEVIEW_TARGET_HIGHLIGHT_COLOR);
		target_highlight_widget->setParent(this);
	}

	TreeViewWidget::TreeViewWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: TreeViewWidget(widget_list, size.x, size.y) { }

	void TreeViewWidget::deselectAllExceptEntry(TreeViewEntry* except_entry) {
		wAssert(all_entries.contains(except_entry));
		for (size_t i = 0; i < all_entries.size(); i++) {
			TreeViewEntry* entry = all_entries[i];
			if (entry != except_entry) {
				all_entries[i]->deselect();
			}
		}
	}

	void TreeViewWidget::deselectAllExceptSubtree(TreeViewEntry* except_subtree) {
		wAssert(all_entries.contains(except_subtree));
		std::function<void(TreeViewEntry*)> deselect_subtree = [&](TreeViewEntry* entry) {
			if (entry == except_subtree) {
				return;
			}
			for (size_t i = 0; i < entry->getChildrenCount(); i++) {
				TreeViewEntry* child = entry->getChild(i);
				deselect_subtree(child);
			}
			entry->deselect();
		};
		for (size_t i = 0; i < top_entries.size(); i++) {
			TreeViewEntry* entry = top_entries[i];
			deselect_subtree(entry);
		}
	}

	void TreeViewWidget::moveEntryToIndex(TreeViewEntry* entry, size_t index) {
		wAssert(top_entries.contains(entry));
		top_entries.moveValueToIndex(entry, index);
	}

	const CompVector<TreeViewEntry*>& TreeViewWidget::getAllEntries() const {
		return all_entries.getCompVector();
	}

	const CompVector<TreeViewEntry*>& TreeViewWidget::getTopEntries() const {
		return top_entries;
	}

	CompVector<TreeViewEntry*> TreeViewWidget::getAllEntriesInOrder() const {
		CompVector<TreeViewEntry*> entries;
		std::function<void(TreeViewEntry*)> add_to_list = [&](TreeViewEntry* entry) {
			entries.add(entry);
			for (TreeViewEntry* child : entry->getChildren()) {
				add_to_list(child);
			}
		};
		for (TreeViewEntry* entry : top_entries) {
			add_to_list(entry);
		}
		return entries;
	}

	size_t TreeViewWidget::getAllEntryCount() const {
		return all_entries.size();
	}

	size_t TreeViewWidget::getTopEntryCount() const {
		return top_entries.size();
	}

	TreeViewEntry* TreeViewWidget::getEntry(size_t index) const {
		return all_entries[index];
	}

	TreeViewEntry* TreeViewWidget::getTopEntry(size_t index) const {
		return top_entries[index];
	}

	TreeViewEntry* TreeViewWidget::addEntry(const sf::String& name) {
		DataPointer<TreeViewEntry> entry_uptr = make_data_pointer<TreeViewEntry>("TreeViewWidget Entry " + name, *this, name);
		TreeViewEntry* ptr = entry_uptr.get();
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

	RectangleWidget* TreeViewWidget::getTargetHighlightWidget() const {
		return target_highlight_widget;
	}

	TreeViewEntry* TreeViewWidget::getTargetHighlightEntry(const sf::Vector2f& global_pos, ptrdiff_t& index) const {
		CompVector<TreeViewEntry*> entries = getAllEntriesInOrder();
		TreeViewEntry* result = nullptr;
		index = -1;
		size_t current_index = 0;
		for (TreeViewEntry* entry : entries) {
			if (entry->getWidget()->isVisible() && !entry->grabbed) {
				sf::Vector2f center = entry->getWidget()->getGlobalCenter();
				if (global_pos.y < center.y) {
					result = entry;
					index = current_index;
					break;
				}
				current_index++;
			}
		}
		return result;
	}

	void TreeViewWidget::putTargetHighlight() {
		target_highlight_widget->setVisible(true);
		sf::Vector2f mouse_pos = widget_list.getMousePosf();
		highlighted_entry = getTargetHighlightEntry(mouse_pos, highlighted_entry_index);
		if (highlighted_entry) {
			sf::Vector2f entry_pos = highlighted_entry->getWidget()->getGlobalPosition();
			target_highlight_widget->setGlobalPosition(entry_pos);
			target_highlight_widget->setOrigin(Anchor::BOTTOM_LEFT);
			target_highlight_widget->setWidth(highlighted_entry->getWidget()->getWidth());
		} else {
			target_highlight_widget->setOrigin(Anchor::TOP_LEFT);
			target_highlight_widget->setWidth(getContentWidth());
			if (children.empty()) {
				target_highlight_widget->setGlobalPosition(getGlobalPosition());
			} else {
				Widget* last_entry_widget = children.back();
				sf::Vector2f last_entry_widget_pos = last_entry_widget->getGlobalBottomLeft();
				target_highlight_widget->setGlobalPosition(last_entry_widget_pos);
			}
		}
	}

	void TreeViewWidget::removeEntry(TreeViewEntry* entry, bool with_children) {
		wAssert(all_entries.contains(entry));
		TreeViewEntry* parent = entry->getParent();
		CompVector<TreeViewEntry*> children_copy = entry->getChildren();
		ptrdiff_t index = -1;
		if (!with_children) {
			index = entry->getIndex();
		}
		for (size_t i = 0; i < children_copy.size(); i++) {
			TreeViewEntry* child = children_copy[i];
			if (with_children) {
				child->remove(true);
			} else {
				child->setParent(parent);
				child->moveToIndex(index + i);
			}
		}
		if (parent) {
			parent->removeChild(entry);
		}
		top_entries.remove(entry);
		all_entries.remove(entry);
	}

	void TreeViewWidget::clear() {
		for (ptrdiff_t i = top_entries.size() - 1; i >= 0; i--) {
			TreeViewEntry* entry = top_entries[i];
			entry->remove(true);
		}
	}

	TreeViewEntry::TreeViewEntry(TreeViewWidget& treeview, const sf::String& name) : treeview(treeview) {
#ifndef NDEBUG
		this->debug_name = name;
#endif
		this->name = name;
		// container
		entry_widget = treeview.widget_list.createContainerWidget(20.0f, TREEVIEW_ENTRY_HEIGHT);
		entry_widget->setName(name + " entry");
		entry_widget->setHorizontal(false);
		entry_widget->setInnerPaddingY(TREEVIEW_CONTAINER_PADDING);
		entry_widget->setFillColor(sf::Color::Transparent);
		entry_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
		entry_widget->setParent(&treeview);
		entry_widget->OnMouseMoved += [&](const sf::Vector2f& pos) {
			if (pressed && !grabbed) {
				sf::Vector2f rel_pos = entry_widget->getRelativeMousePos() - grab_offset;
				if (length(rel_pos) >= TREEVIEW_ENTRY_DRAG_DISTANCE) {
					take();
				}
			}
		};
		entry_widget->OnProcessMouse += [&](const sf::Vector2f& pos) {
			if (grabbed) {
				sf::Vector2f new_pos = pos - grab_offset;
				entry_widget->setGlobalPosition(new_pos);
				if (!grab_begin) {
					treeview.putTargetHighlight();
				}
			}
			grab_begin = false;
		};
		entry_widget->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
			if (grabbed) {
				drop();
			}
		};
		// rectangle
		rectangle_widget = treeview.widget_list.createRectangleWidget(20.0f, TREEVIEW_ENTRY_HEIGHT);
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_BACKGROUND_COLOR);
		rectangle_widget->setClipChildren(true);
		rectangle_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
		rectangle_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
			pressed = true;
			grab_offset = entry_widget->getRelativeMousePos();
		};
		rectangle_widget->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
			pressed = false;
		};
		rectangle_widget->OnLeftClick += [&](const sf::Vector2f& pos) {
			if (!grabbed) {
				click();
			}
		};
		rectangle_widget->setParent(entry_widget);
		// arrow area
		arrow_area_widget = treeview.widget_list.createRectangleWidget(15.0f, 20.0f);
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
		arrow_widget = treeview.widget_list.createPolygonWidget(vertices);
		arrow_widget->setName("arrow");
		arrow_widget->setFillColor(TREEVIEW_ENTRY_ARROW_COLOR);
		arrow_widget->setParentAnchor(fw::Widget::Anchor::CENTER);
		arrow_widget->setParent(arrow_area_widget);
		// text
		text_widget = treeview.widget_list.createTextWidget();
		text_widget->setCharacterSize(TREEVIEW_ENTRY_FONT_SIZE);
		text_widget->setFillColor(TREEVIEW_ENTRY_TEXT_COLOR);
		text_widget->setString(name);
		text_widget->setOrigin(Widget::Anchor::CENTER_LEFT);
		text_widget->setParentAnchor(Widget::Anchor::CENTER_LEFT);
		text_widget->setAnchorOffsetX(18.0f);
		text_widget->setParent(rectangle_widget);
		// children box
		children_box_widget = treeview.widget_list.createContainerWidget(20.0f, TREEVIEW_ENTRY_HEIGHT);
		children_box_widget->setName("children box");
		children_box_widget->setVisible(false);
		children_box_widget->setRenderable(false);
		children_box_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
		children_box_widget->setParent(entry_widget);
		// spacing
		children_spacing_widget = treeview.widget_list.createEmptyWidget();
		children_spacing_widget->setName("spacing");
		children_spacing_widget->setSize(TREEVIEW_ENTRY_CHILDREN_OFFSET, 1.0f);
		children_spacing_widget->setParent(children_box_widget);
		// children
		children_widget = treeview.widget_list.createContainerWidget(20.0f, TREEVIEW_ENTRY_HEIGHT);
		children_widget->setName("children");
		children_widget->setHorizontal(false);
		children_widget->setInnerPaddingY(TREEVIEW_CONTAINER_PADDING);
		children_widget->setRenderable(false);
		children_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
		children_widget->setParent(children_box_widget);
	}

	TreeViewEntry::~TreeViewEntry() {
		if (!treeview.widget_list.isBeingDestroyed()) {
			entry_widget->remove();
		}
	}

	bool TreeViewEntry::isExpanded() const {
		return expanded;
	}

	bool TreeViewEntry::isGrabbed() const {
		return grabbed;
	}

	TreeViewEntry* TreeViewEntry::getParent() const {
		return parent;
	}

	const CompVector<TreeViewEntry*>& TreeViewEntry::getChildren() const {
		return children;
	}

	TreeViewEntry* TreeViewEntry::getChild(size_t index) const {
		return children[index];
	}

	size_t TreeViewEntry::getChildrenCount() const {
		return children.size();
	}

	size_t TreeViewEntry::getIndex() const {
		if (parent) {
			return parent->children.getIndex(const_cast<TreeViewEntry*>(this));
		} else {
			return treeview.top_entries.getIndex(const_cast<TreeViewEntry*>(this));
		}
	}

	fw::ContainerWidget* TreeViewEntry::getWidget() const {
		return entry_widget;
	}

	fw::RectangleWidget* TreeViewEntry::getRectangleWidget() const {
		return rectangle_widget;
	}

	fw::TextWidget* TreeViewEntry::getTextWidget() const {
		return text_widget;
	}

	fw::RectangleWidget* TreeViewEntry::getArrowAreaWidget() const {
		return arrow_area_widget;
	}

	fw::PolygonWidget* TreeViewEntry::getArrowWidget() const {
		return arrow_widget;
	}

	fw::ContainerWidget* TreeViewEntry::getChildrenBoxWidget() const {
		return children_box_widget;
	}

	fw::EmptyWidget* TreeViewEntry::getChildrenSpacingWidget() const {
		return children_spacing_widget;
	}

	fw::ContainerWidget* TreeViewEntry::getChildrenWidget() const {
		return children_widget;
	}

	void TreeViewEntry::selectSilent(bool with_children) {
		selected = true;
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_SELECTION_COLOR);
		if (with_children) {
			for (size_t i = 0; i < children.size(); i++) {
				TreeViewEntry* child = children[i];
				child->select(true);
			}
		}
	}

	void TreeViewEntry::deselectSilent(bool with_children) {
		selected = false;
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_BACKGROUND_COLOR);
		if (with_children) {
			for (size_t i = 0; i < children.size(); i++) {
				TreeViewEntry* child = children[i];
				child->deselect(true);
			}
		}
	}

	void TreeViewEntry::toggleSelectSilent(bool with_children) {
		if (selected) {
			deselectSilent(with_children);
		} else {
			selectSilent(with_children);
		}
	}

	void TreeViewEntry::click() {
		treeview.OnEntryClicked(this);
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
	}

	void TreeViewEntry::select(bool with_children) {
		bool was_selected = selected;
		selectSilent(with_children);
		if (!was_selected) {
			treeview.OnEntrySelected(this);
		}
	}

	void TreeViewEntry::deselect(bool with_children) {
		bool was_selected = selected;
		deselectSilent(with_children);
		if (was_selected) {
			treeview.OnEntryDeselected(this);
		}
	}

	void TreeViewEntry::toggleSelect(bool with_children) {
		if (selected) {
			deselect(with_children);
		} else {
			select(with_children);
		}
	}

	void TreeViewEntry::setParent(TreeViewEntry* new_parent) {
		if (new_parent) {
			wAssert(treeview.all_entries.contains(new_parent));
		}
		TreeViewEntry* old_parent = parent;
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

	void TreeViewEntry::moveToIndex(size_t index) {
		if (parent) {
			parent->moveChildToIndex(this, index);
		} else {
			treeview.moveEntryToIndex(this, index);
		}
		entry_widget->moveToIndex(index);
	}

	void TreeViewEntry::expand() {
		expanded = true;
		updateWidgets();
	}

	void TreeViewEntry::collapse() {
		expanded = false;
		updateWidgets();
	}

	void TreeViewEntry::toggle() {
		if (expanded) {
			collapse();
		} else {
			expand();
		}
	}

	void TreeViewEntry::take() {
		treeview.widget_list.addPendingSetParent(entry_widget, treeview.getParent(), true);
		entry_widget->setParentAnchor(Widget::Anchor::CUSTOM);
		entry_widget->setSizeXPolicy(Widget::SizePolicy::NONE);
		sf::Color rect_color = rectangle_widget->getFillColor();
		rect_color.a /= 2;
		rectangle_widget->setFillColor(rect_color);
		sf::Color text_color = text_widget->getFillColor();
		text_color.a /= 2;
		text_widget->setFillColor(text_color);
		grabbed = true;
		grab_begin = true;
		treeview.grabbed_widget = entry_widget;
		treeview.widget_list.addPendingSetParent(treeview.target_highlight_widget, treeview.getParent());
	}

	void TreeViewEntry::drop() {
		if (treeview.highlighted_entry_index >= 0) {
			moveToIndex(treeview.highlighted_entry_index);
		} else {
			moveToIndex(treeview.getAllEntryCount());
		}
		treeview.widget_list.addPendingSetParent(entry_widget, &treeview, false, treeview.highlighted_entry_index);
		entry_widget->setParentAnchor(Widget::Anchor::TOP_LEFT);
		entry_widget->setSizeXPolicy(Widget::SizePolicy::PARENT);
		sf::Color rect_color = rectangle_widget->getFillColor();
		rect_color.a = 255;
		rectangle_widget->setFillColor(rect_color);
		sf::Color text_color = text_widget->getFillColor();
		text_color.a = 255;
		text_widget->setFillColor(text_color);
		grabbed = false;
		grab_begin = false;
		treeview.grabbed_widget = nullptr;
		treeview.widget_list.addPendingSetParent(treeview.target_highlight_widget, &treeview);
		treeview.target_highlight_widget->setVisible(false);
	}

	void TreeViewEntry::remove(bool with_children) {
		treeview.removeEntry(this, with_children);
	}

	void TreeViewEntry::updateWidgets() {
		if (getChildrenCount() == 0) {
			getArrowAreaWidget()->setVisible(false);
			getChildrenBoxWidget()->setVisible(false);
		} else {
			getArrowAreaWidget()->setVisible(true);
			if (expanded) {
				arrow_widget->setRotation(90.0f);
				getChildrenBoxWidget()->setVisible(true);
			} else {
				arrow_widget->setRotation(0.0f);
				getChildrenBoxWidget()->setVisible(false);
			}
		}
	}

	void TreeViewEntry::addChild(TreeViewEntry* entry) {
		wAssert(treeview.all_entries.contains(entry));
		children.add(entry);
		updateWidgets();
	}

	void TreeViewEntry::moveChildToIndex(TreeViewEntry* entry, size_t index) {
		wAssert(children.contains(entry));
		children.moveValueToIndex(entry, index);
	}

	void TreeViewEntry::removeChild(TreeViewEntry* entry) {
		wAssert(children.contains(entry));
		children.remove(entry);
		updateWidgets();
	}

}
