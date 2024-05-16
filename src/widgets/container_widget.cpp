#include "widgets/container_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	ContainerWidget::ContainerWidget(WidgetList& widget_list, float width, float height)
		: RectangleWidget(widget_list, width, height) {
		setName("container");
	}

	ContainerWidget::ContainerWidget(WidgetList& widget_list, const sf::Vector2f& size) 
		: ContainerWidget(widget_list, size.x, size.y) { }

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

	void ContainerWidget::internalUpdate() {
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
			extend_bounds(container_bounds, child_bounds);
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
			case Alignment::ALIGN_TOP: return Anchor::TOP_LEFT;
			case Alignment::ALIGN_CENTER:
				if (horizontal) {
					return Anchor::CENTER_LEFT;
				} else {
					return Anchor::TOP_CENTER;
				}
				break;
			case Alignment::ALIGN_BOTTOM: return Anchor::BOTTOM_LEFT;
			case Alignment::ALIGN_LEFT: return Anchor::TOP_LEFT;
			case Alignment::ALIGN_RIGHT: return Anchor::TOP_RIGHT;
			default: wAssert(false, "Unknown alignment: " + std::to_string((int)alignment)); return Anchor::TOP_LEFT;
		}
	}

	float ContainerWidget::alignmentToOffset(Alignment alignment, float max_size) const {
		if (horizontal) {
			switch (vertical_alignment) {
				case Alignment::ALIGN_TOP: return 0.0f;
				case Alignment::ALIGN_CENTER: return max_size / 2.0f;
				case Alignment::ALIGN_BOTTOM: return max_size;
				default: return 0.0f;
			}
		} else {
			switch (horizontal_alignment) {
				case Alignment::ALIGN_LEFT: return 0.0f;
				case Alignment::ALIGN_CENTER: return max_size / 2.0f;
				case Alignment::ALIGN_RIGHT: return max_size;
				default: return 0.0f;
			}
		}
	}

}
