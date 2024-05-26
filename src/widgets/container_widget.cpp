#include "widgets/container_widget.h"
#include "widgets/widget_list.h"

namespace fw {

	ContainerWidget::ContainerWidget(WidgetList& widget_list, float width, float height)
		: RectangleWidget(widget_list, width, height) {
		type = WidgetType::Container;
		setName("container");
		setSizeXPolicy(SizePolicy::CHILDREN);
		setSizeYPolicy(SizePolicy::CHILDREN);
	}

	ContainerWidget::ContainerWidget(WidgetList& widget_list, const sf::Vector2f& size) 
		: ContainerWidget(widget_list, size.x, size.y) { }

	bool ContainerWidget::getHorizontal() const {
		return horizontal;
	}

	float ContainerWidget::getHorizontalPadding() const {
		return horizontal_padding;
	}

	float ContainerWidget::getVerticalPadding() const {
		return vertical_padding;
	}

	Widget::Alignment ContainerWidget::getVerticalAlignment() const {
		return vertical_alignment;
	}

	Widget::Alignment ContainerWidget::getHorizontalAlignment() const {
		return horizontal_alignment;
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

	void ContainerWidget::internalUpdate() { }

	void ContainerWidget::updateSizeX() {
		sf::Vector2f new_pos = getPosition();
		sf::Vector2f new_size = getSize();
		if (horizontal_size_policy == SizePolicy::CHILDREN) {
			new_size.x = children_bounds.width + horizontal_padding;
		} else {
			Widget::updateSizeX();
			return;
		}
		setPosition(new_pos);
		setSize(new_size);
	}

	void ContainerWidget::updateSizeY() {
		sf::Vector2f new_pos = getPosition();
		sf::Vector2f new_size = getSize();
		if (vertical_size_policy == SizePolicy::CHILDREN) {
			new_size.y = children_bounds.height + vertical_padding;
		} else {
			Widget::updateSizeY();
			return;
		}
		setPosition(new_pos);
		setSize(new_size);
	}

	void ContainerWidget::updateChildrenX() {
		horizontal ? updatePrimary() : updateSecondary();
	}

	void ContainerWidget::updateChildrenY() {
		horizontal ? updateSecondary() : updatePrimary();
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

	void ContainerWidget::updatePrimary() {
		// helper functions
		auto get_size_policy = [&](const Widget* widget) {
			return horizontal ? widget->getHorizontalSizePolicy() : widget->getVerticalSizePolicy();
		};
		auto get_size = [&](const Widget* widget) {
			return horizontal ? widget->getWidth() : widget->getHeight();
		};
		auto set_size = [&](Widget* widget, float size) {
			horizontal ? widget->setSize(size, widget->getHeight()) : widget->setSize(widget->getWidth(), size);
		};
		auto get_container_size = [&]() {
			return horizontal ? getWidth() : getHeight();
		};
		auto get_padding = [&]() {
			return horizontal ? horizontal_padding : vertical_padding;
		};
		// calculating size of expanding widgets
		float fixed_widgets_size = 0.0f;
		size_t expanding_count = 0;
		for (size_t i = 0; i < children.size(); i++) {
			if (get_size_policy(children[i]) == Widget::SizePolicy::EXPAND) {
				expanding_count++;
			} else {
				fixed_widgets_size += get_size(children[i]);
			}
		}
		float edge_padding = get_padding() * 2.0f;
		float between_padding = get_padding() * (children.size() - 1);
		float container_free_space = get_container_size() - edge_padding - between_padding;
		float expanding_free_space = container_free_space - fixed_widgets_size;
		float expanding_size = expanding_count != 0 ? expanding_free_space / expanding_count : 0.0f;
		expanding_size = std::max(0.0f, expanding_size);
		// expanding widgets
		for (size_t i = 0; i < children.size(); i++) {
			if (get_size_policy(children[i]) == Widget::SizePolicy::EXPAND) {
				set_size(children[i], expanding_size);
			}
		}
		// setting child positions
		children_bounds = sf::FloatRect();
		float next_pos = get_padding();
		for (size_t i = 0; i < children.size(); i++) {
			Widget* child = children[i];
			sf::Vector2f child_pos =
				horizontal
				? sf::Vector2f(next_pos, child->getPosition().y)
				: sf::Vector2f(child->getPosition().x, next_pos);
			child->setPosition(child_pos);
			next_pos += get_size(child) + get_padding();
			sf::FloatRect child_bounds = child->getParentLocalBounds();
			extend_bounds(children_bounds, child_bounds);
		}
	}

	void ContainerWidget::updateSecondary() {
		// helper functions
		auto get_other_size = [&](const Widget* widget) {
			return horizontal ? widget->getHeight() : widget->getWidth();
		};
		auto get_other_padding = [&]() {
			return horizontal ? vertical_padding : horizontal_padding;
		};
		auto get_alignment = [&]() {
			return horizontal ? vertical_alignment : horizontal_alignment;
		};
		// calculating max widget size
		float max_other_size = 0.0f;
		for (size_t i = 0; i < children.size(); i++) {
			if (get_other_size(children[i]) > max_other_size) {
				max_other_size = get_other_size(children[i]);
			}
		}
		// setting child positions
		children_bounds = sf::FloatRect();
		float other_pos = get_other_padding() + alignmentToOffset(get_alignment(), max_other_size);
		for (size_t i = 0; i < children.size(); i++) {
			Widget* child = children[i];
			sf::Vector2f child_pos =
				horizontal
				? sf::Vector2f(child->getPosition().x, other_pos)
				: sf::Vector2f(other_pos, child->getPosition().y);
			child->setPosition(child_pos);
			child->setOrigin(alignmentToAnchor(get_alignment()));
			sf::FloatRect child_bounds = child->getParentLocalBounds();
			extend_bounds(children_bounds, child_bounds);
		}
	}

}
