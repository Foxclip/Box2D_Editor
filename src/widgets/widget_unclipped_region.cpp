#include "widgets/widget_unclipped_region.h"
#include "widgets/widget.h"

WidgetUnclippedRegion::WidgetUnclippedRegion(Widget* widget) {
	this->widget = widget;
}

const sf::FloatRect& WidgetUnclippedRegion::get() const {
	if (!valid) {
		recalc();
	}
	return unclippedRegion;
}

const sf::FloatRect& WidgetUnclippedRegion::getQuantized() const {
	if (!valid) {
		recalc();
	}
	return quantizedUnclippedRegion;
}

bool WidgetUnclippedRegion::isNonZero() const {
	if (!valid) {
		recalc();
	}
	return unclippedRegion.width > 0 && unclippedRegion.height > 0;
}

bool WidgetUnclippedRegion::isQuantizedNonZero() const {
	if (!valid) {
		recalc();
	}
	return quantizedUnclippedRegion.width > 0 && quantizedUnclippedRegion.height > 0;
}

void WidgetUnclippedRegion::recalc() const {
	CompoundVector<Widget*> parents = widget->getParentChain();
	parents.reverse();
	sf::FloatRect result = widget->getVisualGlobalBounds();
	Widget* parent = widget->parent;
	while (parent) {
		if (parent->getClipChildren()) {
			sf::FloatRect parent_unclipped_region = parent->getUnclippedRegion();
			sf::FloatRect intersection;
			bool intersects = result.intersects(parent_unclipped_region, intersection);
			if (intersects) {
				result = intersection;
			} else {
				result = sf::FloatRect(widget->getGlobalPosition(), sf::Vector2f());
			}
		}
		parent = parent->parent;
	}
	unclippedRegion = result;
	quantizedUnclippedRegion = utils::quantize_rect(result, utils::QUANTIZE_MODE_FLOOR);
	valid = true;
}

void WidgetUnclippedRegion::invalidate() {
	valid = false;
	for (size_t i = 0; i < widget->children.size(); i++) {
		widget->children[i]->unclipped_region.invalidate();
	}
}
