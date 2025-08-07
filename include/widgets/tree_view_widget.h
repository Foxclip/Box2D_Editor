#pragma once

#include "container_widget.h"
#include "widgets/tree_view_entry.h"

namespace fw {

	const float TREEVIEW_PADDING = 4.0f;
	const float TREEVIEW_MAIN_PANEL_PADDING = 2.0f;
	const float TREEVIEW_BUTTON_PANEL_PADDING = 4.0f;
	const sf::Color TREEVIEW_BACKGROUND_COLOR = sf::Color(128, 128, 128);
	const sf::Color TREEVIEW_TARGET_HIGHLIGHT_COLOR = sf::Color(255, 255, 0);
	const sf::Vector2f TREEVIEW_BUTTON_SIZE = sf::Vector2f(20, 20);
	const sf::Color TREEVIEW_BUTTON_COLOR = sf::Color(175, 175, 175);
	const sf::Color TREEVIEW_BUTTON_ARROW_COLOR = sf::Color(255, 255, 255);

	class TreeViewEntry;
	class TreeViewWidget;
	class ButtonWidget;
	class PolygonWidget;

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
		ContainerWidget* getButtonPanelWidget();
		ContainerWidget* getMainPanelWidget();
		ButtonWidget* getButtonUpWidget();
		ButtonWidget* getButtonDownWidget();
		const CompVector<TreeViewEntry*>& getAllEntries() const;
		const CompVector<TreeViewEntry*>& getTopEntries() const;
		CompVector<TreeViewEntry*> getAllVisibleEntriesInOrder() const;
		size_t getAllEntryCount() const;
		size_t getTopEntryCount() const;
		TreeViewEntryWidget* getGrabbedWidget() const;
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
		friend class TreeViewEntryWidget;
		ContainerWidget* button_panel = nullptr;
		ContainerWidget* main_panel = nullptr;
		ButtonWidget* button_up_widget = nullptr;
		ButtonWidget* button_down_widget = nullptr;
		PolygonWidget* button_arrow_up_widget = nullptr;
		PolygonWidget* button_arrow_down_widget = nullptr;
		CompVectorUptr<TreeViewEntry> all_entries;
		CompVector<TreeViewEntry*> top_entries;
		TreeViewEntry* grabbed_entry = nullptr;
		TreeViewEntryWidget* grabbed_widget = nullptr;
		TreeViewEntry* highlighted_entry = nullptr;

		void deselectAllExceptEntry(TreeViewEntry* except_entry = nullptr);
		void deselectAllExceptSubtree(TreeViewEntry* except_subtree = nullptr);
		void moveEntryToIndex(TreeViewEntry* entry, size_t index);


	};

}
