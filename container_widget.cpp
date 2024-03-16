#include "container_widget.h"
#include "widget_list.h"

ContainerWidget::ContainerWidget(WidgetList& widget_list) : RectangleWidget(widget_list) {
	setName("container");
}

void ContainerWidget::setAutoResize(bool value) {
	this->auto_resize = value;
}

void ContainerWidget::setHorizontal(bool value) {
	this->horizontal = value;
}

void ContainerWidget::setPadding(float padding) {
	this->horizontal_padding = padding;
	this->vertical_padding = padding;
}

void ContainerWidget::setHorizontalPadding(float padding) {
	this->horizontal_padding = padding;
}

void ContainerWidget::setVerticalPadding(float padding) {
	this->vertical_padding = padding;
}

void ContainerWidget::setVerticalAlignment(Alignment alignment) {
	this->vertical_alignment = alignment;
}

void ContainerWidget::setHorizontalAlignment(Alignment alignment) {
	this->horizontal_alignment = alignment;
}

void ContainerWidget::update() {
	Widget::update();
	sf::FloatRect container_bounds = sf::FloatRect();
	float max_width = 0.0f, max_height = 0.0f;
	for (size_t i = 0; i < children.size(); i++) {
		if (children[i]->getWidth() > max_width) {
			max_width = children[i]->getWidth();
		}
		if (children[i]->getHeight() > max_height) {
			max_height = children[i]->getHeight();
		}
	}
	float next_x = horizontal_padding, next_y = vertical_padding;
	if (horizontal) {
		next_y += alignmentToOffset(vertical_alignment, max_height);
	} else {
		next_x += alignmentToOffset(horizontal_alignment, max_width);
	}
	for (size_t i = 0; i < children.size(); i++) {
		Widget* child = children[i];
		child->setPosition(next_x, next_y);
		sf::FloatRect child_bounds = child->getParentLocalBounds();
		utils::extend_bounds(container_bounds, child_bounds);
		if (horizontal) {
			child->setOrigin(alignmentToAnchor(vertical_alignment));
			next_x += child->getWidth() + horizontal_padding;
		} else {
			child->setOrigin(alignmentToAnchor(horizontal_alignment));
			next_y += child->getHeight() + vertical_padding;
		}
	}
	if (auto_resize) {
		setSize(sf::Vector2f(
			container_bounds.width + horizontal_padding,
			container_bounds.height + vertical_padding
		));
	}
}

Widget::Anchor ContainerWidget::alignmentToAnchor(Alignment alignment) const {
	switch (alignment) {
		case Widget::ALIGN_TOP: return TOP_LEFT;
		case Widget::ALIGN_CENTER:
			if (horizontal) {
				return CENTER_LEFT;
			} else {
				return TOP_CENTER;
			}
			break;
		case Widget::ALIGN_BOTTOM: return BOTTOM_LEFT;
		case Widget::ALIGN_LEFT: return TOP_LEFT;
		case Widget::ALIGN_RIGHT: return TOP_RIGHT;
		default: assert(false, "Unknown alignment: " + std::to_string(alignment));
	}
}

float ContainerWidget::alignmentToOffset(Alignment alignment, float max_size) const {
	if (horizontal) {
		switch (vertical_alignment) {
			case ALIGN_TOP: return 0.0f;
			case ALIGN_CENTER: return max_size / 2.0f;
			case ALIGN_BOTTOM: return max_size;
		}
	} else {
		switch (horizontal_alignment) {
			case ALIGN_LEFT: return 0.0f;
			case ALIGN_CENTER: return max_size / 2.0f;
			case ALIGN_RIGHT: return max_size;
		}
	}
}