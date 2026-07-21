#include "widgets/widget_parent_chain.h"
#include "widgets/widget.h"

namespace fw {

	WidgetParentChain::WidgetParentChain(Widget& widget) : widget(widget) { }

	const CompVector<Widget*>& WidgetParentChain::get() const {
		if (!valid) {
			recalc();
		}
		return chain;
	}

	void WidgetParentChain::invalidate() {
		valid = false;
		for (size_t i = 0; i < widget.getChildrenCount(); i++) {
			Widget* child = widget.getChild(i);
			child->parent_chain.invalidate();
		}
	}

	void WidgetParentChain::recalc() const {
		chain.clear();
		const Widget* cur_obj = &widget;
		while (cur_obj) {
			if (cur_obj->parent) {
				chain.add(cur_obj->parent);
				cur_obj = cur_obj->parent;
			} else {
				break;
			}
		}
		valid = true;
	}

}
