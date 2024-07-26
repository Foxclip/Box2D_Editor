#include "widgets/widget_update_queue.h"
#include "widgets/widget.h"
#include "widgets/widget_list.h"
#include "widgets/container_widget.h"

namespace fw {

	WidgetUpdateQueue::WidgetUpdateQueue(WidgetList& widget_list) : widget_list(widget_list) { }

	void WidgetUpdateQueue::update() {
		std::vector<WidgetUpdateTarget*> entries;
		std::function<void(Widget*)> add_widget = [&](Widget* widget) {
			if (!widget->isVisible()) {
				return;
			}
			entries.push_back(&widget->pos_x_target);
			entries.push_back(&widget->pos_y_target);
			entries.push_back(&widget->size_x_target);
			entries.push_back(&widget->size_y_target);
			if (widget->isContainer()) {
				entries.push_back(&widget->children_x_target);
				entries.push_back(&widget->children_y_target);
			}
			for (size_t i = 0; i < widget->getLinks().size(); i++) {
				entries.push_back(widget->getLinks()[i]);
			}
			for (size_t i = 0; i < widget->getChildren().size(); i++) {
				add_widget(widget->getChild(i));
			}
		};
		add_widget(widget_list.getRootWidget());
		std::vector<std::vector<WidgetUpdateTarget*>> loops;
		std::function<void(const std::vector<WidgetUpdateTarget*>&)> on_loop_detected =
			[&](const std::vector<WidgetUpdateTarget*>& loop) {
			loops.push_back(loop);
		};
		queue = toposort(entries, &WidgetUpdateQueue::getParents, &on_loop_detected);
		if (loops.size() > 0) {
			std::string msg = std::to_string(loops.size()) + " loops detected:\n";
			for (size_t i = 0; i < loops.size(); i++) {
				msg += "    Loop " + std::to_string(i) + ":\n";
				for (WidgetUpdateTarget* entry : loops[i]) {
					msg += "        " + entry->toStr() + "\n";
				}
			}
			throw std::runtime_error(msg);
		}
	}

	const std::vector<std::vector<WidgetUpdateTarget*>>& WidgetUpdateQueue::get() const {
		return queue;
	}

