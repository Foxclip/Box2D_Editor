#include "widgets/scroll_area_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	ScrollAreaWidget::ScrollAreaWidget(WidgetList& widget_list, float width, float height) : EmptyWidget(widget_list) {
		// scrollarea
		type = Widget::WidgetType::ScrollArea;
		setName("scrollarea");
		setSizeInternal(width, height);
		// container
		float default_width = SCROLL_AREA_SCROLLBAR_DEFAULT_WIDTH;
		float container_width = width - default_width;
		float container_height = height - default_width;
		container_widget = widget_list.createWidget<ContainerWidget>(container_width, container_height);
		container_widget->setFillColor(SCROLL_AREA_DEFAULT_BACKGROUND_COLOR);
		container_widget->setSizePolicy(SizePolicy::NONE);
		container_widget->setClickThrough(false);
		container_widget->setParent(this);
		// slider x bg
		slider_background_x_widget = widget_list.createWidget<RectangleWidget>(container_width, default_width);
		slider_background_x_widget->setName("slider bg x");
		slider_background_x_widget->setFillColor(SCROLL_AREA_DEFAULT_SCROLL_BACKGROUND_COLOR);
		slider_background_x_widget->setParentAnchor(Anchor::BOTTOM_LEFT);
		slider_background_x_widget->setOrigin(Anchor::BOTTOM_LEFT);
		slider_background_x_widget->setClickThrough(false);
		slider_background_x_widget->setParent(this);
		// slider x
		slider_x_widget = widget_list.createWidget<RectangleWidget>(container_width / 2.0f, default_width);
		slider_x_widget->setName("slider x");
		slider_x_widget->setFillColor(SCROLL_AREA_DEFAULT_SLIDER_COLOR);
		slider_x_widget->setClickThrough(false);
		slider_x_widget->setParent(slider_background_x_widget);
		// slider y bg
		slider_background_y_widget = widget_list.createWidget<RectangleWidget>(default_width, container_height);
		slider_background_y_widget->setName("slider bg y");
		slider_background_y_widget->setFillColor(SCROLL_AREA_DEFAULT_SCROLL_BACKGROUND_COLOR);
		slider_background_y_widget->setParentAnchor(Anchor::TOP_RIGHT);
		slider_background_y_widget->setOrigin(Anchor::TOP_RIGHT);
		slider_background_y_widget->setClickThrough(false);
		slider_background_y_widget->setParent(this);
		// slider y
		slider_y_widget = widget_list.createWidget<RectangleWidget>(default_width, container_height / 2.0f);
		slider_y_widget->setName("slider y");
		slider_y_widget->setFillColor(SCROLL_AREA_DEFAULT_SLIDER_COLOR);
		slider_y_widget->setClickThrough(false);
		slider_y_widget->setParent(slider_background_y_widget);
		// corner
		corner_widget = widget_list.createWidget<RectangleWidget>(default_width, default_width);
		corner_widget->setName("corner");
		corner_widget->setFillColor(SCROLL_AREA_DEFAULT_CORNER_COLOR);
		corner_widget->setParentAnchor(Anchor::BOTTOM_RIGHT);
		corner_widget->setOrigin(Anchor::BOTTOM_RIGHT);
		corner_widget->setClickThrough(false);
		corner_widget->setParent(this);
	}

	ScrollAreaWidget::ScrollAreaWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: ScrollAreaWidget(widget_list, size.x, size.y) { }

	void ScrollAreaWidget::setSize(float width, float height) {
		float container_width = width - slider_background_y_widget->getWidth();
		float container_height = height - slider_background_x_widget->getHeight();
		container_widget->setSize(container_width, container_height);
		slider_background_x_widget->setWidth(container_width);
		slider_background_y_widget->setHeight(container_height);
	}

}
