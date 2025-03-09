#include "widgets/tree_view_entry_widget.h"
#include "widgets/tree_view_widget.h"
#include "widgets/polygon_widget.h"
#include "widgets/text_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	TreeViewEntryWidget::TreeViewEntryWidget(WidgetList& widget_list, TreeViewEntry& entry, const std::string& name, float width, float height) : ContainerWidget(widget_list, width, height), entry(entry) {
		setName(name + " entry");
		setHorizontal(false);
		setInnerPaddingY(TREEVIEW_MAIN_PANEL_PADDING);
		setFillColor(sf::Color::Transparent);
		setSizeXPolicy(Widget::SizePolicy::PARENT);
		setParent(&entry.treeview);
		OnMouseMoved += [&](const sf::Vector2f& pos) {
			entry.processMouseMove(pos);
		};
		OnProcessMouse += [&](const sf::Vector2f& pos) {
			entry.processMouse(pos);
		};
		OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
			entry.processLeftRelease(pos);
		};
		// rectangle
		rectangle_widget = entry.treeview.widget_list.createRectangleWidget(20.0f, TREEVIEW_ENTRY_HEIGHT);
		rectangle_widget->setFillColor(TREEVIEW_ENTRY_BACKGROUND_COLOR);
		rectangle_widget->setClipChildren(true);
		rectangle_widget->setClickThrough(false);
		rectangle_widget->setSizeXPolicy(Widget::SizePolicy::PARENT);
		rectangle_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
			entry.processLeftPress(pos);
		};
		rectangle_widget->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
			entry.processGlobalLeftRelease(pos);
		};
		rectangle_widget->OnLeftClick += [&](const sf::Vector2f& pos) {
			entry.processLeftClick(pos);
		};
		rectangle_widget->setParent(this);
		// arrow area
		arrow_area_widget = entry.treeview.widget_list.createRectangleWidget(15.0f, 20.0f);
		arrow_area_widget->setName("arrow area");
		arrow_area_widget->setVisible(false);
		arrow_area_widget->setSizeYPolicy(Widget::SizePolicy::PARENT);
		arrow_area_widget->setFillColor(TREEVIEW_ENTRY_ARROW_AREA_COLOR);
		arrow_area_widget->setClickThrough(false);
		arrow_area_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
			entry.toggle();
		};
		arrow_area_widget->setParent(rectangle_widget);
		// arrow
		std::vector<sf::Vector2f> vertices = {
			sf::Vector2f(5.0, 0.0f),
			sf::Vector2f(-3.0, 5.0f),
			sf::Vector2f(-3.0, -5.0f),
		};
		arrow_widget = entry.treeview.widget_list.createPolygonWidget(vertices);
		arrow_widget->setName("arrow");
		arrow_widget->setFillColor(TREEVIEW_ENTRY_ARROW_COLOR);
		arrow_widget->setParentAnchor(Widget::Anchor::CENTER);
		arrow_widget->setParent(arrow_area_widget);
		// text
		text_widget = entry.treeview.widget_list.createTextWidget();
		text_widget->setCharacterSize(TREEVIEW_ENTRY_FONT_SIZE);
		text_widget->setFillColor(TREEVIEW_ENTRY_TEXT_COLOR);
		text_widget->setString(name);
		text_widget->setOrigin(Widget::Anchor::CENTER_LEFT);
		text_widget->setParentAnchor(Widget::Anchor::CENTER_LEFT);
		text_widget->setAnchorOffsetX(18.0f);
		text_widget->setParent(rectangle_widget);
		// children box
		children_box_widget = entry.treeview.widget_list.createContainerWidget(20.0f, TREEVIEW_ENTRY_HEIGHT);
		children_box_widget->setName("children box");
		children_box_widget->setVisible(false);
		children_box_widget->setRenderable(false);
		children_box_widget->setSizeXPolicy(Widget::SizePolicy::PARENT);
		children_box_widget->setParent(this);
		// spacing
		children_spacing_widget = entry.treeview.widget_list.createEmptyWidget();
		children_spacing_widget->setName("spacing");
		children_spacing_widget->setSize(TREEVIEW_ENTRY_CHILDREN_OFFSET, 1.0f);
		children_spacing_widget->setParent(children_box_widget);
		// children
		children_widget = entry.treeview.widget_list.createContainerWidget(20.0f, TREEVIEW_ENTRY_HEIGHT);
		children_widget->setName("children");
		children_widget->setHorizontal(false);
		children_widget->setInnerPaddingY(TREEVIEW_MAIN_PANEL_PADDING);
		children_widget->setRenderable(false);
		children_widget->setSizeXPolicy(Widget::SizePolicy::EXPAND);
		children_widget->setParent(children_box_widget);
	}

	void TreeViewEntryWidget::updateWidgets() {
		if (entry.getChildrenCount() == 0) {
			arrow_area_widget->setVisible(false);
			children_box_widget->setVisible(false);
		} else {
			arrow_area_widget->setVisible(true);
			if (entry.expanded) {
				arrow_widget->setRotation(90.0f);
				children_box_widget->setVisible(true);
			} else {
				arrow_widget->setRotation(0.0f);
				children_box_widget->setVisible(false);
			}
		}
	}

	void TreeViewEntryWidget::setGrabbedVisualMode() {
		rectangle_widget->setAlphaMultiplier(0.5f);
		text_widget->setAlphaMultiplier(0.5f);
		arrow_area_widget->setAlphaMultiplier(0.5f);
		arrow_widget->setAlphaMultiplier(0.5f);
	}

	TreeViewEntryWidget* TreeViewEntryWidget::clone(bool with_children) {
		TreeViewEntryWidget* copy = widget_list.duplicateWidget(this, with_children);
		RectangleWidget* rectangle_widget_copy = dynamic_cast<RectangleWidget*>(copy->find("rectangle"));
		TextWidget* text_widget_copy = dynamic_cast<TextWidget*>(copy->find("text"));
		RectangleWidget* arrow_area_widget_copy = dynamic_cast<RectangleWidget*>(copy->find("arrow area"));
		PolygonWidget* arrow_widget_copy = dynamic_cast<PolygonWidget*>(copy->find("arrow"));
		ContainerWidget* children_box_widget_copy = dynamic_cast<ContainerWidget*>(copy->find("children box"));
		EmptyWidget* children_spacing_widget = dynamic_cast<EmptyWidget*>(copy->find("spacing"));
		ContainerWidget* children_widget = dynamic_cast<ContainerWidget*>(copy->find("children"));
		copy->rectangle_widget = rectangle_widget_copy;
		copy->text_widget = text_widget_copy;
		copy->arrow_area_widget = arrow_area_widget_copy;
		copy->arrow_widget = arrow_widget_copy;
		copy->children_box_widget = children_box_widget_copy;
		copy->children_spacing_widget = children_spacing_widget;
		copy->children_widget = children_widget;
		return copy;
	}

}