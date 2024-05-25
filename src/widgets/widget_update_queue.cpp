#include "widgets/widget_update_queue.h"
#include "widgets/widget.h"
#include "widgets/widget_list.h"
#include "widgets/container_widget.h"

namespace fw {

	WidgetUpdateQueueEntry::WidgetUpdateQueueEntry(Widget* widget, WidgetUpdateType update_type) {
		this->widget = widget;
		this->update_type = update_type;
	}

	WidgetUpdateQueue::WidgetUpdateQueue(WidgetList& widget_list) : widget_list(widget_list) { }

	void WidgetUpdateQueue::update() {
		std::vector<WidgetUpdateQueueEntry*> entries;
		std::function<void(Widget*)> add_widget = [&](Widget* widget) {
			if (!widget->isVisible()) {
				return;
			}
			entries.push_back(&widget->normal_entry);
			entries.push_back(&widget->position_entry);
			entries.push_back(&widget->size_horizontal_entry);
			entries.push_back(&widget->size_vertical_entry);
			for (size_t i = 0; i < widget->getChildren().size(); i++) {
				add_widget(widget->getChild(i));
			}
		};
		add_widget(widget_list.getRootWidget());
		queue = toposort(entries, &WidgetUpdateQueue::getParents);
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
					// child's position update is overwritten by container update
					result.add(&child->position_entry);
					if (container->getHorizontal()) {
						result.add(&child->size_horizontal_entry);
					} else {
						result.add(&child->size_vertical_entry);
					}
				}
			}
		} else if (entry->update_type == WidgetUpdateType::POSITION) {
			WidgetUpdateQueueEntry* horizontal_entry = &entry->widget->getParent()->size_horizontal_entry;
			WidgetUpdateQueueEntry* vertical_entry = &entry->widget->getParent()->size_vertical_entry;
			if (entry->widget->getParentAnchor() == Widget::Anchor::TOP_CENTER) {
				result.add(horizontal_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::TOP_RIGHT) {
				result.add(horizontal_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::CENTER_LEFT) {
				result.add(vertical_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::CENTER) {
				result.add(horizontal_entry);
				result.add(vertical_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::CENTER_RIGHT) {
				result.add(horizontal_entry);
				result.add(vertical_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::BOTTOM_LEFT) {
				result.add(vertical_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::BOTTOM_CENTER) {
				result.add(horizontal_entry);
				result.add(vertical_entry);
			} else if (entry->widget->getParentAnchor() == Widget::Anchor::BOTTOM_RIGHT) {
				result.add(horizontal_entry);
				result.add(vertical_entry);
			}
		} else if (entry->update_type == WidgetUpdateType::SIZE_HORIZONTAL) {
			// size might be changed in normal update,
			// so normal entries are also set as dependencies
			if (entry->widget->getHorizontalSizePolicy() == Widget::SizePolicy::PARENT) {
				result.add(&entry->widget->getParent()->normal_entry);
				result.add(&entry->widget->getParent()->size_horizontal_entry);
			} else if (entry->widget->getHorizontalSizePolicy() == Widget::SizePolicy::CHILDREN) {
				for (Widget* child : entry->widget->getChildren()) {
					result.add(&child->normal_entry);
					result.add(&child->size_horizontal_entry);
				}
			}
		} else if (entry->update_type == WidgetUpdateType::SIZE_VERTICAL) {
			if (entry->widget->getVerticalSizePolicy() == Widget::SizePolicy::PARENT) {
				result.add(&entry->widget->getParent()->normal_entry);
				result.add(&entry->widget->getParent()->size_vertical_entry);
			} else if (entry->widget->getVerticalSizePolicy() == Widget::SizePolicy::CHILDREN) {
				for (Widget* child : entry->widget->getChildren()) {
					result.add(&child->normal_entry);
					result.add(&child->size_vertical_entry);
				}
			}
		}
		return result;
	}

}
