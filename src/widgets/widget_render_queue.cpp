#include "widgets/widget_render_queue.h"
#include "widgets/widget_list.h"

namespace fw {

	RenderQueueLayer::RenderQueueLayer(RenderLayer layer) {
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
		std::function<void(Widget*)> add_widget = [&](Widget* widget) {
			if (!widget->isVisible()) {
				return;
			}
			auto it = layers.find(RenderQueueLayer(widget->getRenderLayer()));
			if (it != layers.end()) {
				RenderQueueLayer* layer = const_cast<RenderQueueLayer*>(&*it);
				layer->widgets.add(widget);
			} else {
				RenderQueueLayer layer(widget->getRenderLayer());
				layer.widgets.add(widget);
				layers.insert(layer);
			}
			for (size_t i = 0; i < widget->getChildren().size(); i++) {
				add_widget(widget->getChild(i));
			}
		};
		add_widget(widget_list.getRootWidget());
	}

	const std::set<RenderQueueLayer>& WidgetRenderQueue::get() const {
		return layers;
	}

}
