#include "widgets/scroll_area_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	ScrollAreaWidget::ScrollAreaWidget(WidgetList& widget_list, float width, float height) : EmptyWidget(widget_list) {
		// scrollarea
		type = Widget::WidgetType::ScrollArea;
		setName("scroll area");
		setSizeInternal(width, height);
		// container
		float default_width = SCROLL_AREA_SCROLLBAR_DEFAULT_WIDTH;
		float area_width = width - default_width;
		float area_height = height - default_width;
		area_widget = widget_list.createWidget<RectangleWidget>(area_width, area_height);
		area_widget->setFillColor(SCROLL_AREA_DEFAULT_BACKGROUND_COLOR);
		area_widget->setParentAnchor(Anchor::TOP_LEFT);
		area_widget->setClipChildren(true);
		area_widget->setClickThrough(false);
		area_widget->setParent(this);
		// slider x bg
		slider_background_x_widget = widget_list.createWidget<RectangleWidget>(area_width, default_width);
		slider_background_x_widget->setName("slider bg x");
		slider_background_x_widget->setFillColor(SCROLL_AREA_DEFAULT_SCROLL_BACKGROUND_COLOR);
		slider_background_x_widget->setParentAnchor(Anchor::BOTTOM_LEFT);
		slider_background_x_widget->setOrigin(Anchor::BOTTOM_LEFT);
		slider_background_x_widget->setClickThrough(false);
		slider_background_x_widget->setParent(this);
		// slider x
		slider_x_widget = widget_list.createWidget<RectangleWidget>(area_width / 2.0f, default_width);
		slider_x_widget->setName("slider x");
		slider_x_widget->setFillColor(SCROLL_AREA_DEFAULT_SLIDER_COLOR);
		slider_x_widget->setClickThrough(false);
		slider_x_widget->setParent(slider_background_x_widget);
		// slider y bg
		slider_background_y_widget = widget_list.createWidget<RectangleWidget>(default_width, area_height);
		slider_background_y_widget->setName("slider bg y");
		slider_background_y_widget->setFillColor(SCROLL_AREA_DEFAULT_SCROLL_BACKGROUND_COLOR);
		slider_background_y_widget->setParentAnchor(Anchor::TOP_RIGHT);
		slider_background_y_widget->setOrigin(Anchor::TOP_RIGHT);
		slider_background_y_widget->setClickThrough(false);
		slider_background_y_widget->setParent(this);
		// slider y
		slider_y_widget = widget_list.createWidget<RectangleWidget>(default_width, area_height / 2.0f);
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

		updateScroll();
	}

	ScrollAreaWidget::ScrollAreaWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: ScrollAreaWidget(widget_list, size.x, size.y) { }

	Widget* ScrollAreaWidget::getWidget() const {
		return scrolled_widget;
	}

	void ScrollAreaWidget::setWidget(Widget* widget) {
		this->scrolled_widget = widget;
		widget->setParent(area_widget);
	}

	void ScrollAreaWidget::setSize(float width, float height) {
		float area_width = width - slider_background_y_widget->getWidth();
		float area_height = height - slider_background_x_widget->getHeight();
		area_widget->setSize(area_width, area_height);
		slider_background_x_widget->setWidth(area_width);
		slider_background_y_widget->setHeight(area_height);
	}

	void ScrollAreaWidget::internalUpdate() {
		float area_x = std::min(area_widget->getAnchorOffset().x, scrolled_widget->getWidth());
		float area_y = std::min(area_widget->getAnchorOffset().y, scrolled_widget->getHeight());
		area_widget->setPosition(area_x, area_y);
		updateScroll();
	}

	void ScrollAreaWidget::updateScroll() {
		// size
		float x_factor = 1.0f;
		float y_factor = 1.0f;
		float x_width = slider_background_x_widget->getWidth();
		float y_height = slider_background_y_widget->getHeight();
		if (scrolled_widget) {
			x_factor = area_widget->getWidth() / scrolled_widget->getWidth();
			y_factor = area_widget->getHeight() / scrolled_widget->getHeight();
			x_factor = std::min(x_factor, 1.0f);
			y_factor = std::min(y_factor, 1.0f);
			x_width = slider_background_x_widget->getWidth() * x_factor;
			y_height = slider_background_y_widget->getHeight() * y_factor;
		}
		slider_x_widget->setWidth(x_width);
		slider_y_widget->setHeight(y_height);
		// position
		float pos_x_factor = 0.0f;
		float pos_y_factor = 0.0f;
		float pos_x = 0.0f;
		float pos_y = 0.0f;
		if (scrolled_widget) {
			pos_x_factor = -area_widget->getAnchorOffset().x / area_widget->getWidth();
			pos_y_factor = -area_widget->getAnchorOffset().y / area_widget->getHeight();
			float x_range = slider_background_x_widget->getWidth() - slider_x_widget->getWidth();
			float y_range = slider_background_y_widget->getHeight() - slider_y_widget->getHeight();
			pos_x = x_range * pos_x_factor;
			pos_y = y_range * pos_y_factor;
		}
		slider_x_widget->setPositionX(pos_x);
		slider_y_widget->setPositionY(pos_y);
	}

}
