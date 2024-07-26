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

	class WidgetList;
	class PolygonWidget;
	class EmptyWidget;

	class TreeViewWidget : public ContainerWidget {
	public:
		class Entry {
		public:
			Entry(TreeViewWidget& treeview, const sf::String& name);
			~Entry();
			bool isExpanded() const;
			Entry* getParent() const;
			const CompVector<Entry*>& getChildren() const;
			Entry* getChild(size_t index) const;
			size_t getChildrenCount() const;
			fw::ContainerWidget* getWidget() const;
			fw::RectangleWidget* getRectangleWidget() const;
			fw::RectangleWidget* getArrowAreaWidget() const;
			fw::PolygonWidget* getArrowWidget() const;
			fw::ContainerWidget* getChildrenBoxWidget() const;
			fw::EmptyWidget* getChildrenSpacingWidget() const;
			fw::ContainerWidget* getChildrenWidget() const;
			void selectSilent(bool with_children = false);
			void deselectSilent(bool with_children = false);
			void toggleSelectSilent(bool with_children = false);
			void select(bool with_children = false);
			void deselect(bool with_children = false);
			void toggleSelect(bool with_children = false);
			void setParent(Entry* new_parent);
			void expand();
			void collapse();
			void toggle();
			void remove(bool with_children);
		private:
			friend TreeViewWidget;
			TreeViewWidget& treeview;
#ifndef NDEBUG
			std::string debug_name;
#endif
			sf::String name;
			Entry* parent = nullptr;
			CompVector<Entry*> children;
			bool expanded = false;
			bool selected = false;
			fw::ContainerWidget* entry_widget = nullptr;
			fw::RectangleWidget* rectangle_widget = nullptr;
			fw::RectangleWidget* arrow_area_widget = nullptr;
			fw::PolygonWidget* arrow_widget = nullptr;
			fw::ContainerWidget* children_box_widget = nullptr;
			fw::EmptyWidget* children_spacing_widget = nullptr;
			fw::ContainerWidget* children_widget = nullptr;

			void updateWidgets();
			void addChild(Entry* entry);
			void removeChild(Entry* entry);

		};

		Event<Entry*> OnEntrySelected;
		Event<Entry*> OnEntryDeselected;

		TreeViewWidget(WidgetList& widget_list, float width, float height);
		TreeViewWidget(WidgetList& widget_list, const sf::Vector2f& size);
		const CompVector<Entry*>& getAllEntries() const;
		const CompVector<Entry*>& getTopEntries() const;
		size_t getAllEntryCount() const;
		size_t getTopEntryCount() const;
		Entry* getFromAll(size_t index) const;
		Entry* getFromTop(size_t index) const;
		Entry* addEntry(const sf::String& name);
		void selectAll();
		void deselectAll();
		void expandAll();
		void collapseAll();
		void removeEntry(Entry* entry, bool with_children);
		void clear();

	protected:

	private:
		friend class Entry;
		CompVectorUptr<Entry> all_entries;
		CompVector<Entry*> top_entries;

		void deselectAllExceptEntry(Entry* except_entry = nullptr);
		void deselectAllExceptSubtree(Entry* except_subtree = nullptr);


	};

}
