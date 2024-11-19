#pragma once

#include "container_widget.h"

namespace fw {

	const float TREEVIEW_ENTRY_HEIGHT = 20.0f;
	const float TREEVIEW_ENTRY_CHILDREN_OFFSET = 20.0f;
	const unsigned int TREEVIEW_ENTRY_FONT_SIZE = 12;
	const sf::Color TREEVIEW_BACKGROUND_COLOR = sf::Color(128, 128, 128);
	const sf::Color TREEVIEW_ENTRY_BACKGROUND_COLOR = sf::Color(100, 100, 100);
	const sf::Color TREEVIEW_ENTRY_TEXT_COLOR = sf::Color(255, 255, 255);
	const sf::Color TREEVIEW_ENTRY_SELECTION_COLOR = sf::Color(200, 100, 0);
	const sf::Color TREEVIEW_ENTRY_ARROW_AREA_COLOR = sf::Color(80, 80, 80);
	const sf::Color TREEVIEW_ENTRY_ARROW_COLOR = sf::Color(255, 255, 255);
	const float TREEVIEW_CONTAINER_PADDING = 2.0f;
	const float TREEVIEW_ENTRY_DRAG_DISTANCE = 20.0f;
	const sf::Color TREEVIEW_TARGET_HIGHLIGHT_COLOR = sf::Color(255, 255, 0);

	class WidgetList;
	class PolygonWidget;
	class EmptyWidget;
	class TextWidget;

	class TreeViewWidget;

	class TreeViewEntry {
	public:
		TreeViewEntry(TreeViewWidget& treeview, const sf::String& name);
		virtual ~TreeViewEntry();
		bool isExpanded() const;
		bool isGrabbed() const;
		TreeViewEntry* getParent() const;
		const CompVector<TreeViewEntry*> getParentChain() const;
		const CompVector<TreeViewEntry*>& getChildren() const;
		TreeViewEntry* getChild(size_t index) const;
		size_t getChildrenCount() const;
		size_t getIndex(TreeViewEntry* skip = nullptr) const;
		fw::ContainerWidget* getWidget() const;
		fw::RectangleWidget* getRectangleWidget() const;
		fw::TextWidget* getTextWidget() const;
		fw::RectangleWidget* getArrowAreaWidget() const;
		fw::PolygonWidget* getArrowWidget() const;
		fw::ContainerWidget* getChildrenBoxWidget() const;
		fw::EmptyWidget* getChildrenSpacingWidget() const;
		fw::ContainerWidget* getChildrenWidget() const;
		void selectSilent(bool with_children = false);
		void deselectSilent(bool with_children = false);
		void toggleSelectSilent(bool with_children = false);
		void click();
		void select(bool with_children = false);
		void deselect(bool with_children = false);
		void toggleSelect(bool with_children = false);
		void setParent(TreeViewEntry* new_parent, bool reparent_widget = true);
		void setWidgetParent(TreeViewEntry* new_parent);
		void moveToIndex(size_t index);
		void expand();
		void collapse();
		void toggle();
		void take();
		void drop();
		void remove(bool with_children);
	private:
		friend TreeViewWidget;
		TreeViewWidget& treeview;
#ifndef NDEBUG
		std::string debug_name;
#endif
		sf::String name;
		TreeViewEntry* parent = nullptr;
		CompVector<TreeViewEntry*> children;
		bool pressed = false;
		bool expanded = false;
		bool selected = false;
		bool grabbed = false;
		bool grab_begin = true;
		sf::Vector2f grab_offset;
		fw::ContainerWidget* entry_widget = nullptr;
		fw::RectangleWidget* rectangle_widget = nullptr;
		fw::TextWidget* text_widget = nullptr;
		fw::RectangleWidget* arrow_area_widget = nullptr;
		fw::PolygonWidget* arrow_widget = nullptr;
		fw::ContainerWidget* children_box_widget = nullptr;
		fw::EmptyWidget* children_spacing_widget = nullptr;
		fw::ContainerWidget* children_widget = nullptr;

		void updateWidgets();
		void addChild(TreeViewEntry* entry);
		void moveChildToIndex(TreeViewEntry* entry, size_t index);
		void removeChild(TreeViewEntry* entry);

	};

	class TreeViewWidget : public ContainerWidget {
	public:
		Event<TreeViewEntry*> OnEntryClicked;
		Event<TreeViewEntry*> OnEntrySelected;
		Event<TreeViewEntry*> OnEntryDeselected;

		TreeViewWidget(WidgetList& widget_list, float width, float height);
		TreeViewWidget(WidgetList& widget_list, const sf::Vector2f& size);
		const CompVector<TreeViewEntry*>& getAllEntries() const;
		const CompVector<TreeViewEntry*>& getTopEntries() const;
		CompVector<TreeViewEntry*> getAllEntriesInOrder() const;
		size_t getAllEntryCount() const;
		size_t getTopEntryCount() const;
		TreeViewEntry* getEntry(size_t index) const;
		TreeViewEntry* getTopEntry(size_t index) const;
		TreeViewEntry* addEntry(const sf::String& name);
		void selectAll();
		void deselectAll();
		void expandAll();
		void collapseAll();
		RectangleWidget* getTargetHighlightWidget() const;
		TreeViewEntry* getTargetHighlightEntry(const sf::Vector2f& global_pos) const;
		void putTargetHighlight();
		void removeEntry(TreeViewEntry* entry, bool with_children);
		void clear();

	protected:

	private:
		friend class TreeViewEntry;
		CompVectorUptr<TreeViewEntry> all_entries;
		CompVector<TreeViewEntry*> top_entries;
		TreeViewEntry* grabbed_entry = nullptr;
		fw::RectangleWidget* target_highlight_widget = nullptr;
		TreeViewEntry* highlighted_entry = nullptr;

		void deselectAllExceptEntry(TreeViewEntry* except_entry = nullptr);
		void deselectAllExceptSubtree(TreeViewEntry* except_subtree = nullptr);
		void moveEntryToIndex(TreeViewEntry* entry, size_t index);


	};

}
