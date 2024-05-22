#include "widgets/widget_update_queue.h"
#include "widgets/widget.h"
#include "widgets/widget_list.h"
#include "widgets/container_widget.h"

namespace fw {

	WidgetUpdateQueueEntry::WidgetUpdateQueueEntry(Widget* widget, WidgetUpdateType update_type) {
		this->widget = widget;
		this->update_type = update_type;
	}

	bool WidgetUpdateQueueEntry::operator<(const WidgetUpdateQueueEntry& other) const {
		if (widget < other.widget) {
			return true;
		} else if (widget > other.widget) {
			return false;
		} else {
			return update_type < other.update_type;
		}
	}

	WidgetUpdateQueue::WidgetUpdateQueue(WidgetList& widget_list) : widget_list(widget_list) { }

	void WidgetUpdateQueue::update() {
		std::vector<WidgetUpdateQueueEntry> entries;
		std::function<void(Widget*)> add_widget = [&](Widget* widget) {
			if (!widget->isVisible()) {
				return;
			}
			entries.push_back(widget->normal_entry);
			entries.push_back(widget->size_horizontal_entry);
			entries.push_back(widget->size_vertical_entry);
			for (size_t i = 0; i < widget->getChildren().size(); i++) {
				add_widget(widget->getChild(i));
			}
		};
		add_widget(widget_list.getRootWidget());
		queue = toposort(entries, &WidgetUpdateQueue::getParents);
	}

	const std::vector<std::vector<WidgetUpdateQueueEntry>>& WidgetUpdateQueue::get() const {
		return queue;
	}

	std::vector<WidgetUpdateQueueEntry> WidgetUpdateQueue::getParents(const WidgetUpdateQueueEntry& entry) {
		std::vector<WidgetUpdateQueueEntry> result;
		if (entry.update_type == WidgetUpdateType::NORMAL) {
			if (ContainerWidget* container = dynamic_cast<ContainerWidget*>(entry.widget)) {
				for (Widget* child : entry.widget->getChildren()) {
					result.push_back(child->normal_entry);
					if (container->getHorizontal()) {
						result.push_back(child->size_horizontal_entry);
					} else {
						result.push_back(child->size_vertical_entry);
					}
				}
			}
		} else if (entry.update_type == WidgetUpdateType::SIZE_HORIZONTAL) {
			// size might be changed in normal update,
			// so normal entries are also set as dependencies
			if (entry.widget->getHorizontalSizePolicy() == Widget::SizePolicy::PARENT) {
				result.push_back(entry.widget->getParent()->normal_entry);
				result.push_back(entry.widget->getParent()->size_horizontal_entry);
			} else if (entry.widget->getHorizontalSizePolicy() == Widget::SizePolicy::CHILDREN) {
				for (Widget* child : entry.widget->getChildren()) {
					result.push_back(child->normal_entry);
					result.push_back(child->size_horizontal_entry);
				}
			}
		} else if (entry.update_type == WidgetUpdateType::SIZE_VERTICAL) {
			if (entry.widget->getVerticalSizePolicy() == Widget::SizePolicy::PARENT) {
				result.push_back(entry.widget->getParent()->normal_entry);
				result.push_back(entry.widget->getParent()->size_vertical_entry);
			} else if (entry.widget->getVerticalSizePolicy() == Widget::SizePolicy::CHILDREN) {
				for (Widget* child : entry.widget->getChildren()) {
					result.push_back(child->normal_entry);
					result.push_back(child->size_vertical_entry);
				}
			}
		}
		return result;
	}

}
