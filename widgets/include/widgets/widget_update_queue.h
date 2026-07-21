#pragma once

#include <string>
#include <vector>
#include "widgets/widget_link.h"

namespace fw {

	class Widget;
	class WidgetList;

	class WidgetUpdateQueue {
	public:
		WidgetUpdateQueue(WidgetList& widget_list);
		void update();
		const std::vector<std::vector<WidgetUpdateTarget*>>& get() const;

	private:
		WidgetList& widget_list;
		std::vector<std::vector<WidgetUpdateTarget*>> queue;

		static std::vector<WidgetUpdateTarget*> getParents(const WidgetUpdateTarget* entry);
	};

}
