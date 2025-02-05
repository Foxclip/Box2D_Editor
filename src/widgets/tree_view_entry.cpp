#include "widgets/tree_view_entry.h"
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
				treeview.grabbed_widget->setGlobalPosition(new_pos);
				if (!grab_begin) {
					treeview.putTargetHighlight();
				}
			}
			grab_begin = false;
		};
		entry_widget->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
			if (grabbed) {
				if (treeview.highlighted_entry) {
					TreeViewEntry* highlighted_entry_parent = treeview.highlighted_entry->getParent();
					size_t highlighted_entry_index = treeview.highlighted_entry->getIndex();
					dropTo(highlighted_entry_parent, highlighted_entry_index);
				} else {
					dropTo(nullptr, treeview.getTopEntryCount());
				}
			}
		};
		// rectangle
		rectangle_widget = treeview.widget_list.createRectangleWidget(20.0f, TREEVIEW_ENTRY_HEIGHT);
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_BACKGROUND_COLOR);
		rectangle_widget->setClipChildren(true);
		rectangle_widget->setClickThrough(false);
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

	void TreeViewEntry::setParent(TreeViewEntry* new_parent, bool reparent_widget) {
		if (new_parent) {
			wAssert(treeview.all_entries.contains(new_parent));
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
		treeview.widget_list.addPostAction([this](WidgetList& widget_list) {
			Widget* copy = entry_widget->clone();
			copy->setParentAnchor(Widget::Anchor::CUSTOM);
			copy->setSizeXPolicy(Widget::SizePolicy::NONE);
			copy->find("rectangle")->setClickThrough(true); // to allow mouse wheel events
			copy->setParent(nullptr);
			grabbed = true;
			grab_begin = true;
			treeview.grabbed_entry = this;
			treeview.grabbed_widget = copy;
		}, PostActionStage::DUPLICATE);
	}

	void TreeViewEntry::dropTo(TreeViewEntry* parent, size_t index) {
		treeview.widget_list.addPostAction([this, parent](WidgetList& widget_list) {
			this->setParent(parent); 
		}, PostActionStage::SET_PARENT);
		grabbed = false;
		grab_begin = false;
		treeview.grabbed_entry = nullptr;
		treeview.widget_list.addPostAction([this](WidgetList& widget_list) {
			treeview.grabbed_widget->remove();
		}, PostActionStage::REMOVE);
		TreeViewWidget::target_highlight.visible = false;
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
