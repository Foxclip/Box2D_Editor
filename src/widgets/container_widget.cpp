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

	float ContainerWidget::getInnerPaddingX() const {
		return inner_padding_x;
	}

	float ContainerWidget::getInnerPaddingY() const {
		return inner_padding_y;
	}

	float ContainerWidget::getTopPadding() const {
		return top_padding;
	}

	float ContainerWidget::getBottomPadding() const {
		return bottom_padding;
	}

	float ContainerWidget::getLeftPadding() const {
		return left_padding;
	}

	float ContainerWidget::getRightPadding() const {
		return right_padding;
	}

	Widget::Alignment ContainerWidget::getAlignmentY() const {
		return alignment_y;
	}

	Widget::Alignment ContainerWidget::getAlignmentX() const {
		return alignment_x;
	}

	void ContainerWidget::setHorizontal(bool value) {
		this->horizontal = value;
	}

	void ContainerWidget::setPadding(float padding) {
		this->inner_padding_x = padding;
		this->inner_padding_y = padding;
		this->top_padding = padding;
		this->bottom_padding = padding;
		this->left_padding = padding;
		this->right_padding = padding;
	}

	void ContainerWidget::setPaddingX(float padding) {
		this->inner_padding_x = padding;
		this->left_padding = padding;
		this->right_padding = padding;
	}

	void ContainerWidget::setPaddingY(float padding) {
		this->inner_padding_y = padding;
		this->top_padding = padding;
		this->bottom_padding = padding;
	}

	void ContainerWidget::setInnerPaddingX(float padding) {
		this->inner_padding_x = padding;
	}

	void ContainerWidget::setInnerPaddingY(float padding) {
		this->inner_padding_y = padding;
	}

	void ContainerWidget::setTopPadding(float padding) {
		this->top_padding = padding;
	}

	void ContainerWidget::setBottomPadding(float padding) {
		this->bottom_padding = padding;
	}

	void ContainerWidget::setLeftPadding(float padding) {
		this->left_padding = padding;
	}

	void ContainerWidget::setRightPadding(float padding) {
		this->right_padding = padding;
	}

	void ContainerWidget::setAlignmentX(Alignment alignment) {
		this->alignment_y = alignment;
	}

	void ContainerWidget::setAlignmentY(Alignment alignment) {
		this->alignment_x = alignment;
	}

	void ContainerWidget::updateSizeX() {
		sf::Vector2f new_pos = getPosition();
		sf::Vector2f new_size = getSize();
		if (size_policy_x == SizePolicy::CHILDREN) {
			new_size.x = children_bounds.width + right_padding;
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
		if (size_policy_y == SizePolicy::CHILDREN) {
			new_size.y = children_bounds.height + bottom_padding;
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
			switch (alignment_y) {
				case Alignment::ALIGN_TOP: return 0.0f;
				case Alignment::ALIGN_CENTER: return max_size / 2.0f;
				case Alignment::ALIGN_BOTTOM: return max_size;
				default: return 0.0f;
			}
		} else {
			switch (alignment_x) {
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
			return horizontal ? widget->getSizeXPolicy() : widget->getSizeYPolicy();
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
		auto get_min_size = [&](const Widget* widget) {
			return horizontal ? widget->getMinSize().x : widget->getMinSize().y;
		};
		auto get_max_size = [&](const Widget* widget) {
			return horizontal ? widget->getMaxSize().x : widget->getMaxSize().y;
		};
		auto get_inner_padding = [&]() {
			return horizontal ? inner_padding_x : inner_padding_y;
		};
		auto get_outer_padding_1 = [&]() {
			return horizontal ? left_padding : top_padding;
		};
		auto get_outer_padding_2 = [&]() {
			return horizontal ? right_padding : bottom_padding;
		};
		// calculating fixed size
		float fixed_size = 0.0f;
		auto cmp_max_size = [&](const Widget* left, const Widget* right) {
			if (get_max_size(left) > 0.0f && get_max_size(right) < 0.0f) {
				return true;
			} else if (get_max_size(left) < 0.0f && get_max_size(right) > 0.0f) {
				return false;
			} else if (get_max_size(left) > 0.0f && get_max_size(right) > 0.0f) {
				return get_max_size(left) < get_max_size(right);
			} else {
				return left < right;
			}
		};
		std::vector<Widget*> expanding_widgets;
		size_t visible_count = 0;
		for (size_t i = 0; i < children.size(); i++) {
			Widget* child = children[i];
			if (!child->isVisible()) {
				continue;
			}
			visible_count++;
			if (get_size_policy(child) == Widget::SizePolicy::EXPAND) {
				wAssert(child->getMinSize().x <= child->getMaxSize().x || child->getMaxSize().x < 0.0f);
				wAssert(child->getMinSize().y <= child->getMaxSize().y || child->getMaxSize().y < 0.0f);
				fixed_size += get_min_size(child);
				expanding_widgets.push_back(child);
			} else {
				fixed_size += get_size(child);
			}
		}
		float outer_padding = get_outer_padding_1() + get_outer_padding_2();
		float inner_padding = get_inner_padding() * (visible_count - 1);
		float container_free_space = get_container_size() - outer_padding - inner_padding;
		float expanding_free_space = container_free_space - fixed_size;
		// setting widgets to their minimum size
		for (size_t i = 0; i < expanding_widgets.size(); i++) {
			Widget* widget = expanding_widgets[i];
			set_size(widget, get_min_size(widget));
		}
		std::sort(expanding_widgets.begin(), expanding_widgets.end(), cmp_max_size);
		float remaining_free_space = std::max(0.0f, expanding_free_space);
		size_t last_i = 0;
		// widgets hitting max_size ceiling take only what they need
		for (size_t i = 0; i < expanding_widgets.size(); i++) {
			Widget* widget = expanding_widgets[i];
			float max_size = get_max_size(widget);
			if (max_size < 0.0f) {
				last_i = i;
				break;
			}
			size_t widgets_remaining = expanding_widgets.size() - i;
			float exp_delta = remaining_free_space / widgets_remaining;
			float size_after_exp = get_size(widget) + exp_delta;
			if (max_size < size_after_exp) {
				set_size(widget, max_size);
				remaining_free_space -= std::max(0.0f, max_size);
			} else {
				last_i = i;
				break;
			}
		}
		// free space is distributed evenly among remaining widgets
		size_t widgets_remaining = expanding_widgets.size() - last_i;
		float exp_delta = remaining_free_space / widgets_remaining;
		for (size_t i = last_i; i < expanding_widgets.size(); i++) {
			Widget* widget = expanding_widgets[i];
			size_t widgets_remaining = expanding_widgets.size() - i;
			float size_after_exp = get_size(widget) + exp_delta;
			set_size(widget, size_after_exp);
		}
		// setting child positions
		children_bounds = sf::FloatRect();
		float next_pos = get_outer_padding_1();
		for (size_t i = 0; i < children.size(); i++) {
			Widget* child = children[i];
			if (!child->isVisible()) {
				continue;
			}
			sf::Vector2f child_pos =
				horizontal
				? sf::Vector2f(next_pos, child->getPosition().y)
				: sf::Vector2f(child->getPosition().x, next_pos);
			child->setPosition(child_pos);
			next_pos += get_size(child) + get_inner_padding();
			sf::FloatRect child_bounds = child->getParentLocalBounds();
			extend_bounds(children_bounds, child_bounds);
		}
	}

	void ContainerWidget::updateSecondary() {
		// helper functions
		auto get_secondary_size = [&](const Widget* widget) {
			return horizontal ? widget->getHeight() : widget->getWidth();
		};
		auto get_secondary_outer_padding_1 = [&]() {
			return horizontal ? top_padding : left_padding;
		};
		auto get_secondary_outer_padding_2 = [&]() {
			return horizontal ? bottom_padding : right_padding;
		};
		auto get_alignment = [&]() {
			return horizontal ? alignment_y : alignment_x;
		};
		// calculating max widget size
		float max_secondary_size = 0.0f;
		for (size_t i = 0; i < children.size(); i++) {
			if (get_secondary_size(children[i]) > max_secondary_size) {
				max_secondary_size = get_secondary_size(children[i]);
			}
		}
		// setting child positions
		children_bounds = sf::FloatRect();
		float secondary_pos = get_secondary_outer_padding_1() + alignmentToOffset(get_alignment(), max_secondary_size);
		for (size_t i = 0; i < children.size(); i++) {
			Widget* child = children[i];
			sf::Vector2f child_pos =
				horizontal
				? sf::Vector2f(child->getPosition().x, secondary_pos)
				: sf::Vector2f(secondary_pos, child->getPosition().y);
			child->setPosition(child_pos);
			child->setOrigin(alignmentToAnchor(get_alignment()));
			sf::FloatRect child_bounds = child->getParentLocalBounds();
			extend_bounds(children_bounds, child_bounds);
		}
	}

}
