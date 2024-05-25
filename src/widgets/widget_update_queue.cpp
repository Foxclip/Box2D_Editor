#include "widgets/widget_update_queue.h"
#include "widgets/widget.h"
#include "widgets/widget_list.h"
#include "widgets/container_widget.h"

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
			if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(entry->widget)) {
				for (Widget* child : entry->widget->getChildren()) {
					result.add(&child->normal_entry);
					if (container->getHorizontal()) {
						// child's position update is overwritten by container update
						result.add(&child->pos_x_entry);
						result.add(&child->size_x_entry);
					} else {
						result.add(&child->pos_y_entry);
						result.add(&child->size_y_entry);
					}
				}
			}
		} else if (entry->update_type == WidgetUpdateType::POS_X) {
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
			// size might be changed in normal update,
			// so normal entries are also set as dependencies
			if (entry->widget->getHorizontalSizePolicy() == Widget::SizePolicy::PARENT) {
				result.add(&entry->widget->getParent()->normal_entry);
				result.add(&entry->widget->getParent()->size_x_entry);
			} else if (entry->widget->getHorizontalSizePolicy() == Widget::SizePolicy::CHILDREN) {
				result.add(&entry->widget->normal_entry);
				for (Widget* child : entry->widget->getChildren()) {
					result.add(&child->normal_entry);
					result.add(&child->size_x_entry);
				}
			} else if (entry->widget->getHorizontalSizePolicy() == Widget::SizePolicy::EXPAND) {
				result.add(&entry->widget->getParent()->size_x_entry);
			}
		} else if (entry->update_type == WidgetUpdateType::SIZE_Y) {
			if (entry->widget->getVerticalSizePolicy() == Widget::SizePolicy::PARENT) {
				result.add(&entry->widget->getParent()->normal_entry);
				result.add(&entry->widget->getParent()->size_y_entry);
			} else if (entry->widget->getVerticalSizePolicy() == Widget::SizePolicy::CHILDREN) {
				result.add(&entry->widget->normal_entry);
				for (Widget* child : entry->widget->getChildren()) {
					result.add(&child->normal_entry);
					result.add(&child->size_y_entry);
				}
			} else if (entry->widget->getVerticalSizePolicy() == Widget::SizePolicy::EXPAND) {
				result.add(&entry->widget->getParent()->size_y_entry);
			}
		}
		return result;
	}

}
