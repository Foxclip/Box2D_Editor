#pragma once

#include <SFML/Graphics.hpp>
#include "compvector.h"

namespace fw {

	class Widget;
	class WidgetList;

	enum RenderLayer {
		BASE,
		TEXTBOX_SELECTION,
		TEXTBOX_TEXT,
		TEXTBOX_CURSOR,
	};

	struct RenderQueueLayer {
		RenderLayer layer;
		CompVector<Widget*> widgets;
		RenderQueueLayer(RenderLayer layer);
		bool operator<(const RenderQueueLayer& other) const;
		bool operator==(const RenderQueueLayer& other) const;
	};

	class WidgetRenderQueue {
	public:
		WidgetRenderQueue(WidgetList& widget_list);
		void update();
		const std::set<RenderQueueLayer>& get() const;

	private:
		WidgetList& widget_list;
		std::set<RenderQueueLayer> layers;

	};

}
