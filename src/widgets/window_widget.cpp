#include "widgets/window_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	enum class WindowRenderLayers {
		RESIZE,
		WINDOW,
	};

	WindowWidget::WindowWidget(WidgetList& widget_list, float width, float height) : RectangleWidget(widget_list) {
		wAssert(ONSCREEN_MARGIN <= MIN_WINDOW_SIZE.x && ONSCREEN_MARGIN <= MIN_WINDOW_SIZE.y);
		// header
		setSize(width, HEADER_HEIGHT);
		setOrigin(Anchor::TOP_LEFT);
		setClickThrough(false);
		setName("window header");
		header_size = getSize();
		OnLeftPress = [&](const sf::Vector2f& pos) {
			is_grabbed = true;
			header_click_offset = getGlobalPosition() - pos;
		};
		OnProcessMouse = [&](const sf::Vector2f& pos) {
			if (is_grabbed) {
				sf::Vector2f new_pos = pos + header_click_offset;
				sf::FloatRect global_bounds = getGlobalBounds();
				sf::Vector2u window_size = widget_list.getWindowSize();
				new_pos.x = std::clamp(new_pos.x, ONSCREEN_MARGIN - global_bounds.width, window_size.x - ONSCREEN_MARGIN);
				new_pos.y = std::clamp(new_pos.y, ONSCREEN_MARGIN - global_bounds.height, window_size.y - ONSCREEN_MARGIN);
				setGlobalPosition(new_pos);
			}
		};
		OnLeftRelease = [&](const sf::Vector2f& pos) {
			is_grabbed = false;
		};
		setLocalRenderLayer(static_cast<size_t>(WindowRenderLayers::WINDOW));
		// header text
		header_text_widget = widget_list.createWidget<TextWidget>();
		header_text_widget->setOrigin(Anchor::TOP_LEFT);
		header_text_widget->setParentAnchor(Anchor::TOP_LEFT);
		header_text_widget->setAnchorOffset(HEADER_TEXT_PADDING, 0.0f);
		header_text_widget->setName("header text");
		header_text_widget->setParent(this);
		header_text_widget->setParentLocalRenderLayer(static_cast<size_t>(WindowRenderLayers::WINDOW));
		setHeaderText("New Window");
		setHeaderColor(DEFAULT_HEADER_COLOR);
		setHeaderTextColor(DEFAULT_HEADER_TEXT_COLOR);
		setHeaderTextCharacterSize(DEFAULT_HEADER_TEXT_CHARACTER_SIZE);
		// main widget
		main_widget = widget_list.createWidget<RectangleWidget>();
		main_widget->setName("window area");
		main_widget->setSize(width, height);
		main_widget->setParentAnchor(Anchor::BOTTOM_LEFT);
		main_widget->setFillColor(DEFAULT_WINDOW_COLOR);
		main_widget->setClipChildren(true);
		main_widget->setClickThrough(false);
		main_widget->setForceCustomCursor(true);
		main_widget->setParent(this);
		main_widget->setParentLocalRenderLayer(static_cast<size_t>(WindowRenderLayers::WINDOW));
		// resize widget
		resize_widget = widget_list.createWidget<RectangleWidget>();
		resize_widget->setName("resize");
		resize_widget->setSize(width + RESIZE_WIDGET_MARGIN * 2, HEADER_HEIGHT + height + RESIZE_WIDGET_MARGIN * 2);
		resize_widget->setFillColor(sf::Color::Transparent);
		resize_widget->setParentAnchor(Anchor::TOP_LEFT);
		resize_widget->setAnchorOffset(-RESIZE_WIDGET_MARGIN, -RESIZE_WIDGET_MARGIN);
		resize_widget->setClickThrough(false);
		resize_widget->setForceCustomCursor(true);
		resize_widget->GetCursorType = [&]() {
			Resizing resizing_type = getResizingType();
			switch (resizing_type) {
				case Resizing::NONE: return sf::Cursor::Arrow;
				case Resizing::TOP_LEFT: return sf::Cursor::SizeTopLeft;
				case Resizing::TOP: return sf::Cursor::SizeTop;
				case Resizing::TOP_RIGHT: return sf::Cursor::SizeTopRight;
				case Resizing::LEFT: return sf::Cursor::SizeLeft;
				case Resizing::RIGHT: return sf::Cursor::SizeRight;
				case Resizing::BOTTOM_LEFT: return sf::Cursor::SizeBottomLeft;
				case Resizing::BOTTOM: return sf::Cursor::SizeBottom;
				case Resizing::BOTTOM_RIGHT: return sf::Cursor::SizeBottomRight;
				default: return sf::Cursor::Arrow;
			}
		};
		resize_widget->OnLeftPress = [&](const sf::Vector2f& pos) {
			resizing = getResizingType();
			switch (resizing) {
				case Resizing::NONE: break;
				case Resizing::TOP_LEFT: resizing_anchor = main_widget->getGlobalBottomRight(); break;
				case Resizing::TOP: resizing_anchor = main_widget->getGlobalBottomLeft(); break;
				case Resizing::TOP_RIGHT: resizing_anchor = main_widget->getGlobalBottomLeft(); break;
				case Resizing::LEFT: resizing_anchor = getGlobalTopRight(); break;
				case Resizing::RIGHT: resizing_anchor = getGlobalTopLeft(); break;
				case Resizing::BOTTOM_LEFT: resizing_anchor = getGlobalTopRight(); break;
				case Resizing::BOTTOM: resizing_anchor = getGlobalTopLeft(); break;
				case Resizing::BOTTOM_RIGHT: resizing_anchor = getGlobalTopLeft(); break;
			}
		};
		resize_widget->OnProcessMouse = [&](const sf::Vector2f& pos) {
			if (resizing == Resizing::NONE) {
				return;
			}
			sf::Vector2f mouse_pos = widget_list.getMousePosf();
			float x_min = std::min(mouse_pos.x, resizing_anchor.x - MIN_WINDOW_SIZE.x);
			float x_max = std::max(mouse_pos.x, resizing_anchor.x + MIN_WINDOW_SIZE.x);
			float y_min = std::min(mouse_pos.y, resizing_anchor.y - MIN_WINDOW_SIZE.y - HEADER_HEIGHT);
			float y_max = std::max(mouse_pos.y, resizing_anchor.y + MIN_WINDOW_SIZE.y + HEADER_HEIGHT);
			float clamped_x_min = std::min(x_min, widget_list.getWindowSize().x - ONSCREEN_MARGIN);
			float clamped_x_max = std::max(x_max, ONSCREEN_MARGIN);
			float clamped_y_min = std::clamp(y_min, 0.0f, widget_list.getWindowSize().y - ONSCREEN_MARGIN);
			float clamped_y_max = y_max;
			float width_min = resizing_anchor.x - clamped_x_min;
			float width_max = clamped_x_max - resizing_anchor.x;
			float height_min = resizing_anchor.y - clamped_y_min - HEADER_HEIGHT;
			float height_max = clamped_y_max - resizing_anchor.y - HEADER_HEIGHT;
			float old_x = resizing_anchor.x;
			float old_y = resizing_anchor.y;
			float old_width = main_widget->getWidth();
			float old_height = main_widget->getHeight();
			if (resizing == Resizing::TOP_LEFT) {
				setGlobalPosition(clamped_x_min, clamped_y_min);
				main_widget->setSize(width_min, height_min);
			} else if (resizing == Resizing::TOP) {
				setGlobalPosition(old_x, clamped_y_min);
				main_widget->setSize(old_width, height_min);
			} else if (resizing == Resizing::TOP_RIGHT) {
				setGlobalPosition(old_x, clamped_y_min);
				main_widget->setSize(width_max, height_min);
			} else if (resizing == Resizing::LEFT) {
				setGlobalPosition(clamped_x_min, old_y);
				main_widget->setSize(width_min, old_height);
			} else if (resizing == Resizing::RIGHT) {
				main_widget->setSize(width_max, old_height);
			} else if (resizing == Resizing::BOTTOM_LEFT) {
				setGlobalPosition(clamped_x_min, old_y);
				main_widget->setSize(width_min, height_max);
			} else if (resizing == Resizing::BOTTOM) {
				main_widget->setSize(old_width, height_max);
			} else if (resizing == Resizing::BOTTOM_RIGHT) {
				main_widget->setSize(width_max, height_max);
			}
		};
		resize_widget->OnLeftRelease = [&](const sf::Vector2f& pos) {
			resizing = Resizing::NONE;
		};
		resize_widget->setParent(this);
		resize_widget->setParentLocalRenderLayer(static_cast<size_t>(WindowRenderLayers::RESIZE));

		lockChildren();
	}

	WindowWidget::WindowWidget(WidgetList& widget_list, const sf::Vector2f& size) : WindowWidget(widget_list, size.x, size.y) { }

	bool WindowWidget::getHeaderVisible() const {
		return isVisible();
	}

	void WindowWidget::getHeaderColor() const {
		getFillColor();
	}

	const sf::String& WindowWidget::getHeaderText() const {
		return header_text_widget->getString();
	}

	const sf::Color& WindowWidget::getHeaderTextColor() const {
		return header_text_widget->getFillColor();
	}

	const sf::Font* WindowWidget::getHeaderFont() const {
		return header_text_widget->getFont();
	}

	unsigned int WindowWidget::getHeaderTextCharacterSize() const {
		return header_text_widget->getCharacterSize();
	}

	const CompVector<Widget*>& WindowWidget::getWindowChildren() const {
		return main_widget->getChildren();
	}

	void WindowWidget::setHeaderVisible(bool value) {
		if (value) {
			setSize(header_size);
		} else {
			header_size = getSize();
			setSize(sf::Vector2f());
		}
	}

	void WindowWidget::setHeaderColor(const sf::Color& color) {
		setFillColor(color);
	}

	void WindowWidget::setHeaderText(const sf::String& text) {
		header_text_widget->setString(text);
	}

	void WindowWidget::setHeaderTextColor(const sf::Color& color) {
		header_text_widget->setFillColor(color);
	}

	void WindowWidget::setHeaderFont(const sf::Font& font) {
		header_text_widget->setFont(font);
	}

	void WindowWidget::setHeaderTextCharacterSize(unsigned int size) {
		header_text_widget->setCharacterSize(size);
	}

	void WindowWidget::addChild(Widget* child) {
		if (children_locked) {
			wAssert(false, "Use window->addWindowChild(child) instead of child->setParent(window)");
		} else {
			Widget::addChild(child);
		}
	}

	void WindowWidget::addWindowChild(Widget* child) {
		child->setParent(main_widget);
	}

	void WindowWidget::internalUpdate() {
		setSize(main_widget->getSize().x, HEADER_HEIGHT);
		resize_widget->setSize(
			main_widget->getWidth() + RESIZE_WIDGET_MARGIN * 2,
			HEADER_HEIGHT + main_widget->getHeight() + RESIZE_WIDGET_MARGIN * 2
		);
	}

	WindowWidget::Resizing WindowWidget::getResizingType() const {
		float r_width = resize_widget->getWidth();
		float r_height = resize_widget->getHeight();
		sf::Vector2f mpos = widget_list.getMousePosf() - resize_widget->getGlobalPosition();
		bool x_left = mpos.x < RESIZE_WIDGET_MARGIN;
		bool x_center = mpos.x >= RESIZE_WIDGET_MARGIN && mpos.x < r_width - RESIZE_WIDGET_MARGIN;
		bool x_right = mpos.x >= r_width - RESIZE_WIDGET_MARGIN;
		bool y_top = mpos.y < RESIZE_WIDGET_MARGIN;
		bool y_center = mpos.y >= RESIZE_WIDGET_MARGIN && mpos.y < r_height - RESIZE_WIDGET_MARGIN;
		bool y_bottom = mpos.y >= r_height - RESIZE_WIDGET_MARGIN;
		if (x_left && y_top) {
			return Resizing::TOP_LEFT;
		} else if (x_center && y_top) {
			return Resizing::TOP;
		} else if (x_right && y_top) {
			return Resizing::TOP_RIGHT;
		} else if (x_left && y_center) {
			return Resizing::LEFT;
		} else if (x_right && y_center) {
			return Resizing::RIGHT;
		} else if (x_left && y_bottom) {
			return Resizing::BOTTOM_LEFT;
		} else if (x_center && y_bottom) {
			return Resizing::BOTTOM;
		} else if (x_right && y_bottom) {
			return Resizing::BOTTOM_RIGHT;
		} else {
			return Resizing::NONE;
		}
	}

}