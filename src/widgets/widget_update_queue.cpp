#include "widgets/widget_update_queue.h"
#include "widgets/widget.h"
#include "widgets/widget_list.h"
#include "widgets/container_widget.h"
#include "widgets/scroll_area_widget.h"

namespace fw {

	WidgetUpdateQueue::WidgetUpdateQueue(WidgetList& widget_list) : widget_list(widget_list) { }

	void WidgetUpdateQueue::update() {
		std::vector<WidgetUpdateTarget*> entries;
		std::function<void(Widget*)> add_widget = [&](Widget* widget) {
			if (!widget->isVisible()) {
				return;
			}
			entries.push_back(&widget->normal_target);
			entries.push_back(&widget->pos_x_target);
			entries.push_back(&widget->pos_y_target);
			entries.push_back(&widget->size_x_target);
			entries.push_back(&widget->size_y_target);
			if (widget->getType() == Widget::WidgetType::Container) {
				entries.push_back(&widget->children_x_target);
				entries.push_back(&widget->children_y_target);
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
			std::string msg = "Loops detected:\n";
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

	std::vector<WidgetUpdateTarget*> WidgetUpdateQueue::getParents(const WidgetUpdateTarget* entry) {
		CompVector<WidgetUpdateTarget*> result;
		if (entry->getType() == WidgetUpdateType::NORMAL) {
			// scroll area updates scrollbars depending on child's size
			if (ScrollAreaWidget* scroll_area = dynamic_cast<ScrollAreaWidget*>(entry->getWidget())) {
				result.add(&scroll_area->getWidget()->size_x_target);
				result.add(&scroll_area->getWidget()->size_y_target);
			}
		} else if (entry->getType() == WidgetUpdateType::POS_X) {
			// pos entry depends on parent's children x entry,
			// and so all entries dependent on pos entry will be updated
			// after parent's children x entry
			if (entry->getWidget()->getParent() && entry->getWidget()->getParent()->getType() == Widget::WidgetType::Container) {
				result.add(&entry->getWidget()->getParent()->children_x_target);
			}
			// some anchors need parent size
			WidgetUpdateTarget* size_x_entry = &entry->getWidget()->getParent()->size_x_target;
			if (entry->getWidget()->getParentAnchor() == Widget::Anchor::TOP_CENTER) {
				result.add(size_x_entry);
			} else if (entry->getWidget()->getParentAnchor() == Widget::Anchor::TOP_RIGHT) {
				result.add(size_x_entry);
			} else if (entry->getWidget()->getParentAnchor() == Widget::Anchor::CENTER) {
				result.add(size_x_entry);
			} else if (entry->getWidget()->getParentAnchor() == Widget::Anchor::CENTER_RIGHT) {
				result.add(size_x_entry);
			} else if (entry->getWidget()->getParentAnchor() == Widget::Anchor::BOTTOM_CENTER) {
				result.add(size_x_entry);
			} else if (entry->getWidget()->getParentAnchor() == Widget::Anchor::BOTTOM_RIGHT) {
				result.add(size_x_entry);
			}
		} else if (entry->getType() == WidgetUpdateType::POS_Y) {
			if (entry->getWidget()->getParent() && entry->getWidget()->getParent()->getType() == Widget::WidgetType::Container) {
				result.add(&entry->getWidget()->getParent()->children_y_target);
			}
			WidgetUpdateTarget* size_y_entry = &entry->getWidget()->getParent()->size_y_target;
			if (entry->getWidget()->getParentAnchor() == Widget::Anchor::CENTER_LEFT) {
				result.add(size_y_entry);
			} else if (entry->getWidget()->getParentAnchor() == Widget::Anchor::CENTER) {
				result.add(size_y_entry);
			} else if (entry->getWidget()->getParentAnchor() == Widget::Anchor::CENTER_RIGHT) {
				result.add(size_y_entry);
			} else if (entry->getWidget()->getParentAnchor() == Widget::Anchor::BOTTOM_LEFT) {
				result.add(size_y_entry);
			} else if (entry->getWidget()->getParentAnchor() == Widget::Anchor::BOTTOM_CENTER) {
				result.add(size_y_entry);
			} else if (entry->getWidget()->getParentAnchor() == Widget::Anchor::BOTTOM_RIGHT) {
				result.add(size_y_entry);
			}
		} else if (entry->getType() == WidgetUpdateType::SIZE_X) {
			if (entry->getWidget()->getSizeXPolicy() == Widget::SizePolicy::PARENT) {
				// size might be changed in normal update,
				// so normal entries are also set as dependencies
				result.add(&entry->getWidget()->getParent()->normal_target);
				result.add(&entry->getWidget()->getParent()->size_x_target);
			} else if (entry->getWidget()->getSizeXPolicy() == Widget::SizePolicy::CHILDREN) {
				// need to calculate children bounds before updating container's size
				result.add(&entry->getWidget()->children_x_target);
				// container's size depends on its children
				for (Widget* child : entry->getWidget()->getChildren()) {
					// avoiding a loop
					if (child->getSizeXPolicy() != Widget::SizePolicy::EXPAND) {
						result.add(&child->normal_target);
						result.add(&child->size_x_target);
					}
				}
			} else if (entry->getWidget()->getSizeXPolicy() == Widget::SizePolicy::EXPAND) {
				// expanding widget depends on the parent's size
				result.add(&entry->getWidget()->getParent()->size_x_target);
			}
		} else if (entry->getType() == WidgetUpdateType::SIZE_Y) {
			if (entry->getWidget()->getSizeYPolicy() == Widget::SizePolicy::PARENT) {
				result.add(&entry->getWidget()->getParent()->normal_target);
				result.add(&entry->getWidget()->getParent()->size_y_target);
			} else if (entry->getWidget()->getSizeYPolicy() == Widget::SizePolicy::CHILDREN) {
				result.add(&entry->getWidget()->children_y_target);
				for (Widget* child : entry->getWidget()->getChildren()) {
					if (child->getSizeYPolicy() != Widget::SizePolicy::EXPAND) {
						result.add(&child->normal_target);
						result.add(&child->size_y_target);
					}
				}
			} else if (entry->getWidget()->getSizeYPolicy() == Widget::SizePolicy::EXPAND) {
				result.add(&entry->getWidget()->getParent()->size_y_target);
			}
		} else if (entry->getType() == WidgetUpdateType::CHILDREN_X) {
			// If container's size does not depend on its children,
			// it should be calculated before size update.
			// The opposite case is in the SIZE_X and SIZE_Y part.
			if (entry->getWidget()->getSizeXPolicy() != Widget::SizePolicy::CHILDREN) {
				result.add(&entry->getWidget()->size_x_target);
			}
			if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(entry->getWidget())) {
				for (Widget* child : entry->getWidget()->getChildren()) {
					// child's position and size update needs to be overwritten by container update
					if (container->getHorizontal()) {
						// avoiding a loop
						if (child->getSizeXPolicy() != Widget::SizePolicy::EXPAND) {
							result.add(&child->size_x_target);
						}
					}
				}
			} else {
				wAssert("Non-containers should not have CHILDREN_X update entry");
			}
		} else if (entry->getType() == WidgetUpdateType::CHILDREN_Y) {
			if (entry->getWidget()->getSizeYPolicy() != Widget::SizePolicy::CHILDREN) {
				result.add(&entry->getWidget()->size_y_target);
			}
			if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(entry->getWidget())) {
				for (Widget* child : entry->getWidget()->getChildren()) {
					if (!container->getHorizontal()) {
						if (child->getSizeXPolicy() != Widget::SizePolicy::EXPAND) {
							result.add(&child->size_y_target);
						}
					}
				}
			} else {
				wAssert("Non-containers should not have CHILDREN_Y update entry");
			}
		}
		return result;
	}

}
