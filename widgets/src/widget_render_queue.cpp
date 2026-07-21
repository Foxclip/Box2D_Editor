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
		for (Widget* widget : root_render_queue) {
			bool found = false;
			size_t global_layer = static_cast<size_t>(widget->getGlobalRenderLayer());
			for (RenderQueueLayer& layer : layers) {
				if (layer.layer == global_layer) {
					layer.widgets.add(widget);
					found = true;
					break;
				}
			}
			if (!found) {
				RenderQueueLayer layer(global_layer);
				layer.widgets.add(widget);
				layers.push_back(layer);
			}
		}
	}

	const std::vector<RenderQueueLayer>& WidgetRenderQueue::get() const {
		return layers;
	}

	void WidgetRenderQueue::remove(Widget* widget) {
		for (size_t i = 0; i < layers.size(); i++) {
			if (layers[i].widgets.remove(widget) > 0) {
				break;
			}
		}
	}

}
