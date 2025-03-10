#include "widgets/window_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	enum class WindowRenderLayers {
		RESIZE,
		WINDOW,
		HEADER,
		OUTLINE,
	};

	WindowWidget::WindowWidget(WidgetList& widget_list, float width, float height) : EmptyWidget(widget_list) {
		wAssert(WINDOW_ONSCREEN_MARGIN <= WINDOW_MIN_SIZE.x && WINDOW_ONSCREEN_MARGIN <= WINDOW_MIN_SIZE.y);
		type = WidgetType::Window;
		// empty
		setName("window");
		setSizeInternal(width, height + WINDOW_HEADER_HEIGHT);
		// header
		header_widget = widget_list.createRectangleWidget(width, WINDOW_HEADER_HEIGHT);
		header_widget->setOrigin(Anchor::TOP_LEFT);
		header_widget->setClipChildren(true);
		header_widget->setClickThrough(false);
		header_widget->setName("header");
		header_widget->setSizeXPolicy(SizePolicy::PARENT);
		header_size = getSize();
		header_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
			is_grabbed = true;
			header_click_offset = getGlobalPosition() - pos;
			widget_list.addPostAction([this](WidgetList& widget_list) {
				wAssert(widget_list.contains(this));
				moveToTop();
			}, PostActionStage::MOVE);
		};
		header_widget->OnProcessMouse += [&](const sf::Vector2f& pos) {
			if (is_grabbed) {
				sf::Vector2f parent_local_pos = parent->toLocal(pos);
				sf::Vector2f new_pos = parent_local_pos + header_click_offset;
				sf::FloatRect parent_local_bounds = header_widget->getParentLocalBounds();
				sf::Vector2f parent_size = parent->getSize();
				new_pos.x = std::clamp(
					new_pos.x,
					WINDOW_ONSCREEN_MARGIN - parent_local_bounds.width,
					parent_size.x - WINDOW_ONSCREEN_MARGIN
				);
				new_pos.y = std::clamp(new_pos.y,
					WINDOW_ONSCREEN_MARGIN - parent_local_bounds.height,
					parent_size.y - WINDOW_ONSCREEN_MARGIN
				);
				setTransformPosition(new_pos);
			}
		};
		header_widget->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
			is_grabbed = false;
		};
		header_widget->setParent(this);
		header_widget->setParentLocalRenderLayer(static_cast<size_t>(WindowRenderLayers::HEADER));
		// header text
		header_text_widget = widget_list.createTextWidget();
		header_text_widget->setOrigin(Anchor::TOP_LEFT);
		header_text_widget->setParentAnchor(Anchor::TOP_LEFT);
		header_text_widget->setAnchorOffset(WINDOW_HEADER_TEXT_PADDING, 0.0f);
		header_text_widget->setName("text");
		header_text_widget->setParent(header_widget);
		setHeaderText("New Window");
		setHeaderColor(WINDOW_HEADER_DEFAULT_COLOR);
		setHeaderTextColor(WINDOW_HEADER_TEXT_DEFAULT_COLOR);
		setHeaderTextCharacterSize(WINDOW_HEADER_TEXT_DEFAULT_CHARACTER_SIZE);
		// main widget
		main_widget = widget_list.createRectangleWidget(width, height);
		main_widget->setName("main");
		main_widget->setParentAnchor(Anchor::TOP_LEFT);
		main_widget->setAnchorOffset(0.0f, WINDOW_HEADER_HEIGHT);
		main_widget->setFillColor(WINDOW_DEFAULT_COLOR);
		main_widget->setClipChildren(true);
		main_widget->setClickThrough(false);
		main_widget->setForceCustomCursor(true);
		main_widget->setParent(this);
		main_widget->setParentLocalRenderLayer(static_cast<size_t>(WindowRenderLayers::WINDOW));
		// resize widget
		sf::Vector2f resize_size(width + WINDOW_RESIZE_MARGIN * 2, WINDOW_HEADER_HEIGHT + height + WINDOW_RESIZE_MARGIN * 2);
		resize_widget = widget_list.createEmptyWidget();
		resize_widget->setName("resize");
		resize_widget->setSize(resize_size);
		resize_widget->setParentAnchor(Anchor::TOP_LEFT);
		resize_widget->setAnchorOffset(-WINDOW_RESIZE_MARGIN, -WINDOW_RESIZE_MARGIN);
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
		resize_widget->OnLeftPress += [&](const sf::Vector2f& pos) {
			sf::Vector2f parent_local_pos = parent->toLocal(pos);
			active_resizing_type = getResizingType();
			if (active_resizing_type == Resizing::NONE) {
				// skip
			} else if (active_resizing_type == Resizing::TOP_LEFT) {
				resizing_anchor = getBottomRight();
				resizing_cursor_offset = parent_local_pos - getTopLeft();
			} else if (active_resizing_type == Resizing::TOP) {
				resizing_anchor = getBottom();
				resizing_cursor_offset = parent_local_pos - getTop();
			} else if (active_resizing_type == Resizing::TOP_RIGHT) {
				resizing_anchor = getBottomLeft();
				resizing_cursor_offset = parent_local_pos - getTopRight();
			} else if (active_resizing_type == Resizing::LEFT) {
				resizing_anchor = getRight();
				resizing_cursor_offset = parent_local_pos - getLeft();
			} else if (active_resizing_type == Resizing::RIGHT) {
				resizing_anchor = getLeft();
				resizing_cursor_offset = parent_local_pos - getRight();
			} else if (active_resizing_type == Resizing::BOTTOM_LEFT) {
				resizing_anchor = getTopRight();
				resizing_cursor_offset = parent_local_pos - getBottomLeft();
			} else if (active_resizing_type == Resizing::BOTTOM) {
				resizing_anchor = getTop();
				resizing_cursor_offset = parent_local_pos - getBottom();
			} else if (active_resizing_type == Resizing::BOTTOM_RIGHT) {
				resizing_anchor = getTopLeft();
				resizing_cursor_offset = parent_local_pos - getBottomRight();
			}
			widget_list.addPostAction([this](WidgetList& widget_list) {
				wAssert(widget_list.contains(this));
				moveToTop();
			}, PostActionStage::MOVE);
		};
		resize_widget->OnProcessMouse += [&](const sf::Vector2f& pos) {
			if (active_resizing_type == Resizing::NONE) {
				return;
			}
			sf::Vector2f global_mouse_pos = widget_list.getMousePosf();
			sf::Vector2f anchored_pos = global_mouse_pos - resizing_cursor_offset;
			sf::Vector2f mouse_pos = parent->toLocal(anchored_pos);
			float x_min = std::min(mouse_pos.x, resizing_anchor.x - WINDOW_MIN_SIZE.x);
			float x_max = std::max(mouse_pos.x, resizing_anchor.x + WINDOW_MIN_SIZE.x);
			float y_min = std::min(mouse_pos.y, resizing_anchor.y - WINDOW_MIN_SIZE.y - WINDOW_HEADER_HEIGHT);
			float y_max = std::max(mouse_pos.y, resizing_anchor.y + WINDOW_MIN_SIZE.y + WINDOW_HEADER_HEIGHT);
			float clamped_x_min = std::min(x_min, parent->getWidth() - WINDOW_ONSCREEN_MARGIN);
			float clamped_x_max = std::max(x_max, WINDOW_ONSCREEN_MARGIN);
			float clamped_y_min = std::clamp(y_min, 0.0f, parent->getHeight() - WINDOW_ONSCREEN_MARGIN);
			float clamped_y_max = y_max;
			float width_min = resizing_anchor.x - clamped_x_min;
			float width_max = clamped_x_max - resizing_anchor.x;
			float height_min = resizing_anchor.y - clamped_y_min - WINDOW_HEADER_HEIGHT;
			float height_max = clamped_y_max - resizing_anchor.y - WINDOW_HEADER_HEIGHT;
			float old_x = getTransformPosition().x;
			float old_y = getTransformPosition().y;
			float old_width = main_widget->getWidth();
			float old_height = main_widget->getHeight();
			if (active_resizing_type == Resizing::TOP_LEFT) {
				setTransformPosition(clamped_x_min, clamped_y_min);
				setSizeKeepPos(width_min, height_min + header_widget->getHeight());
			} else if (active_resizing_type == Resizing::TOP) {
				setTransformPosition(old_x, clamped_y_min);
				setSizeKeepPos(old_width, height_min + header_widget->getHeight());
			} else if (active_resizing_type == Resizing::TOP_RIGHT) {
				setTransformPosition(old_x, clamped_y_min);
				setSizeKeepPos(width_max, height_min + header_widget->getHeight());
			} else if (active_resizing_type == Resizing::LEFT) {
				setTransformPosition(clamped_x_min, old_y);
				setSizeKeepPos(width_min, old_height + header_widget->getHeight());
			} else if (active_resizing_type == Resizing::RIGHT) {
				setTransformPosition(old_x, old_y);
				setSizeKeepPos(width_max, old_height + header_widget->getHeight());
			} else if (active_resizing_type == Resizing::BOTTOM_LEFT) {
				setTransformPosition(clamped_x_min, old_y);
				setSizeKeepPos(width_min, height_max + header_widget->getHeight());
			} else if (active_resizing_type == Resizing::BOTTOM) {
				setTransformPosition(old_x, old_y);
				setSizeKeepPos(old_width, height_max + header_widget->getHeight());
			} else if (active_resizing_type == Resizing::BOTTOM_RIGHT) {
				setTransformPosition(old_x, old_y);
				setSizeKeepPos(width_max, height_max + header_widget->getHeight());
			}
		};
		resize_widget->OnGlobalLeftRelease += [&](const sf::Vector2f& pos) {
			active_resizing_type = Resizing::NONE;
		};
		resize_widget->setParent(this);
		resize_widget->setParentLocalRenderLayer(static_cast<size_t>(WindowRenderLayers::RESIZE));
		// outline
		outline_widget = widget_list.createRectangleWidget(width, height + WINDOW_HEADER_HEIGHT);
		outline_widget->setName("outline");
		outline_widget->setSize(width, WINDOW_HEADER_HEIGHT + height);
		outline_widget->setFillColor(sf::Color::Transparent);
		outline_widget->setSizePolicy(SizePolicy::PARENT);
		outline_widget->OnAfterRender += [&](sf::RenderTarget& target) {
			sf::FloatRect quantized_bounds = quantize_rect(
				outline_widget->getLocalBounds(),
				QUANTIZE_MODE_FLOOR_SUBTRACT
			);
			draw_wire_rect(target, quantized_bounds, outline_color, outline_widget->getGlobalTransform());
		};
		outline_widget->setParent(this);
		outline_widget->setParentLocalRenderLayer(static_cast<size_t>(WindowRenderLayers::OUTLINE));

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

	const fw::Font& WindowWidget::getHeaderFont() const {
		return header_text_widget->getFont();
	}

	unsigned int WindowWidget::getHeaderTextCharacterSize() const {
		return header_text_widget->getCharacterSize();
	}

	const sf::Color& WindowWidget::getOutlineColor() const {
		return outline_color;
	}

	const CompVector<Widget*>& WindowWidget::getWindowChildren() const {
		return main_widget->getChildren();
	}

	RectangleWidget* WindowWidget::getHeaderWidget() const {
		return header_widget;
	}

	RectangleWidget* WindowWidget::getMainWidget() const {
		return main_widget;
	}

	EmptyWidget* WindowWidget::getResizeWidget() const {
		return resize_widget;
	}

	RectangleWidget* WindowWidget::getOutlineWidget() const {
		return outline_widget;
	}

	void WindowWidget::setHeaderVisible(bool value) {
		if (value) {
			header_widget->setVisible(true);
			header_widget->setSize(header_size);
		} else {
			header_widget->setVisible(false);
			header_size = header_widget->getSize();
			header_widget->setSize(sf::Vector2f());
		}
	}

	void WindowWidget::setHeaderColor(const sf::Color& color) {
		header_widget->setFillColor(color);
	}

	void WindowWidget::setHeaderText(const sf::String& text) {
		header_text_widget->setString(text);
	}

	void WindowWidget::setHeaderTextColor(const sf::Color& color) {
		header_text_widget->setFillColor(color);
	}

	void WindowWidget::setHeaderFont(const fw::Font& font) {
		header_text_widget->setFont(font);
	}

	void WindowWidget::setHeaderTextCharacterSize(unsigned int size) {
		header_text_widget->setCharacterSize(size);
	}

	void WindowWidget::setSize(float width, float height) {
		setSizeInternal(width, height);
		main_widget->setSize(width, std::max(0.0f, height - header_widget->getHeight()));
		resize_widget->setSize(
			getWidth() + WINDOW_RESIZE_MARGIN * 2,
			getHeight() + WINDOW_RESIZE_MARGIN * 2
		);
	}

	void WindowWidget::setOutlineColor(const sf::Color& color) {
		this->outline_color = color;
	}

	void WindowWidget::addChild(Widget* child) {
		if (children_locked) {
			main_widget->addChild(child);
		} else {
			Widget::addChild(child);
		}
	}

	void WindowWidget::setSizeInternal(float width, float height) {
		EmptyWidget::setSizeInternal(width, height);
	}

	WindowWidget::Resizing WindowWidget::getResizingType() const {
		float r_width = resize_widget->getWidth();
		float r_height = resize_widget->getHeight();
		sf::Vector2f mpos = widget_list.getMousePosf() - resize_widget->getGlobalPosition();
		bool x_left = mpos.x < WINDOW_RESIZE_MARGIN;
		bool x_center = mpos.x >= WINDOW_RESIZE_MARGIN && mpos.x < r_width - WINDOW_RESIZE_MARGIN;
		bool x_right = mpos.x >= r_width - WINDOW_RESIZE_MARGIN;
		bool y_top = mpos.y < WINDOW_RESIZE_MARGIN;
		bool y_center = mpos.y >= WINDOW_RESIZE_MARGIN && mpos.y < r_height - WINDOW_RESIZE_MARGIN;
		bool y_bottom = mpos.y >= r_height - WINDOW_RESIZE_MARGIN;
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