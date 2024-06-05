#include "widgets/scroll_area_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	enum class ScrollAreaRenderLayers {
		BASE,
		SCROLL_AREA,
	};

	ScrollAreaWidget::ScrollAreaWidget(WidgetList& widget_list, float width, float height) : EmptyWidget(widget_list) {
		// scrollarea
		type = Widget::WidgetType::ScrollArea;
		setName("scroll area");
		setSizeInternal(width, height);
		setLocalRenderLayer(static_cast<size_t>(ScrollAreaRenderLayers::SCROLL_AREA));
		// container
		float default_width = SCROLL_AREA_SCROLLBAR_DEFAULT_WIDTH;
		float area_width = width - default_width;
		float area_height = height - default_width;
		area_widget = widget_list.createWidget<RectangleWidget>(area_width, area_height);
		area_widget->setName("area");
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
		slider_x_widget->setQuantizeRenderedPosition(false); // slider might be in non-integer positions
		slider_x_widget->setParent(slider_background_x_widget);
		slider_x_widget->OnMouseEnter = [&](const sf::Vector2f& pos) {
			slider_x_hover = true;
			updateColors();
		};
		slider_x_widget->OnLeftPress = [&](const sf::Vector2f& pos) {
			slider_x_grabbed = true;
			mouse_grab_pos = pos.x;
			slider_grab_pos = slider_x_widget->getPosition().x;
			updateColors();
		};
		slider_x_widget->OnMouseMoved = [&](const sf::Vector2f& pos) {
			if (slider_x_grabbed) {
				float x_offset = pos.x - mouse_grab_pos;
				setSliderX(slider_grab_pos + x_offset);
			}
		};
		slider_x_widget->OnLeftRelease = [&](const sf::Vector2f& pos) {
			slider_x_grabbed = false;
			updateColors();
		};
		slider_x_widget->OnMouseExit = [&](const sf::Vector2f& pos) {
			slider_x_hover = false;
			updateColors();
		};
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
		slider_y_widget->setQuantizeRenderedPosition(false);
		slider_y_widget->setParent(slider_background_y_widget);
		slider_y_widget->OnMouseEnter = [&](const sf::Vector2f& pos) {
			slider_y_hover = true;
			updateColors();
		};
		slider_y_widget->OnLeftPress = [&](const sf::Vector2f& pos) {
			slider_y_grabbed = true;
			mouse_grab_pos = pos.y;
			slider_grab_pos = slider_y_widget->getPosition().y;
			updateColors();
		};
		slider_y_widget->OnMouseMoved = [&](const sf::Vector2f& pos) {
			if (slider_y_grabbed) {
				float y_offset = pos.y - mouse_grab_pos;
				setSliderY(slider_grab_pos + y_offset);
			}
		};
		slider_y_widget->OnLeftRelease = [&](const sf::Vector2f& pos) {
			slider_y_grabbed = false;
			updateColors();
		};
		slider_y_widget->OnMouseExit = [&](const sf::Vector2f& pos) {
			slider_y_hover = false;
			updateColors();
		};
		// corner
		corner_widget = widget_list.createWidget<RectangleWidget>(default_width, default_width);
		corner_widget->setName("corner");
		corner_widget->setFillColor(SCROLL_AREA_DEFAULT_CORNER_COLOR);
		corner_widget->setParentAnchor(Anchor::BOTTOM_RIGHT);
		corner_widget->setOrigin(Anchor::BOTTOM_RIGHT);
		corner_widget->setClickThrough(false);
		corner_widget->setParent(this);
		// links
		WidgetLink* area_link_x = area_widget->addLink(
			"SIZE_X",
			{ this->getSizeXTarget(), slider_background_y_widget->getSizeXTarget() },
			[&](const std::vector<fw::WidgetUpdateTarget*>& targets) {
				area_widget->setWidth(getWidth() - slider_background_y_widget->getWidth());
			}
		);
		WidgetLink* area_link_y = area_widget->addLink(
			"SIZE_Y",
			{ this->getSizeYTarget(), slider_background_x_widget->getSizeYTarget() },
			[&](const std::vector<fw::WidgetUpdateTarget*>& targets) {
				area_widget->setHeight(getHeight() - slider_background_x_widget->getHeight());
			}
		);
		WidgetLink* slider_bg_link_x = slider_background_x_widget->addLink(
			"SIZE_X",
			{ this->getSizeXTarget(), slider_background_y_widget->getSizeXTarget() },
			[&](const std::vector<fw::WidgetUpdateTarget*>& targets) {
				slider_background_x_widget->setWidth(getWidth() - slider_background_y_widget->getWidth());
			}
		);
		WidgetLink* slider_bg_link_y = slider_background_y_widget->addLink(
			"SIZE_Y",
			{ this->getSizeYTarget(), slider_background_x_widget->getSizeYTarget() },
			[&](const std::vector<fw::WidgetUpdateTarget*>& targets) {
				slider_background_y_widget->setHeight(getHeight() - slider_background_x_widget->getHeight());
			}
		);
		area_widget->addLink(
			"SCROLL",
			{ area_link_x, area_link_y, slider_bg_link_x, slider_bg_link_y },
			[&](const std::vector<fw::WidgetUpdateTarget*>& targets) {
				updateScroll();
			}
		);

		updateScroll();
	}

	ScrollAreaWidget::ScrollAreaWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: ScrollAreaWidget(widget_list, size.x, size.y) { }

	Widget* ScrollAreaWidget::getWidget() const {
		return scrolled_widget;
	}

	void ScrollAreaWidget::setWidget(Widget* widget) {
		this->scrolled_widget = widget;
		widget->setParentAnchor(Anchor::TOP_LEFT);
		widget->setAnchorOffset(0.0f, 0.0f);
		widget->setParent(area_widget);
	}

	void ScrollAreaWidget::setDeltaX(float delta) {
		this->delta_x = delta;
	}

	void ScrollAreaWidget::setDeltaY(float delta) {
		this->delta_y = delta;
	}

	void ScrollAreaWidget::internalOnScrollX(const sf::Vector2f& pos, float delta) {
		scrollX(delta_x * -delta);
	}

	void ScrollAreaWidget::internalOnScrollY(const sf::Vector2f& pos, float delta) {
		scrollY(delta_y * -delta);
	}

	void ScrollAreaWidget::updateScroll() {
		// size
		float x_width = slider_background_x_widget->getWidth() * getSizeXFactor();
		float y_height = slider_background_y_widget->getHeight() * getSizeYFactor();
		slider_x_widget->setWidth(x_width);
		slider_y_widget->setHeight(y_height);
		// position
		float pos_x = getSliderXFromArea();
		float pos_y = getSliderYFromArea();
		slider_x_widget->setPositionX(pos_x);
		slider_y_widget->setPositionY(pos_y);
	}

	void ScrollAreaWidget::updateColors() {
		sf::Color slider_x_color = SCROLL_AREA_DEFAULT_SLIDER_COLOR;
		sf::Color slider_y_color = SCROLL_AREA_DEFAULT_SLIDER_COLOR;
		if (slider_x_grabbed) {
			slider_x_color = SCROLL_AREA_DEFAULT_SLIDER_PRESSED_COLOR;
		} else if (slider_x_hover) {
			slider_x_color = SCROLL_AREA_DEFAULT_SLIDER_HOVER_COLOR;
		}
		if (slider_y_grabbed) {
			slider_y_color = SCROLL_AREA_DEFAULT_SLIDER_PRESSED_COLOR;
		} else if (slider_y_hover) {
			slider_y_color = SCROLL_AREA_DEFAULT_SLIDER_HOVER_COLOR;
		}
		slider_x_widget->setFillColor(slider_x_color);
		slider_y_widget->setFillColor(slider_y_color);
	}

	float ScrollAreaWidget::getSizeXFactor() const {
		float x_factor = 1.0f;
		if (scrolled_widget) {
			x_factor = area_widget->getWidth() / scrolled_widget->getWidth();
			x_factor = std::min(x_factor, 1.0f);
		}
		return x_factor;
	}

	float ScrollAreaWidget::getSizeYFactor() const {
		float y_factor = 1.0f;
		if (scrolled_widget) {
			y_factor = area_widget->getHeight() / scrolled_widget->getHeight();
			y_factor = std::min(y_factor, 1.0f);
		}
		return y_factor;
	}

	float ScrollAreaWidget::getXFactorFromArea() const {
		float pos_x_factor = 0.0f;
		if (scrolled_widget) {
			float scroll_range = getScrollXRange();
			if (scroll_range > 0.0f) {
				pos_x_factor = -scrolled_widget->getAnchorOffset().x / scroll_range;
			} else {
				pos_x_factor = 0.0f;
			}
		}
		return pos_x_factor;
	}

	float ScrollAreaWidget::getYFactorFromArea() const {
		float pos_y_factor = 0.0f;
		if (scrolled_widget) {
			float scroll_range = getScrollYRange();
			if (scroll_range > 0.0f) {
				pos_y_factor = -scrolled_widget->getAnchorOffset().y / scroll_range;
			} else {
				pos_y_factor = 0.0f;
			}
		}
		return pos_y_factor;
	}

	float ScrollAreaWidget::getXRange() const {
		float x_range = slider_background_x_widget->getWidth() - slider_x_widget->getWidth();
		return x_range;
	}

	float ScrollAreaWidget::getYRange() const {
		float y_range = slider_background_y_widget->getHeight() - slider_y_widget->getHeight();
		return y_range;
	}

	float ScrollAreaWidget::getScrollXRange() const {
		float scroll_range = scrolled_widget->getWidth() - area_widget->getWidth();
		return scroll_range;
	}

	float ScrollAreaWidget::getScrollYRange() const {
		float scroll_range = scrolled_widget->getHeight() - area_widget->getHeight();
		return scroll_range;
	}

	float ScrollAreaWidget::getSliderXFromArea() const {
		float pos_x = getXRange() * getXFactorFromArea();
		return pos_x;
	}

	float ScrollAreaWidget::getSliderYFromArea() const {
		float pos_y = getYRange() * getYFactorFromArea();
		return pos_y;
	}

	void ScrollAreaWidget::setSliderX(float x) {
		float x_range = getXRange();
		float pos_x = std::clamp(x, 0.0f, x_range);
		slider_x_widget->setPositionX(pos_x);
		if (scrolled_widget) {
			if (x_range > 0.0f) {
				float x_factor = pos_x / x_range;
				float scroll_range = scrolled_widget->getWidth() - area_widget->getWidth();
				float offset = -x_factor * scroll_range;
				scrolled_widget->setAnchorOffsetX(offset);
			} else {
				scrolled_widget->setAnchorOffsetX(0.0f);
			}
		}
	}

	void ScrollAreaWidget::setSliderY(float y) {
		float y_range = getYRange();
		float pos_y = std::clamp(y, 0.0f, y_range);
		slider_y_widget->setPositionY(pos_y);
		if (scrolled_widget) {
			if (y_range > 0.0f) {
				float y_factor = pos_y / y_range;
				float scroll_range = scrolled_widget->getHeight() - area_widget->getHeight();
				float offset = -y_factor * scroll_range;
				scrolled_widget->setAnchorOffsetY(offset);
			} else {
				scrolled_widget->setAnchorOffsetY(0.0f);
			}
		}
	}

	void ScrollAreaWidget::moveSliderX(float x_offset) {
		setSliderX(slider_x_widget->getPosition().x + x_offset);
	}

	void ScrollAreaWidget::moveSliderY(float y_offset) {
		setSliderY(slider_y_widget->getPosition().y + y_offset);
	}

	void ScrollAreaWidget::scrollX(float x_offset) {
		if (!scrolled_widget) {
			return;
		}
		float pos_x = scrolled_widget->getAnchorOffset().x - x_offset;
		float scroll_range = getScrollXRange();
		pos_x = std::clamp(pos_x, -scroll_range, 0.0f);
		scrolled_widget->setAnchorOffsetX(pos_x);
		updateScroll();
	}

	void ScrollAreaWidget::scrollY(float y_offset) {
		if (!scrolled_widget) {
			return;
		}
		float pos_y = scrolled_widget->getAnchorOffset().y - y_offset;
		float scroll_range = getScrollYRange();
		pos_y = std::clamp(pos_y, -scroll_range, 0.0f);
		scrolled_widget->setAnchorOffsetY(pos_y);
		updateScroll();
	}

}
