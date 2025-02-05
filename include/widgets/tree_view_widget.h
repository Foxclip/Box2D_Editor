#pragma once

#include "container_widget.h"
#include "widgets/tree_view_entry.h"

namespace fw {

	const float TREEVIEW_CONTAINER_PADDING = 2.0f;
	const sf::Color TREEVIEW_BACKGROUND_COLOR = sf::Color(128, 128, 128);
	const sf::Color TREEVIEW_TARGET_HIGHLIGHT_COLOR = sf::Color(255, 255, 0);

	class TreeViewEntry;
	class TreeViewWidget;

	struct TreeviewTargetHighlight {
		bool visible = false;
		sf::Vector2f pos = sf::Vector2f(0, 0);
		sf::Vector2f size = sf::Vector2f(0, 0);
	};

	class TreeViewWidget : public ContainerWidget {
	public:
		Event<TreeViewEntry*> OnEntryClicked;
		Event<TreeViewEntry*> OnEntrySelected;
		Event<TreeViewEntry*> OnEntryDeselected;
		static TreeviewTargetHighlight target_highlight;

		TreeViewWidget(WidgetList& widget_list, float width, float height);
		TreeViewWidget(WidgetList& widget_list, const sf::Vector2f& size);
		TreeViewWidget(const TreeViewWidget& other);
		const CompVector<TreeViewEntry*>& getAllEntries() const;
		const CompVector<TreeViewEntry*>& getTopEntries() const;
		CompVector<TreeViewEntry*> getAllVisibleEntriesInOrder() const;
		size_t getAllEntryCount() const;
		size_t getTopEntryCount() const;
		TreeViewEntry* getEntry(size_t index) const;
		TreeViewEntry* getTopEntry(size_t index) const;
		TreeViewEntry* addEntry(const sf::String& name);
		void selectAll();
		void deselectAll();
		void expandAll();
		void collapseAll();
		TreeViewEntry* getTargetHighlightEntry(const sf::Vector2f& global_pos) const;
		void putTargetHighlight();
		void removeEntry(TreeViewEntry* entry, bool with_children);
		void clear();
		TreeViewWidget* clone(bool with_children = true) override;

	protected:

	private:
		friend class TreeViewEntry;
		friend class PendingEntryMove;
		friend class PendingEntryDelete;
		friend class PendingEntrySetParent;
		friend class PendingEntryDetach;
		CompVectorUptr<TreeViewEntry> all_entries;
		CompVector<TreeViewEntry*> top_entries;
		TreeViewEntry* grabbed_entry = nullptr;
		TreeViewEntry* grabbed_entry_original_parent = nullptr;
		size_t grabbed_entry_original_index = 0;
		TreeViewEntry* highlighted_entry = nullptr;

		void deselectAllExceptEntry(TreeViewEntry* except_entry = nullptr);
		void deselectAllExceptSubtree(TreeViewEntry* except_subtree = nullptr);
		void moveEntryToIndex(TreeViewEntry* entry, size_t index);


	};

}
