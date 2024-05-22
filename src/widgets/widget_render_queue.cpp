#include "widgets/widget_render_queue.h"
#include "widgets/widget_list.h"

namespace fw {

	RenderQueueLayer::RenderQueueLayer(size_t layer) {
		this->layer = layer;
	}

	bool RenderQueueLayer::operator<(const RenderQueueLayer& other) const {
		return layer < other.layer;
	}

	bool RenderQueueLayer::operator==(const RenderQueueLayer& other) const {
		return layer == other.layer;
	}

	WidgetRenderQueue::WidgetRenderQueue(WidgetList& widget_list) : widget_list(widget_list) { }

	void WidgetRenderQueue::update() {
		layers.clear();
		CompVector<Widget*> root_render_queue = widget_list.getRootWidget()->getRenderQueue();
		for (size_t i = 0; i < root_render_queue.size(); i++) {
			Widget* widget = root_render_queue[i];
			auto it = layers.find(RenderQueueLayer(static_cast<size_t>(widget->getGlobalRenderLayer())));
			if (it != layers.end()) {
				RenderQueueLayer* layer = const_cast<RenderQueueLayer*>(&*it);
				layer->widgets.add(widget);
			} else {
				RenderQueueLayer layer(static_cast<size_t>(widget->getGlobalRenderLayer()));
				layer.widgets.add(widget);
				layers.insert(layer);
			}
		}
	}

	const std::set<RenderQueueLayer>& WidgetRenderQueue::get() const {
		return layers;
	}

}
