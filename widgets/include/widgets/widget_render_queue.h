#pragma once

#include <SFML/Graphics.hpp>
#include "common/compvector.h"

namespace fw {

	class Widget;
	class WidgetList;

	enum class GlobalRenderLayer {
		BASE,
		DROPDOWN_PANEL,
		TOP
	};

	struct RenderQueueLayer {
		size_t layer;
		CompVector<Widget*> widgets;
		RenderQueueLayer(size_t layer);
		bool operator<(const RenderQueueLayer& other) const;
		bool operator==(const RenderQueueLayer& other) const;
	};

	class WidgetRenderQueue {
	public:
		WidgetRenderQueue(WidgetList& widget_list);
		void update();
		const std::vector<RenderQueueLayer>& get() const;
		void remove(Widget* widget);

	private:
		WidgetList& widget_list;
		std::vector<RenderQueueLayer> layers;

	};

}
