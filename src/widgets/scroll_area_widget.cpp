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
		// area
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
		scrollbar_x_widget = widget_list.createWidget<RectangleWidget>(area_width, default_width);
		scrollbar_x_widget->setName("slider bg x");
		scrollbar_x_widget->setFillColor(SCROLL_AREA_DEFAULT_SCROLL_BACKGROUND_COLOR);
		scrollbar_x_widget->setParentAnchor(Anchor::BOTTOM_LEFT);
		scrollbar_x_widget->setOrigin(Anchor::BOTTOM_LEFT);
		scrollbar_x_widget->setClickThrough(false);
		scrollbar_x_widget->setParent(this);
		// slider x
		slider_x_widget = widget_list.createWidget<RectangleWidget>(area_width / 2.0f, default_width);
		slider_x_widget->setName("slider x");
		slider_x_widget->setFillColor(SCROLL_AREA_DEFAULT_SLIDER_COLOR);
		slider_x_widget->setClickThrough(false);
		slider_x_widget->setQuantizeRenderedPosition(false); // slider might be in non-integer positions
		slider_x_widget->setParent(scrollbar_x_widget);
		slider_x_widget->OnMouseEnter += [&](const sf::Vector2f& pos) {
			slider_x_hover = true;
			updateColors();
		};
		slider_x_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
			slider_x_grabbed = true;
			mouse_grab_pos = pos.x;
			slider_grab_pos = slider_x_widget->getPosition().x;
			updateColors();
		};
		slider_x_widget->OnMouseMoved += [&](const sf::Vector2f& pos) {
			if (slider_x_grabbed) {
				float x_offset = pos.x - mouse_grab_pos;
				setSliderX(slider_grab_pos + x_offset);
			}
		};
		slider_x_widget->OnLeftRelease += [&](const sf::Vector2f& pos) {
			slider_x_grabbed = false;
			updateColors();
		};
		slider_x_widget->OnMouseExit += [&](const sf::Vector2f& pos) {
			slider_x_hover = false;
			updateColors();
		};
		// slider y bg
		scrollbar_y_widget = widget_list.createWidget<RectangleWidget>(default_width, area_height);
		scrollbar_y_widget->setName("slider bg y");
		scrollbar_y_widget->setFillColor(SCROLL_AREA_DEFAULT_SCROLL_BACKGROUND_COLOR);
		scrollbar_y_widget->setParentAnchor(Anchor::TOP_RIGHT);
		scrollbar_y_widget->setOrigin(Anchor::TOP_RIGHT);
		scrollbar_y_widget->setClickThrough(false);
		scrollbar_y_widget->setParent(this);
		// slider y
		slider_y_widget = widget_list.createWidget<RectangleWidget>(default_width, area_height / 2.0f);
		slider_y_widget->setName("slider y");
		slider_y_widget->setFillColor(SCROLL_AREA_DEFAULT_SLIDER_COLOR);
		slider_y_widget->setClickThrough(false);
		slider_y_widget->setQuantizeRenderedPosition(false);
		slider_y_widget->setParent(scrollbar_y_widget);
		slider_y_widget->OnMouseEnter += [&](const sf::Vector2f& pos) {
			slider_y_hover = true;
			updateColors();
		};
		slider_y_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
			slider_y_grabbed = true;
			mouse_grab_pos = pos.y;
			slider_grab_pos = slider_y_widget->getPosition().y;
			updateColors();
		};
		slider_y_widget->OnMouseMoved += [&](const sf::Vector2f& pos) {
			if (slider_y_grabbed) {
				float y_offset = pos.y - mouse_grab_pos;
				setSliderY(slider_grab_pos + y_offset);
			}
		};
		slider_y_widget->OnLeftRelease += [&](const sf::Vector2f& pos) {
			slider_y_grabbed = false;
			updateColors();
		};
		slider_y_widget->OnMouseExit += [&](const sf::Vector2f& pos) {
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

		updateScroll();
	}

	ScrollAreaWidget::ScrollAreaWidget(WidgetList& widget_list, const sf::Vector2f& size)
		: ScrollAreaWidget(widget_list, size.x, size.y) { }

	Widget* ScrollAreaWidget::getScrolledWidget() const {
		return scrolled_widget;
	}

	Widget* ScrollAreaWidget::getAreaWidget() const {
		return area_widget;
	}

	Widget* ScrollAreaWidget::getScrollbarXWidget() const {
		return scrollbar_x_widget;
	}

	Widget* ScrollAreaWidget::getScrollbarYWidget() const {
		return scrollbar_y_widget;
	}

	Widget* ScrollAreaWidget::getSliderXWidget() const {
		return slider_x_widget;
	}

	Widget* ScrollAreaWidget::getSliderYWidget() const {
		return slider_y_widget;
	}

	Widget* ScrollAreaWidget::getCornerWidget() const {
		return corner_widget;
	}

	void ScrollAreaWidget::setScrolledWidget(Widget* widget) {
		this->scrolled_widget = widget;
		widget->setOrigin(Anchor::TOP_LEFT);
		widget->setParentAnchor(Anchor::TOP_LEFT);
		widget->setAnchorOffset(0.0f, 0.0f);
		widget->setParent(area_widget);
	}

	Widget* ScrollAreaWidget::takeScrolledWidget() {
		Widget* widget = scrolled_widget;
		widget->setParent(getParent());
		scrolled_widget = nullptr;
		return widget;
	}

	void ScrollAreaWidget::setBackgroundColor(const sf::Color& color) {
		area_widget->setFillColor(color);
	}

	void ScrollAreaWidget::setScrollbarColor(const sf::Color& color) {
		scrollbar_x_widget->setFillColor(color);
		scrollbar_y_widget->setFillColor(color);
	}

	void ScrollAreaWidget::setSliderColor(const sf::Color& color) {
		slider_x_widget->setFillColor(color);
		slider_y_widget->setFillColor(color);
	}

	void ScrollAreaWidget::setDeltaX(float delta) {
		this->delta_x = delta;
	}

	void ScrollAreaWidget::setDeltaY(float delta) {
		this->delta_y = delta;
	}

	void ScrollAreaWidget::setScrollbarXPolicy(ScrollbarPolicy policy) {
		this->scrollbar_x_policy = policy;
	}

	void ScrollAreaWidget::setScrollbarYPolicy(ScrollbarPolicy policy) {
		this->scrollbar_y_policy = policy;
	}

	void ScrollAreaWidget::internalPreUpdate() {
		updateScrollbarVisibility();
	}

	void ScrollAreaWidget::internalPostUpdate() {
		area_widget->setWidth(getWidth() - getSliderBgYEffectiveWidth());
		area_widget->setHeight(getHeight() - getSliderBgXEffectiveHeight());
		scrollbar_x_widget->setWidth(getWidth() - getSliderBgYEffectiveWidth());
		scrollbar_y_widget->setHeight(getHeight() - getSliderBgXEffectiveHeight());
		if (scrolled_widget) {
			float right_offset = area_widget->getRight().x - scrolled_widget->getRight().x;
			if (right_offset > 0.0f) {
				if (getScrollXRange() > 0.0f) {
					scrollX(right_offset);
				} else {
					scrolled_widget->setAnchorOffsetX(0.0f);
				}
			}
			float bottom_offset = area_widget->getBottom().y - scrolled_widget->getBottom().y;
			if (bottom_offset > 0.0f) {
				if (getScrollYRange() > 0.0f) {
					scrollY(bottom_offset);
				} else {
					scrolled_widget->setAnchorOffsetY(0.0f);
				}
			}
		}
		updateScroll();
	}

	void ScrollAreaWidget::internalOnScrollX(const sf::Vector2f& pos, float delta) {
		scrollX(delta_x * -delta);
	}

	void ScrollAreaWidget::internalOnScrollY(const sf::Vector2f& pos, float delta) {
		scrollY(delta_y * -delta);
	}

	bool ScrollAreaWidget::getScrollbarXVisibleIndividual() {
		if (scrollbar_x_policy == ScrollbarPolicy::OFF) {
			return false;
		} else if (scrollbar_x_policy == ScrollbarPolicy::ON) {
			return false;
		} else {
			if (!scrolled_widget) {
				return false;
			} else {
				float scrolled_width = scrolled_widget->getWidth();
				float width = getWidth();
				return scrolled_widget->getWidth() > getWidth();
			}
		}
	}

	bool ScrollAreaWidget::getScrollbarYVisibleIndividual() {
		if (scrollbar_y_policy == ScrollbarPolicy::OFF) {
			return false;
		} else if (scrollbar_y_policy == ScrollbarPolicy::ON) {
			return true;
		} else {
			if (!scrolled_widget) {
				return false;
			} else {
				return scrolled_widget->getHeight() > getHeight();
			}
		}
	}

	void ScrollAreaWidget::updateScrollbarVisibility() {
		// Calculation of whether scrollbars are visible must be done in two stages:
		// First, calculate whether scrollbar is visible without taking other scrollbar into account,
		// second, calculate final states given individual states.
		// This helps to avoid issue when scroll area is almost as big as scrolled widget,
		// and the size of the scroll area is changing.
		bool x_state = false;
		bool y_state = false;
		bool x_state_individual = getScrollbarXVisibleIndividual();
		bool y_state_individual = getScrollbarYVisibleIndividual();
		bool x_state_individual_is_final = !scrolled_widget || (scrollbar_x_policy != ScrollbarPolicy::SIZE);
		bool y_state_individual_is_final = !scrolled_widget || (scrollbar_y_policy != ScrollbarPolicy::SIZE);
		if (x_state_individual_is_final) {
			x_state = x_state_individual;
		} else {
			float scrollbar_y_effective_width = y_state_individual ? scrollbar_y_widget->getWidth() : 0.0f;
			x_state = scrolled_widget->getWidth() > getWidth() - scrollbar_y_effective_width;
		}
		if (y_state_individual_is_final) {
			y_state = y_state_individual;
		} else {
			float scrollbar_x_effective_height = x_state_individual ? scrollbar_x_widget->getHeight() : 0.0f;
			y_state = scrolled_widget->getHeight() > getHeight() - scrollbar_x_effective_height;
		}
		bool corner_state = x_state && y_state;
		scrollbar_x_widget->setVisible(x_state);
		scrollbar_y_widget->setVisible(y_state);
		corner_widget->setVisible(corner_state);
	}

	void ScrollAreaWidget::updateScroll() {
		// size
		float x_width = scrollbar_x_widget->getWidth() * getSizeXFactor();
		float y_height = scrollbar_y_widget->getHeight() * getSizeYFactor();
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
		float x_range = scrollbar_x_widget->getWidth() - slider_x_widget->getWidth();
		return x_range;
	}

	float ScrollAreaWidget::getYRange() const {
		float y_range = scrollbar_y_widget->getHeight() - slider_y_widget->getHeight();
		return y_range;
	}

	float ScrollAreaWidget::getScrollXRange() const {
		if (!scrolled_widget) {
			return 0.0f;
		}
		float scroll_range = scrolled_widget->getWidth() - area_widget->getWidth();
		return scroll_range;
	}

	float ScrollAreaWidget::getScrollYRange() const {
		if (!scrolled_widget) {
			return 0.0f;
		}
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

	float ScrollAreaWidget::getSliderBgYEffectiveWidth() const {
		float slider_y_bg_width =
			scrollbar_y_widget->isVisible() ?
			scrollbar_y_widget->getWidth() :
			0.0f;
		return slider_y_bg_width;
	}

	float ScrollAreaWidget::getSliderBgXEffectiveHeight() const {
		float slider_x_bg_height =
			scrollbar_x_widget->isVisible() ?
			scrollbar_x_widget->getHeight() :
			0.0f;
		return slider_x_bg_height;
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
		if (scroll_range > 0.0f) {
			pos_x = std::clamp(pos_x, -scroll_range, 0.0f);
			scrolled_widget->setAnchorOffsetX(pos_x);
			updateScroll();
		}
	}

	void ScrollAreaWidget::scrollY(float y_offset) {
		if (!scrolled_widget) {
			return;
		}
		float pos_y = scrolled_widget->getAnchorOffset().y - y_offset;
		float scroll_range = getScrollYRange();
		if (scroll_range > 0.0f) {
			pos_y = std::clamp(pos_y, -scroll_range, 0.0f);
			scrolled_widget->setAnchorOffsetY(pos_y);
			updateScroll();
		}
	}

}
