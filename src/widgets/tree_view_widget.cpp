#include "widgets/tree_view_widget.h"
#include "widgets/widget_list.h"

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
		OnLeftClick += [&](const sf::Vector2f& pos) {
			deselectAll();
		};
		widget_list.OnProcessAfterInput += [&]() {
			executePendingOperations();
		};

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

	void TreeViewWidget::addPendingEntryMove(TreeViewEntry* entry, size_t index) {
		wAssert(!widget_list.isLocked());
		DataPointer<PendingEntryMove> uptr = make_data_pointer<PendingEntryMove>(
			"PendingEntryMove " + entry->name + " " + std::to_string(index), *this, entry, index
		);
		pending_entry_move.add(std::move(uptr));
	}

	void TreeViewWidget::addPendingEntryDelete(TreeViewEntry* entry, bool with_children) {
		wAssert(!widget_list.isLocked());
		DataPointer<PendingEntryDelete> uptr = make_data_pointer<PendingEntryDelete>(
			"PendingEntryDelete " + entry->name + " " + std::to_string(with_children), *this, entry, with_children
		);
		pending_entry_delete.add(std::move(uptr));
	}

	void TreeViewWidget::addPendingEntrySetParent(TreeViewEntry* entry, TreeViewEntry* new_parent, ptrdiff_t move_to_index) {
		wAssert(!widget_list.isLocked());
		sf::String parent_name = new_parent ? new_parent->name : "null";
		DataPointer<PendingEntrySetParent> uptr = make_data_pointer<PendingEntrySetParent>(
			"PendingEntrySetParent " + entry->name + " " + parent_name, *this, entry, new_parent, move_to_index
		);
		pending_entry_setparent.add(std::move(uptr));
	}

	void TreeViewWidget::addPendingDetach(TreeViewEntry* entry) {
		wAssert(!widget_list.isLocked());
		DataPointer<PendingEntryDetach> uptr = make_data_pointer<PendingEntryDetach>(
			"PendingEntryDetach " + entry->name, *this, entry
		);
		pending_entry_detach.add(std::move(uptr));
	}

	void TreeViewWidget::executePendingOperations() {
		for (PendingEntryMove* op : pending_entry_move) {
			op->execute();
		}
		for (PendingEntrySetParent* op : pending_entry_setparent) {
			op->execute();
		}
		for (PendingEntryDelete* op : pending_entry_delete) {
			op->execute();
		}
		for (PendingEntryDetach* op : pending_entry_detach) {
			op->execute();
		}
		pending_entry_move.clear();
		pending_entry_delete.clear();
		pending_entry_setparent.clear();
		pending_entry_detach.clear();
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

	TreeViewEntry* TreeViewWidget::getTargetHighlightEntry(const sf::Vector2f& global_pos) const {
		CompVector<TreeViewEntry*> entries = getAllVisibleEntriesInOrder();
		TreeViewEntry* result = nullptr;
		size_t current_index = 0;
		for (TreeViewEntry* entry : entries) {
			// TODO: cache parent chain
			if (entry->getWidget()->isVisible() && !entry->grabbed && !entry->getParentChain().contains(grabbed_entry)) {
				sf::Vector2f center = entry->getRectangleWidget()->getGlobalCenter();
				if (global_pos.y < center.y) {
					result = entry;
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
		highlighted_entry = getTargetHighlightEntry(mouse_pos);
		if (highlighted_entry) {
			sf::Vector2f entry_pos = highlighted_entry->getWidget()->getGlobalPosition();
			target_highlight_widget->setGlobalPosition(entry_pos);
			target_highlight_widget->setOrigin(Anchor::BOTTOM_LEFT);
			target_highlight_widget->setWidth(highlighted_entry->getWidget()->getWidth());
		} else {
			target_highlight_widget->setOrigin(Anchor::TOP_LEFT);
			target_highlight_widget->setWidth(getContentWidth());
			if (children.empty()) {
				target_highlight_widget->setGlobalPosition(getGlobalPosition() + sf::Vector2f(getLeftPadding(), 0.0f));
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

	PendingEntryOperation::PendingEntryOperation(TreeViewWidget& tree_view_widget) : tree_view_widget(tree_view_widget) {
	}

	PendingEntryMove::PendingEntryMove(TreeViewWidget& tree_view_widget, TreeViewEntry* entry, size_t index) : PendingEntryOperation(tree_view_widget) {
		this->entry = entry;
		this->index = index;
	}

	void PendingEntryMove::execute() {
		wAssert(tree_view_widget.all_entries.contains(entry));
		wAssert(tree_view_widget.all_entries.contains(entry->getParent()));
		TreeViewEntry* parent = entry->getParent();
		parent->moveChildToIndex(entry, index);
	}

	PendingEntryDelete::PendingEntryDelete(TreeViewWidget& tree_view_widget, TreeViewEntry* entry, bool with_children) : PendingEntryOperation(tree_view_widget) {
		this->entry = entry;
		this->with_children = with_children;
	}

	void PendingEntryDelete::execute() {
		wAssert(tree_view_widget.all_entries.contains(entry));
		tree_view_widget.removeEntry(entry, with_children);
	}

	PendingEntrySetParent::PendingEntrySetParent(
		TreeViewWidget& tree_view_widget,
		TreeViewEntry* entry,
		TreeViewEntry* new_parent,
		ptrdiff_t move_to_index
	) : PendingEntryOperation(tree_view_widget) {
		this->entry = entry;
		this->new_parent = new_parent;
		this->move_to_index = move_to_index;
	}

	void PendingEntrySetParent::execute() {
		wAssert(tree_view_widget.all_entries.contains(entry));
		wAssert(new_parent == nullptr || tree_view_widget.all_entries.contains(new_parent));
		entry->setParent(new_parent);
		if (move_to_index >= 0) {
			entry->moveToIndex(move_to_index);
		}
	}

	PendingEntryDetach::PendingEntryDetach(TreeViewWidget& tree_view_widget, TreeViewEntry* entry) : PendingEntryOperation(tree_view_widget) {
		this->entry = entry;
	}

	void PendingEntryDetach::execute() {
		wAssert(tree_view_widget.all_entries.contains(entry));
		entry->setParent(nullptr, false);
		entry->getWidget()->setParentKeepPos(nullptr);
		tree_view_widget.top_entries.remove(entry);
	}

}
