#include "widgets/widget_update_queue.h"
#include "widgets/widget.h"
#include "widgets/widget_list.h"
#include "widgets/container_widget.h"
#include "widgets/scroll_area_widget.h"

namespace fw {

	WidgetUpdateQueueEntry::WidgetUpdateQueueEntry(Widget* widget, WidgetUpdateType update_type) {
		this->widget = widget;
		this->update_type = update_type;
	}

	std::string WidgetUpdateQueueEntry::toStr() const {
		std::string type_str;
		switch (update_type) {
			case fw::WidgetUpdateType::NORMAL: type_str = "NORMAL"; break;
			case fw::WidgetUpdateType::POS_X: type_str = "POS_X"; break;
			case fw::WidgetUpdateType::POS_Y: type_str = "POS_Y"; break;
			case fw::WidgetUpdateType::SIZE_X: type_str = "SIZE_X"; break;
			case fw::WidgetUpdateType::SIZE_Y: type_str = "SIZE_Y"; break;
			case fw::WidgetUpdateType::CHILDREN_X: type_str = "CHILDREN_X"; break;
			case fw::WidgetUpdateType::CHILDREN_Y: type_str = "CHILDREN_Y"; break;
			default: wAssert("Unknown update type"); type_str = "<unknown>"; break;
		}
		return widget->getFullName() + " (" + type_str + ")";
	}

	WidgetUpdateQueue::WidgetUpdateQueue(WidgetList& widget_list) : widget_list(widget_list) { }

	void WidgetUpdateQueue::update() {
		std::vector<WidgetUpdateQueueEntry*> entries;
		std::function<void(Widget*)> add_widget = [&](Widget* widget) {
			if (!widget->isVisible()) {
				return;
			}
			entries.push_back(&widget->normal_entry);
			entries.push_back(&widget->pos_x_entry);
			entries.push_back(&widget->pos_y_entry);
			entries.push_back(&widget->size_x_entry);
			entries.push_back(&widget->size_y_entry);
			if (widget->getType() == Widget::WidgetType::Container) {
				entries.push_back(&widget->children_x_entry);
				entries.push_back(&widget->children_y_entry);
			}
			for (size_t i = 0; i < widget->getChildren().size(); i++) {
				add_widget(widget->getChild(i));
			}
		};
		add_widget(widget_list.getRootWidget());
		std::vector<std::vector<WidgetUpdateQueueEntry*>> loops;
		std::function<void(const std::vector<WidgetUpdateQueueEntry*>&)> on_loop_detected =
			[&](const std::vector<WidgetUpdateQueueEntry*>& loop) {
			loops.push_back(loop);
		};
		queue = toposort(entries, &WidgetUpdateQueue::getParents, &on_loop_detected);
		if (loops.size() > 0) {
			std::string msg = "Loops detected:\n";
			for (size_t i = 0; i < loops.size(); i++) {
				msg += "    Loop " + std::to_string(i) + ":\n";
				for (WidgetUpdateQueueEntry* entry : loops[i]) {
					msg += "        " + entry->toStr() + "\n";
				}
			}
			throw std::runtime_error(msg);
		}
	}

	const std::vector<std::vector<WidgetUpdateQueueEntry*>>& WidgetUpdateQueue::get() const {
		return queue;
	}

