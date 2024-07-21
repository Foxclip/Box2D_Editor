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
			Event<const sf::Vector2f&> OnLeftPress;

			Entry(TreeViewWidget& treeview, const sf::String& name);
			~Entry();
			fw::ContainerWidget* getWidget() const;
			fw::RectangleWidget* getRectangleWidget() const;
			fw::RectangleWidget* getArrowAreaWidget() const;
			fw::PolygonWidget* getArrowWidget() const;
			fw::ContainerWidget* getChildrenBoxWidget() const;
			fw::EmptyWidget* getChildrenSpacingWidget() const;
			fw::ContainerWidget* getChildrenWidget() const;
			void select();
			void deselect();
		private:
			TreeViewWidget& treeview;
			fw::ContainerWidget* entry_widget = nullptr;
			fw::RectangleWidget* rectangle_widget = nullptr;
			fw::RectangleWidget* arrow_area_widget = nullptr;
			fw::PolygonWidget* arrow_widget = nullptr;
			fw::ContainerWidget* children_box_widget = nullptr;
			fw::EmptyWidget* children_spacing_widget = nullptr;
			fw::ContainerWidget* children_widget = nullptr;

		};
		TreeViewWidget(WidgetList& widget_list, float width, float height);
		TreeViewWidget(WidgetList& widget_list, const sf::Vector2f& size);
		Entry* getEntry(size_t index) const;
		Entry* addEntry(const sf::String& name);
		void removeEntry(Entry* entry);
		void setParentToEntry(Entry* child_entry, Entry* parent_entry);
		void clear();

	protected:


	private:
		friend class Entry;
		fw::ContainerWidget* container_widget = nullptr;
		CompVectorUptr<Entry> entries;

	};

}