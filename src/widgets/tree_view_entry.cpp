#include "widgets/tree_view_entry.h"
#include "widgets/tree_view_entry_widget.h"
#include "widgets/tree_view_widget.h"
#include "widgets/widget_list.h"
#include "widgets/polygon_widget.h"
#include "widgets/text_widget.h"

namespace fw {

	TreeViewEntry::TreeViewEntry(TreeViewWidget& treeview, const sf::String& name) : treeview(treeview) {
#ifndef NDEBUG
		this->debug_name = name;
#endif
		this->name = name;
		entry_widget = treeview.widget_list.createWidget<TreeViewEntryWidget>(*this, name, 20.0f, TREEVIEW_ENTRY_HEIGHT);
		entry_widget->setParent(&treeview);
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

	const CompVector<TreeViewEntry*> TreeViewEntry::getParentChain() const {
		CompVector<TreeViewEntry*> chain;
		const TreeViewEntry* cur = this;
		while (cur) {
			if (cur->parent) {
				chain.add(cur->parent);
				cur = cur->parent;
			} else {
				break;
			}
		}
		return chain;
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
		CompVector<fw::TreeViewEntry*>* parent_list = &treeview.top_entries;
		if (parent) {
			parent_list = &parent->children;
		}
		return (*parent_list).getIndex(const_cast<TreeViewEntry*>(this));
	}

	fw::ContainerWidget* TreeViewEntry::getWidget() const {
		return entry_widget;
	}

	fw::RectangleWidget* TreeViewEntry::getRectangleWidget() const {
		return entry_widget->rectangle_widget;
	}

	fw::TextWidget* TreeViewEntry::getTextWidget() const {
		return entry_widget->text_widget;
	}

	fw::RectangleWidget* TreeViewEntry::getArrowAreaWidget() const {
		return entry_widget->arrow_area_widget;
	}

	fw::PolygonWidget* TreeViewEntry::getArrowWidget() const {
		return entry_widget->arrow_widget;
	}

	fw::ContainerWidget* TreeViewEntry::getChildrenBoxWidget() const {
		return entry_widget->children_box_widget;
	}

	fw::EmptyWidget* TreeViewEntry::getChildrenSpacingWidget() const {
		return entry_widget->children_spacing_widget;
	}

	fw::ContainerWidget* TreeViewEntry::getChildrenWidget() const {
		return entry_widget->children_widget;
	}

	void TreeViewEntry::processMouseMove(const sf::Vector2f& pos) {
		if (pressed && !grabbed) {
			sf::Vector2f rel_pos = entry_widget->getRelativeMousePos() - grab_offset;
			if (length(rel_pos) >= TREEVIEW_ENTRY_DRAG_DISTANCE) {
				take();
			}
		}
	}

	void TreeViewEntry::processMouse(const sf::Vector2f& pos) {
		if (grabbed) {
			sf::Vector2f new_pos = pos - grab_offset;
			treeview.grabbed_widget->setGlobalPosition(new_pos);
			if (!grab_begin) {
				treeview.putTargetHighlight();
			}
		}
		grab_begin = false;
	}

	void TreeViewEntry::processLeftPress(const sf::Vector2f& pos) {
		pressed = true;
		grab_offset = entry_widget->getRelativeMousePos();
	}

	void TreeViewEntry::processLeftClick(const sf::Vector2f& pos) {
		if (!grabbed) {
			click();
		}
	}

	void TreeViewEntry::processLeftRelease(const sf::Vector2f& pos) {
		if (grabbed) {
			if (treeview.highlighted_entry) {
				TreeViewEntry* highlighted_entry_parent = treeview.highlighted_entry->getParent();
				size_t highlighted_entry_index = treeview.highlighted_entry->getIndex();
				dropTo(highlighted_entry_parent, highlighted_entry_index);
			} else {
				dropTo(nullptr, treeview.getTopEntryCount());
			}
		}
	}

	void TreeViewEntry::processGlobalLeftRelease(const sf::Vector2f& pos) {
		pressed = false;
	}

	void TreeViewEntry::selectSilent(bool with_children) {
		selected = true;
		entry_widget->rectangle_widget->setFillColor(TREEVIEW_ENTRY_SELECTION_COLOR);
		if (with_children) {
			for (size_t i = 0; i < children.size(); i++) {
				TreeViewEntry* child = children[i];
				child->select(true);
			}
		}
	}

	void TreeViewEntry::deselectSilent(bool with_children) {
		selected = false;
		entry_widget->rectangle_widget->setFillColor(TREEVIEW_ENTRY_BACKGROUND_COLOR);
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

	void TreeViewEntry::setParent(TreeViewEntry* new_parent, bool reparent_widget) {
		if (new_parent) {
			wAssert(treeview.all_entries.contains(new_parent));
		}
		if (new_parent == parent) {
			return;
		}
		TreeViewEntry* old_parent = parent;
		if (old_parent) {
			old_parent->removeChild(this);
		}
		if (new_parent) {
			new_parent->addChild(this);
			treeview.top_entries.remove(this);
		} else {
			treeview.top_entries.add(this);
		}
		if (reparent_widget) {
			setWidgetParent(new_parent);
		}
		this->parent = new_parent;
	}

	void TreeViewEntry::setWidgetParent(TreeViewEntry* new_parent) {
		if (new_parent) {
			fw::ContainerWidget* parent_children_widget = new_parent->getChildrenWidget();
			entry_widget->setParent(parent_children_widget);
		} else {
			entry_widget->setParent(&treeview);
		}
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
		entry_widget->updateWidgets();
	}

	void TreeViewEntry::collapse() {
		expanded = false;
		entry_widget->updateWidgets();
	}

	void TreeViewEntry::toggle() {
		if (expanded) {
			collapse();
		} else {
			expand();
		}
	}

	void TreeViewEntry::take() {
		treeview.widget_list.addPostAction([this](WidgetList& widget_list) {
			grabbed = true;
			grab_begin = true;
			treeview.grabbed_entry = this;
			TreeViewEntryWidget* widget_copy = entry_widget->clone();
			widget_copy->setParentAnchor(Widget::Anchor::CUSTOM);
			widget_copy->setSizeXPolicy(Widget::SizePolicy::NONE);
			widget_copy->rectangle_widget->setClickThrough(true); // to allow mouse wheel events
			widget_copy->setParent(nullptr);
			widget_copy->setGrabbedVisualMode();
			widget_copy->setGlobalPosition(entry_widget->getGlobalPosition());
			treeview.grabbed_widget = widget_copy;
		}, PostActionStage::DUPLICATE);
		treeview.widget_list.addPostAction([this](WidgetList& widget_list) {
			treeview.grabbed_widget->children_box_widget->remove();
		}, PostActionStage::REMOVE);
	}

	void TreeViewEntry::dropTo(TreeViewEntry* parent, size_t index) {
		if (this != parent && !parent->getParentChain().contains(this)) {
			treeview.widget_list.addPostAction([this, parent, index](WidgetList& widget_list) {
				setParent(parent);
				if (index >= 0) {
					moveToIndex(index);
				}
			}, PostActionStage::SET_PARENT);
		}
		releaseGrab();
	}

	void TreeViewEntry::releaseGrab() {
		if (!grabbed) {
			return;
		}
		grabbed = false;
		grab_begin = false;
		pressed = false;
		treeview.grabbed_entry = nullptr;
		treeview.widget_list.addPostAction([this](WidgetList& widget_list) {
			treeview.grabbed_widget->remove();
			treeview.grabbed_widget = nullptr;
		}, PostActionStage::REMOVE);
		TreeViewWidget::target_highlight.visible = false;
	}

	void TreeViewEntry::remove(bool with_children) {
		treeview.removeEntry(this, with_children);
	}

	void TreeViewEntry::addChild(TreeViewEntry* entry) {
		wAssert(treeview.all_entries.contains(entry));
		children.add(entry);
		entry_widget->updateWidgets();
	}

	void TreeViewEntry::moveChildToIndex(TreeViewEntry* entry, size_t index) {
		wAssert(children.contains(entry));
		children.moveValueToIndex(entry, index);
	}

	void TreeViewEntry::removeChild(TreeViewEntry* entry) {
		wAssert(children.contains(entry));
		children.remove(entry);
		entry_widget->updateWidgets();
	}

}
