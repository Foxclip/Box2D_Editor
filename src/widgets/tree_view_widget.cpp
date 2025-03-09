#include "widgets/tree_view_widget.h"
#include "widgets/widget_list.h"
#include "widgets/button_widget.h"

namespace fw {

	TreeviewTargetHighlight TreeViewWidget::target_highlight;

	TreeViewWidget::TreeViewWidget(WidgetList& widget_list, float width, float height)
		: ContainerWidget(widget_list, width, height) {
		// treeview
		type = WidgetType::TreeView;
		setName("treeview");
		setFillColor(TREEVIEW_BACKGROUND_COLOR);
		setClipChildren(true);
		setClickThrough(false);
		setHorizontal(false);
		setPadding(TREEVIEW_PADDING);
		setSizeXPolicy(SizePolicy::NONE);

		// button panel
		button_panel = widget_list.createContainerWidget(width, height);
		button_panel->setFillColor(sf::Color::Transparent);
		button_panel->setSizeXPolicy(SizePolicy::PARENT);
		button_panel->setInnerPaddingX(TREEVIEW_BUTTON_PANEL_PADDING);
		button_panel->setParent(this);

		// button up
		button_up_widget = widget_list.createButtonWidget(TREEVIEW_BUTTON_SIZE);
		button_up_widget->setNormalColor(TREEVIEW_BUTTON_COLOR);
		button_up_widget->setParent(button_panel);

		// button down
		button_down_widget = widget_list.createButtonWidget(TREEVIEW_BUTTON_SIZE);
		button_down_widget->setNormalColor(TREEVIEW_BUTTON_COLOR);
		button_down_widget->setParent(button_panel);

		// main panel
		main_panel = widget_list.createContainerWidget(width, height);
		main_panel->setFillColor(sf::Color::Transparent);
		main_panel->setHorizontal(false);
		main_panel->setInnerPaddingY(TREEVIEW_MAIN_PANEL_PADDING);
		main_panel->setSizeXPolicy(SizePolicy::PARENT);
		main_panel->setParent(this);
		main_panel->OnLeftClick += [&](const sf::Vector2f& pos) {
			deselectAll();
		};

		widget_list.OnKeyPressed += [&](const sf::Keyboard::Key& key) {
			if (key == sf::Keyboard::Escape) {
				if (grabbed_entry) {
					grabbed_entry->releaseGrab();
				}
			}
		};
	}

	TreeViewWidget::TreeViewWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: TreeViewWidget(widget_list, size.x, size.y) { }

	TreeViewWidget::TreeViewWidget(const TreeViewWidget& other)
		: ContainerWidget(other) {
		std::function<void(TreeViewEntry*)> addEntryWithChildren = [&](TreeViewEntry* entry) {
			addEntry(entry->name);
			for (TreeViewEntry* child : entry->getChildren()) {
				addEntryWithChildren(child);
			}
		};
		for (TreeViewEntry* entry : other.top_entries) {
			addEntryWithChildren(entry);
		}
	}

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

	CompVector<TreeViewEntry*> TreeViewWidget::getAllVisibleEntriesInOrder() const {
		CompVector<TreeViewEntry*> entries;
		std::function<void(TreeViewEntry*)> add_to_list = [&](TreeViewEntry* entry) {
			entries.add(entry);
			if (entry->isExpanded()) {
				for (TreeViewEntry* child : entry->getChildren()) {
					add_to_list(child);
				}
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

	TreeViewEntryWidget* TreeViewWidget::getGrabbedWidget() const {
		return grabbed_widget;
	}

	TreeViewEntry* TreeViewWidget::getEntry(size_t index) const {
		return all_entries[index];
	}

	TreeViewEntry* TreeViewWidget::getTopEntry(size_t index) const {
		return top_entries[index];
	}

	TreeViewEntry* TreeViewWidget::addEntry(const sf::String& name) {
		dp::DataPointerUnique<TreeViewEntry> entry_uptr = dp::make_data_pointer<TreeViewEntry>("TreeViewWidget Entry " + name, *this, name);
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

	TreeViewEntry* TreeViewWidget::getTargetHighlightEntry(const sf::Vector2f& global_pos) const {
		CompVector<TreeViewEntry*> entries = getAllVisibleEntriesInOrder();
		TreeViewEntry* result = nullptr;
		size_t current_index = 0;
		for (size_t i = 0; i < entries.size(); i++) {
			TreeViewEntry* entry = entries[i];
			if (entry->getWidget()->isVisible()) {
				sf::Vector2f center = entry->getRectangleWidget()->getGlobalCenter();
				if (global_pos.y < center.y) {
					result = entry;
					break;
				}
			}
		}
		return result;
	}

	void TreeViewWidget::putTargetHighlight() {
		target_highlight.visible = true;
		sf::Vector2f mouse_pos = widget_list.getMousePosf();
		highlighted_entry = getTargetHighlightEntry(mouse_pos);
		if (highlighted_entry) {
			sf::Vector2f entry_pos = highlighted_entry->getWidget()->getGlobalPosition();
			target_highlight.pos = entry_pos - sf::Vector2f(0.0f, TREEVIEW_MAIN_PANEL_PADDING);
			target_highlight.size = sf::Vector2f(highlighted_entry->getWidget()->getWidth(), TREEVIEW_MAIN_PANEL_PADDING);
		} else {
			target_highlight.size = sf::Vector2f(getContentWidth(), TREEVIEW_MAIN_PANEL_PADDING);
			if (children.empty()) {
				target_highlight.pos = getGlobalPosition() + sf::Vector2f(getLeftPadding(), 0.0f);
			} else {
				Widget* last_entry_widget = children.back();
				sf::Vector2f last_entry_widget_pos = last_entry_widget->getGlobalBottomLeft();
				target_highlight.pos = last_entry_widget_pos;
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

	TreeViewWidget* TreeViewWidget::clone(bool with_children) {
		return widget_list.duplicateWidget(this, with_children);
	}

}
