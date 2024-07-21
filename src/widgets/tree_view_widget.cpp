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
		setHorizontal(false);
		setPadding(TREEVIEW_CONTAINER_PADDING);
		setSizeXPolicy(SizePolicy::NONE);
	}

	TreeViewWidget::TreeViewWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: TreeViewWidget(widget_list, size.x, size.y) { }

	TreeViewWidget::Entry* TreeViewWidget::getEntry(size_t index) const {
		return entries[index];
	}

	TreeViewWidget::Entry* TreeViewWidget::addEntry(const sf::String& name) {
		std::unique_ptr<Entry> entry_uptr = std::make_unique<Entry>(*this, name);
		Entry* ptr = entry_uptr.get();
		entries.add(std::move(entry_uptr));
		return ptr;
	}

	void TreeViewWidget::removeEntry(Entry* entry) {
		entries.remove(entry);
	}

	void TreeViewWidget::setParentToEntry(Entry* child_entry, Entry* parent_entry) {
		fw::ContainerWidget* child_entry_widget = child_entry->getWidget();
		if (parent) {
			parent_entry->getArrowAreaWidget()->setVisible(true);
			fw::ContainerWidget* parent_children_widget = parent_entry->getChildrenWidget();
			child_entry_widget->setParent(parent_children_widget);
		} else {
			child_entry_widget->setParent(container_widget);
			if (parent_entry->getChildrenWidget()->getChildrenCount() == 0) {
				parent_entry->getArrowAreaWidget()->setVisible(false);
				parent_entry->getChildrenBoxWidget()->setVisible(false);
			}
		}
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
		rectangle_widget->OnLeftPress += this->OnLeftPress;
		rectangle_widget->setParent(entry_widget);
		// arrow area
		arrow_area_widget = treeview.widget_list.createWidget<fw::RectangleWidget>(15.0f, 20.0f);
		arrow_area_widget->setName("arrow area");
		arrow_area_widget->setVisible(false);
		arrow_area_widget->setSizeYPolicy(fw::Widget::SizePolicy::PARENT);
		arrow_area_widget->setFillColor(TREEVIEW_ENTRY_ARROW_AREA_COLOR);
		arrow_area_widget->setClickThrough(false);
		arrow_area_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
			if (children_box_widget->isVisible()) {
				arrow_widget->setRotation(0.0f);
			} else {
				arrow_widget->setRotation(90.0f);
			}
			children_box_widget->toggleVisible();
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

	void TreeViewWidget::Entry::select() {
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_SELECTION_COLOR);
	}

	void TreeViewWidget::Entry::deselect() {
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_BACKGROUND_COLOR);
	}

}
