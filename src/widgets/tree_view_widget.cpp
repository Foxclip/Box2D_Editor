#include "widgets/tree_view_widget.h"
#include "widgets/widget_list.h"
#include "widgets/polygon_widget.h"
#include "widgets/text_widget.h"

namespace fw {

	TreeViewWidget::TreeViewWidget(WidgetList& widget_list, float width, float height)
		: ContainerWidget(widget_list, width, height) {
		type = WidgetType::TreeView;
		setName("treeview");
		setFillColor(sf::Color::Transparent);
		setClipChildren(true);
		setClickThrough(false);
		setHorizontal(false);
		setPadding(TREEVIEW_CONTAINER_PADDING);
		setSizeXPolicy(SizePolicy::NONE);
	}

	TreeViewWidget::TreeViewWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: TreeViewWidget(widget_list, size.x, size.y) { }

	void TreeViewWidget::deselectAll() {
		for (size_t i = 0; i < entries.size(); i++) {
			entries[i]->deselect();
		}
	}

	const CompVector<TreeViewWidget::Entry*>& TreeViewWidget::getEntries() const {
		return entries.getCompVector();
	}

	size_t TreeViewWidget::getEntryCount() const {
		return entries.size();
	}

	TreeViewWidget::Entry* TreeViewWidget::getEntry(size_t index) const {
		return entries[index];
	}

	TreeViewWidget::Entry* TreeViewWidget::addEntry(const sf::String& name) {
		std::unique_ptr<Entry> entry_uptr = std::make_unique<Entry>(*this, name);
		Entry* ptr = entry_uptr.get();
		entries.add(std::move(entry_uptr));
		return ptr;
	}

	void TreeViewWidget::expandAll() {
		for (size_t i = 0; i < entries.size(); i++) {
			entries[i]->expand();
		}
	}

	void TreeViewWidget::collapseAll() {
		for (size_t i = 0; i < entries.size(); i++) {
			entries[i]->collapse();
		}
	}

	void TreeViewWidget::removeEntry(Entry* entry) {
		entries.remove(entry);
	}

	void TreeViewWidget::clear() {
		entries.clear();
	}

