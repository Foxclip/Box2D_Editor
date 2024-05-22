#pragma once

#include "compvector.h"

namespace fw {

	class Widget;
	class WidgetList;

	enum class WidgetUpdateType {
		NORMAL,
		SIZE_HORIZONTAL,
		SIZE_VERTICAL,
	};

	struct WidgetUpdateQueueEntry {
		WidgetUpdateQueueEntry(Widget* widget, WidgetUpdateType update_type);
		Widget* widget = nullptr;
		WidgetUpdateType update_type;
		bool operator<(const WidgetUpdateQueueEntry& other) const;
	};

	class WidgetUpdateQueue {
	public:
		WidgetUpdateQueue(WidgetList& widget_list);
		void update();
		const std::vector<std::vector<WidgetUpdateQueueEntry>>& get() const;

	private:
		WidgetList& widget_list;
		std::vector<std::vector<WidgetUpdateQueueEntry>> queue;

		static std::vector<WidgetUpdateQueueEntry> getParents(const WidgetUpdateQueueEntry& entry);
	};

}
