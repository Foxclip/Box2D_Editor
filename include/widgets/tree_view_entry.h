#pragma once

#include <SFML/Graphics.hpp>
#include "common/compvector.h"

namespace fw {

	const float TREEVIEW_ENTRY_HEIGHT = 20.0f;
	const float TREEVIEW_ENTRY_CHILDREN_OFFSET = 20.0f;
	const unsigned int TREEVIEW_ENTRY_FONT_SIZE = 12;
	const sf::Color TREEVIEW_ENTRY_BACKGROUND_COLOR = sf::Color(100, 100, 100);
	const sf::Color TREEVIEW_ENTRY_TEXT_COLOR = sf::Color(255, 255, 255);
	const sf::Color TREEVIEW_ENTRY_SELECTION_COLOR = sf::Color(200, 100, 0);
	const sf::Color TREEVIEW_ENTRY_ARROW_AREA_COLOR = sf::Color(80, 80, 80);
	const sf::Color TREEVIEW_ENTRY_ARROW_COLOR = sf::Color(255, 255, 255);
	const float TREEVIEW_ENTRY_DRAG_DISTANCE = 10.0f;

	class WidgetList;
	class Widget;
	class PolygonWidget;
	class EmptyWidget;
	class TextWidget;
	class TreeViewWidget;
	class ContainerWidget;
	class RectangleWidget;
	class TreeViewEntryWidget;

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
		size_t getIndex() const;
		fw::ContainerWidget* getWidget() const;
		fw::RectangleWidget* getRectangleWidget() const;
		fw::TextWidget* getTextWidget() const;
		fw::RectangleWidget* getArrowAreaWidget() const;
		fw::PolygonWidget* getArrowWidget() const;
		fw::ContainerWidget* getChildrenBoxWidget() const;
		fw::EmptyWidget* getChildrenSpacingWidget() const;
		fw::ContainerWidget* getChildrenWidget() const;
		void processMouseMove(const sf::Vector2f& pos);
		void processMouse(const sf::Vector2f& pos);
		void processLeftPress(const sf::Vector2f& pos);
		void processLeftClick(const sf::Vector2f& pos);
		void processLeftRelease(const sf::Vector2f& pos);
		void processGlobalLeftRelease(const sf::Vector2f& pos);
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
		void dropTo(TreeViewEntry* parent, size_t index);
		void remove(bool with_children);

	private:
		friend TreeViewWidget;
		friend TreeViewEntryWidget;
		friend class PendingEntryMove;
		friend class PendingEntryDelete;
		friend class PendingEntrySetParent;
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
		TreeViewEntryWidget* entry_widget = nullptr;

		void addChild(TreeViewEntry* entry);
		void moveChildToIndex(TreeViewEntry* entry, size_t index);
		void removeChild(TreeViewEntry* entry);

	};

}