	TreeViewWidget::Entry::Entry(TreeViewWidget& treeview, const sf::String& name) : treeview(treeview) {
		// container
		entry_widget = treeview.widget_list.createWidget<fw::ContainerWidget>(20.0f, TREEVIEW_ENTRY_HEIGHT);
		entry_widget->setName(name + " entry");
		entry_widget->setHorizontal(false);
		entry_widget->setInnerPaddingY(TREEVIEW_CONTAINER_PADDING);
		entry_widget->setFillColor(sf::Color::Transparent);
		entry_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
		entry_widget->setParent(&treeview);
		// rectangle
		rectangle_widget = treeview.widget_list.createWidget<fw::RectangleWidget>(20.0f, TREEVIEW_ENTRY_HEIGHT);
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_BACKGROUND_COLOR);
		rectangle_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
		rectangle_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
			bool with_children = treeview.widget_list.isLCtrlPressed();
			if (treeview.widget_list.isLShiftPressed()) {
				toggleSelect(with_children);
			} else {
				treeview.deselectAll();
				select(with_children);
			}
		};
		rectangle_widget->setParent(entry_widget);
		// arrow area
		arrow_area_widget = treeview.widget_list.createWidget<fw::RectangleWidget>(15.0f, 20.0f);
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
		arrow_widget = treeview.widget_list.createWidget<fw::PolygonWidget>(vertices);
		arrow_widget->setName("arrow");
		arrow_widget->setFillColor(TREEVIEW_ENTRY_ARROW_COLOR);
		arrow_widget->setParentAnchor(fw::Widget::Anchor::CENTER);
		arrow_widget->setParent(arrow_area_widget);
		// text
		fw::TextWidget* text_widget = treeview.widget_list.createWidget<fw::TextWidget>();
		text_widget->setCharacterSize(TREEVIEW_ENTRY_FONT_SIZE);
		text_widget->setFillColor(TREEVIEW_ENTRY_TEXT_COLOR);
		text_widget->setString(name);
		text_widget->setOrigin(Anchor::CENTER_LEFT);
		text_widget->setParentAnchor(Anchor::CENTER_LEFT);
		text_widget->setAnchorOffsetX(18.0f);
		text_widget->setParent(rectangle_widget);
		// children box
		children_box_widget = treeview.widget_list.createWidget<fw::ContainerWidget>(20.0f, TREEVIEW_ENTRY_HEIGHT);
		children_box_widget->setName("children box");
		children_box_widget->setVisible(false);
		children_box_widget->setRenderable(false);
		children_box_widget->setSizeXPolicy(fw::Widget::SizePolicy::PARENT);
		children_box_widget->setParent(entry_widget);
		// spacing
		children_spacing_widget = treeview.widget_list.createWidget<fw::EmptyWidget>();
		children_spacing_widget->setName("spacing");
		children_spacing_widget->setSize(TREEVIEW_ENTRY_CHILDREN_OFFSET, 1.0f);
		children_spacing_widget->setParent(children_box_widget);
		// children
		children_widget = treeview.widget_list.createWidget<fw::ContainerWidget>(20.0f, TREEVIEW_ENTRY_HEIGHT);
		children_widget->setName("children");
		children_widget->setHorizontal(false);
		children_widget->setInnerPaddingY(TREEVIEW_CONTAINER_PADDING);
		children_widget->setRenderable(false);
		children_widget->setSizeXPolicy(fw::Widget::SizePolicy::EXPAND);
		children_widget->setParent(children_box_widget);
	}

	TreeViewWidget::Entry::~Entry() {
		entry_widget->remove();
	}

	bool TreeViewWidget::Entry::isExpanded() const {
		return expanded;
	}

	TreeViewWidget::Entry* TreeViewWidget::Entry::getParent() const {
		return parent;
	}

	CompVector<TreeViewWidget::Entry*> TreeViewWidget::Entry::getChildren() const {
		return children;
	}

	TreeViewWidget::Entry* TreeViewWidget::Entry::getChild(size_t index) const {
		return children[index];
	}

	size_t TreeViewWidget::Entry::getChildrenCount() const {
		return children.size();
	}

	fw::ContainerWidget* TreeViewWidget::Entry::getWidget() const {
		return entry_widget;
	}

	fw::RectangleWidget* TreeViewWidget::Entry::getRectangleWidget() const {
		return rectangle_widget;
	}

	fw::RectangleWidget* TreeViewWidget::Entry::getArrowAreaWidget() const {
		return arrow_area_widget;
	}

	fw::PolygonWidget* TreeViewWidget::Entry::getArrowWidget() const {
		return arrow_widget;
	}

	fw::ContainerWidget* TreeViewWidget::Entry::getChildrenBoxWidget() const {
		return children_box_widget;
	}

	fw::EmptyWidget* TreeViewWidget::Entry::getChildrenSpacingWidget() const {
		return children_spacing_widget;
	}

	fw::ContainerWidget* TreeViewWidget::Entry::getChildrenWidget() const {
		return children_widget;
	}

	void TreeViewWidget::Entry::selectSilent(bool with_children) {
		selected = true;
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_SELECTION_COLOR);
		if (with_children) {
			for (size_t i = 0; i < children.size(); i++) {
				Entry* child = children[i];
				child->select(true);
			}
		}
	}

	void TreeViewWidget::Entry::deselectSilent(bool with_children) {
		selected = false;
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_BACKGROUND_COLOR);
		if (with_children) {
			for (size_t i = 0; i < children.size(); i++) {
				Entry* child = children[i];
				child->deselect(true);
			}
		}
	}

	void TreeViewWidget::Entry::toggleSelectSilent(bool with_children) {
		if (selected) {
			deselectSilent(with_children);
		} else {
			selectSilent(with_children);
		}
	}

	void TreeViewWidget::Entry::select(bool with_children) {
		selectSilent(with_children);
		treeview.OnEntrySelected(this);
	}

	void TreeViewWidget::Entry::deselect(bool with_children) {
		deselectSilent(with_children);
		treeview.OnEntryDeselected(this);
	}

	void TreeViewWidget::Entry::toggleSelect(bool with_children) {
		if (selected) {
			deselect(with_children);
		} else {
			select(with_children);
		}
	}

	void TreeViewWidget::Entry::setParent(Entry* new_parent) {
		Entry* old_parent = parent;
		if (old_parent) {
			old_parent->children.remove(this);
			if (old_parent->getChildrenWidget()->getChildrenCount() == 0) {
				old_parent->getArrowAreaWidget()->setVisible(false);
				old_parent->getChildrenBoxWidget()->setVisible(false);
			}
		}
		if (new_parent) {
			new_parent->children.add(this);
			new_parent->getArrowAreaWidget()->setVisible(true);
			fw::ContainerWidget* parent_children_widget = new_parent->getChildrenWidget();
			entry_widget->setParent(parent_children_widget);
		} else {
			entry_widget->setParent(treeview.container_widget);
		}
	}

	void TreeViewWidget::Entry::expand() {
		expanded = true;
		arrow_widget->setRotation(90.0f);
		children_box_widget->setVisible(true);
	}

	void TreeViewWidget::Entry::collapse() {
		expanded = false;
		arrow_widget->setRotation(0.0f);
		children_box_widget->setVisible(false);
	}

	void TreeViewWidget::Entry::toggle() {
		if (expanded) {
			collapse();
		} else {
			expand();
		}
	}

}