	std::vector<WidgetUpdateTarget*> WidgetUpdateQueue::getParents(const WidgetUpdateTarget* target) {
		CompVector<WidgetUpdateTarget*> result;
		if (const WidgetLink* link = dynamic_cast<const WidgetLink*>(target)) {
			return link->getTargets();
		} else if (const WidgetUpdateSocket* socket = dynamic_cast<const WidgetUpdateSocket*>(target)) {
			CompVector socket_targets = socket->getTargets();
			result.insert(result.end(), socket_targets.begin(), socket_targets.end());
			if (socket->getType() == WidgetUpdateType::POS_X) {
				// pos entry depends on parent's children x entry,
				// and so all entries dependent on pos entry will be updated
				// after parent's children x entry
				if (socket->getWidget()->getParent() && socket->getWidget()->getParent()->isContainer()) {
					result.add(&socket->getWidget()->getParent()->children_x_target);
				}
				// some anchors need parent size
				WidgetUpdateTarget* size_x_entry = &socket->getWidget()->getParent()->size_x_target;
				if (socket->getWidget()->getParentAnchor() == Widget::Anchor::TOP_CENTER) {
					result.add(size_x_entry);
				} else if (socket->getWidget()->getParentAnchor() == Widget::Anchor::TOP_RIGHT) {
					result.add(size_x_entry);
				} else if (socket->getWidget()->getParentAnchor() == Widget::Anchor::CENTER) {
					result.add(size_x_entry);
				} else if (socket->getWidget()->getParentAnchor() == Widget::Anchor::CENTER_RIGHT) {
					result.add(size_x_entry);
				} else if (socket->getWidget()->getParentAnchor() == Widget::Anchor::BOTTOM_CENTER) {
					result.add(size_x_entry);
				} else if (socket->getWidget()->getParentAnchor() == Widget::Anchor::BOTTOM_RIGHT) {
					result.add(size_x_entry);
				}
			} else if (socket->getType() == WidgetUpdateType::POS_Y) {
				if (socket->getWidget()->getParent() && socket->getWidget()->getParent()->isContainer()) {
					result.add(&socket->getWidget()->getParent()->children_y_target);
				}
				WidgetUpdateTarget* size_y_entry = &socket->getWidget()->getParent()->size_y_target;
				if (socket->getWidget()->getParentAnchor() == Widget::Anchor::CENTER_LEFT) {
					result.add(size_y_entry);
				} else if (socket->getWidget()->getParentAnchor() == Widget::Anchor::CENTER) {
					result.add(size_y_entry);
				} else if (socket->getWidget()->getParentAnchor() == Widget::Anchor::CENTER_RIGHT) {
					result.add(size_y_entry);
				} else if (socket->getWidget()->getParentAnchor() == Widget::Anchor::BOTTOM_LEFT) {
					result.add(size_y_entry);
				} else if (socket->getWidget()->getParentAnchor() == Widget::Anchor::BOTTOM_CENTER) {
					result.add(size_y_entry);
				} else if (socket->getWidget()->getParentAnchor() == Widget::Anchor::BOTTOM_RIGHT) {
					result.add(size_y_entry);
				}
			} else if (socket->getType() == WidgetUpdateType::SIZE_X) {
				if (socket->getWidget()->getSizeXPolicy() == Widget::SizePolicy::PARENT) {
					result.add(&socket->getWidget()->getParent()->size_x_target);
				} else if (socket->getWidget()->getSizeXPolicy() == Widget::SizePolicy::CHILDREN) {
					// need to calculate children bounds before updating container's size
					result.add(&socket->getWidget()->children_x_target);
					// container's size depends on its children
					for (Widget* child : socket->getWidget()->getChildren()) {
						// avoiding a loop
						if (child->getSizeXPolicy() != Widget::SizePolicy::EXPAND) {
							result.add(&child->size_x_target);
						}
					}
				} else if (socket->getWidget()->getSizeXPolicy() == Widget::SizePolicy::EXPAND) {
					// expanding widget depends on the parent's size
					result.add(&socket->getWidget()->getParent()->size_x_target);
				}
			} else if (socket->getType() == WidgetUpdateType::SIZE_Y) {
				if (socket->getWidget()->getSizeYPolicy() == Widget::SizePolicy::PARENT) {
					result.add(&socket->getWidget()->getParent()->size_y_target);
				} else if (socket->getWidget()->getSizeYPolicy() == Widget::SizePolicy::CHILDREN) {
					result.add(&socket->getWidget()->children_y_target);
					for (Widget* child : socket->getWidget()->getChildren()) {
						if (child->getSizeYPolicy() != Widget::SizePolicy::EXPAND) {
							result.add(&child->size_y_target);
						}
					}
				} else if (socket->getWidget()->getSizeYPolicy() == Widget::SizePolicy::EXPAND) {
					result.add(&socket->getWidget()->getParent()->size_y_target);
				}
			} else if (socket->getType() == WidgetUpdateType::CHILDREN_X) {
				// If container's size does not depend on its children,
				// it should be calculated before size update.
				// The opposite case is in the SIZE_X and SIZE_Y part.
				if (socket->getWidget()->getSizeXPolicy() != Widget::SizePolicy::CHILDREN) {
					result.add(&socket->getWidget()->size_x_target);
				}
				if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(socket->getWidget())) {
					for (Widget* child : socket->getWidget()->getChildren()) {
						// child's position and size update needs to be overwritten by container update
						// avoiding a loop
						if (child->getSizeXPolicy() != Widget::SizePolicy::EXPAND) {
							result.add(&child->size_x_target);
						}
					}
				} else {
					wAssert("Non-containers should not have CHILDREN_X update entry");
				}
			} else if (socket->getType() == WidgetUpdateType::CHILDREN_Y) {
				if (socket->getWidget()->getSizeYPolicy() != Widget::SizePolicy::CHILDREN) {
					result.add(&socket->getWidget()->size_y_target);
				}
				if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(socket->getWidget())) {
					for (Widget* child : socket->getWidget()->getChildren()) {
						if (child->getSizeYPolicy() != Widget::SizePolicy::EXPAND) {
							result.add(&child->size_y_target);
						}
					}
				} else {
					wAssert("Non-containers should not have CHILDREN_Y update entry");
				}
			}
		} else {
			wAssert(false, "Unknown target type");
		}
		return result;
	}

}