	std::vector<WidgetUpdateQueueEntry*> WidgetUpdateQueue::getParents(const WidgetUpdateQueueEntry* entry) {
		CompVector<WidgetUpdateQueueEntry*> result;
		if (entry->update_type == WidgetUpdateType::NORMAL) {
			// scroll area updates scrollbars depending on child's size
			if (ScrollAreaWidget* scroll_area = dynamic_cast<ScrollAreaWidget*>(entry->widget)) {
				result.add(&scroll_area->getWidget()->size_x_entry);
				result.add(&scroll_area->getWidget()->size_y_entry);
			}
		} else if (entry->update_type == WidgetUpdateType::POS_X) {
			// pos entry depends on parent's children x entry,
			// and so all entries dependent on pos entry will be updated
			// after parent's children x entry
			if (entry->widget->getParent() && entry->widget->getParent()->getType() == Widget::WidgetType::Container) {
				result.add(&entry->widget->getParent()->children_x_entry);
			}
			// some anchors need parent size
			WidgetUpdateQueueEntry* size_x_entry = &entry->widget->getParent()->size_x_entry;
			if (entry->widget->getParentAnchor() == Widget::Anchor::TOP_CENTER) {
				result.add(size_x_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::TOP_RIGHT) {
				result.add(size_x_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::CENTER) {
				result.add(size_x_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::CENTER_RIGHT) {
				result.add(size_x_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::BOTTOM_CENTER) {
				result.add(size_x_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::BOTTOM_RIGHT) {
				result.add(size_x_entry);
			}
		} else if (entry->update_type == WidgetUpdateType::POS_Y) {
			if (entry->widget->getParent() && entry->widget->getParent()->getType() == Widget::WidgetType::Container) {
				result.add(&entry->widget->getParent()->children_y_entry);
			}
			WidgetUpdateQueueEntry* size_y_entry = &entry->widget->getParent()->size_y_entry;
			if (entry->widget->getParentAnchor() == Widget::Anchor::CENTER_LEFT) {
				result.add(size_y_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::CENTER) {
				result.add(size_y_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::CENTER_RIGHT) {
				result.add(size_y_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::BOTTOM_LEFT) {
				result.add(size_y_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::BOTTOM_CENTER) {
				result.add(size_y_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::BOTTOM_RIGHT) {
				result.add(size_y_entry);
			}
		} else if (entry->update_type == WidgetUpdateType::SIZE_X) {
			if (entry->widget->getSizeXPolicy() == Widget::SizePolicy::PARENT) {
				// size might be changed in normal update,
				// so normal entries are also set as dependencies
				result.add(&entry->widget->getParent()->normal_entry);
				result.add(&entry->widget->getParent()->size_x_entry);
			} else if (entry->widget->getSizeXPolicy() == Widget::SizePolicy::CHILDREN) {
				// need to calculate children bounds before updating container's size
				result.add(&entry->widget->children_x_entry);
				// container's size depends on its children
				for (Widget* child : entry->widget->getChildren()) {
					// avoiding a loop
					if (child->getSizeXPolicy() != Widget::SizePolicy::EXPAND) {
						result.add(&child->normal_entry);
						result.add(&child->size_x_entry);
					}
				}
			} else if (entry->widget->getSizeXPolicy() == Widget::SizePolicy::EXPAND) {
				// expanding widget depends on the parent's size
				result.add(&entry->widget->getParent()->size_x_entry);
			}
		} else if (entry->update_type == WidgetUpdateType::SIZE_Y) {
			if (entry->widget->getSizeYPolicy() == Widget::SizePolicy::PARENT) {
				result.add(&entry->widget->getParent()->normal_entry);
				result.add(&entry->widget->getParent()->size_y_entry);
			} else if (entry->widget->getSizeYPolicy() == Widget::SizePolicy::CHILDREN) {
				result.add(&entry->widget->children_y_entry);
				for (Widget* child : entry->widget->getChildren()) {
					if (child->getSizeYPolicy() != Widget::SizePolicy::EXPAND) {
						result.add(&child->normal_entry);
						result.add(&child->size_y_entry);
					}
				}
			} else if (entry->widget->getSizeYPolicy() == Widget::SizePolicy::EXPAND) {
				result.add(&entry->widget->getParent()->size_y_entry);
			}
		} else if (entry->update_type == WidgetUpdateType::CHILDREN_X) {
			// If container's size does not depend on its children,
			// it should be calculated before size update.
			// The opposite case is in the SIZE_X and SIZE_Y part.
			if (entry->widget->getSizeXPolicy() != Widget::SizePolicy::CHILDREN) {
				result.add(&entry->widget->size_x_entry);
			}
			if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(entry->widget)) {
				for (Widget* child : entry->widget->getChildren()) {
					// child's position and size update needs to be overwritten by container update
					if (container->getHorizontal()) {
						// avoiding a loop
						if (child->getSizeXPolicy() != Widget::SizePolicy::EXPAND) {
							result.add(&child->size_x_entry);
						}
					}
				}
			} else {
				wAssert("Non-containers should not have CHILDREN_X update entry");
			}
		} else if (entry->update_type == WidgetUpdateType::CHILDREN_Y) {
			if (entry->widget->getSizeYPolicy() != Widget::SizePolicy::CHILDREN) {
				result.add(&entry->widget->size_y_entry);
			}
			if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(entry->widget)) {
				for (Widget* child : entry->widget->getChildren()) {
					if (!container->getHorizontal()) {
						if (child->getSizeXPolicy() != Widget::SizePolicy::EXPAND) {
							result.add(&child->size_y_entry);
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
