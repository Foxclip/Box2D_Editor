#include "widgets/window_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	WindowWidget::WindowWidget(WidgetList& widget_list, float width, float height) : RectangleWidget(widget_list) {
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
		// header text
		header_text_widget = widget_list.createWidget<TextWidget>();
		header_text_widget->setOrigin(Anchor::TOP_LEFT);
		header_text_widget->setParentAnchor(Anchor::TOP_LEFT);
		header_text_widget->setAnchorOffset(HEADER_TEXT_PADDING, 0.0f);
		header_text_widget->setName("header text");
		header_text_widget->setParent(this);
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
		// resize widget
		resize_widget = widget_list.createWidget<RectangleWidget>();
		resize_widget->setName("resize");
		resize_widget->setSize(width + RESIZE_WIDGET_MARGIN * 2, height + RESIZE_WIDGET_MARGIN * 2);
		resize_widget->setParentAnchor(Anchor::TOP_LEFT);
		resize_widget->setAnchorOffset(-RESIZE_WIDGET_MARGIN, -RESIZE_WIDGET_MARGIN);
		resize_widget->setFillColor(sf::Color::Blue);
		resize_widget->setClickThrough(false);
		resize_widget->setForceCustomCursor(true);
		resize_widget->GetCursorType = [&]() {
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
				return sf::Cursor::SizeTopLeft;
			} else if (x_center && y_top) {
				return sf::Cursor::SizeTop;
			} else if (x_right && y_top) {
				return sf::Cursor::SizeTopRight;
			} else if (x_left && y_center) {
				return sf::Cursor::SizeLeft;
			} else if (x_right && y_center) {
				return sf::Cursor::SizeRight;
			} else if (x_left && y_bottom) {
				return sf::Cursor::SizeBottomLeft;
			} else if (x_center && y_bottom) {
				return sf::Cursor::SizeBottom;
			} else if (x_right && y_bottom) {
				return sf::Cursor::SizeBottomRight;
			} else {
				return sf::Cursor::Arrow;
			}
		};
		resize_widget->setParent(this);

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

}