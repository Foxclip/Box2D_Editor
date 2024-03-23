#include "widgets/widget_list.h"
#include "widgets/rectangle_widget.h"

WidgetList::WidgetList() {
	root_widget = createWidget<RectangleWidget>();
	root_widget->setFillColor(sf::Color::Transparent);
	root_widget->setClipChildren(true);
	root_widget->setName("root");
}

bool WidgetList::contains(const Widget* widget) {
	return widgets.contains(widget);
}

bool WidgetList::isClickBlocked() const {
	return click_blocked;
}

bool WidgetList::isReleaseBlocked() const {
	return release_blocked;
}

Widget* WidgetList::getRootWidget() const {
	return root_widget;
}

Widget* WidgetList::getFocusedWidget() const {
	return focused_widget;
}

Widget* WidgetList::find(const std::string& name) const {
	return root_widget->find(name);
}

void WidgetList::processClick(const sf::Vector2f pos) {
	focused_widget_temp = nullptr;
	root_widget->processClick(pos);
	setFocusedWidget(focused_widget_temp);
}

void WidgetList::processRelease(const sf::Vector2f pos) {
	if (focused_widget) {
		focused_widget->processRelease(pos);
	}
}

void WidgetList::processMouse(const sf::Vector2f pos) {
	root_widget->processMouse(pos);
}

void WidgetList::processKeyboardEvent(const sf::Event& event) {
	if (focused_widget) {
		focused_widget->processKeyboardEvent(event);
	}
}

void WidgetList::render(sf::RenderTarget& target) {
	root_widget->unclipped_region.invalidate();
	root_widget->update();
#ifndef NDEBUG
	for (size_t i = 0; i < widgets.size(); i++) {
		widgets[i]->updateVisibility();
	}
#endif
	updateRenderQueue();
	for (RenderQueueLayer layer : render_queue) {
		for (size_t widget_i = 0; widget_i < layer.widgets.size(); widget_i++) {
			Widget* widget = layer.widgets[widget_i];
			widget->render(target);
		}
	}
	if (debug_render) {
		root_widget->renderBounds(target, sf::Color::Green, true);
		root_widget->renderOrigin(target);
	}
	if (focused_widget) {
		focused_widget->renderBounds(target, focused_widget_bounds_color, false);
	}
}

void WidgetList::reset(const sf::Vector2f& root_size, const sf::Vector2f& mouse_pos) {
	root_widget->setSize(root_size);
	root_widget->updateMouseState(mouse_pos);
	click_blocked = false;
	release_blocked = false;
}

void WidgetList::setFocusedWidget(Widget* widget) {
	LoggerTag tag_set_focused_widget("setFocusedWidget");
	if (widget == focused_widget) {
		return;
	}
	if (widget) {
		if (!widget->isFocusable()) {
			return;
		}
		assert(widgets.contains(widget));
	}
	if (focused_widget) {
		logger << focused_widget->getFullName() << " lost focus\n";
		focused_widget->internalOnFocusLost();
		focused_widget->OnFocusLost();
	}
	focused_widget = widget;
	if (focused_widget) {
		logger << focused_widget->getFullName() << " is focused\n";
		focused_widget->internalOnFocused();
		focused_widget->OnFocused();
	}
}

void WidgetList::updateRenderQueue() {
	render_queue.clear();
	std::function<void(Widget*)> add_widget = [&](Widget* widget) {
		if (!widget->isVisible()) {
			return;
		}
		auto it = render_queue.find(RenderQueueLayer(widget->getRenderLayer()));
		if (it != render_queue.end()) {
			RenderQueueLayer* layer = const_cast<RenderQueueLayer*>(&*it);
			layer->widgets.add(widget);
		} else {
			RenderQueueLayer layer(widget->getRenderLayer());
			layer.widgets.add(widget);
			render_queue.insert(layer);
		}
		for (size_t i = 0; i < widget->getChildren().size(); i++) {
			add_widget(widget->getChild(i));
		}
	};
	add_widget(root_widget);
}

RenderQueueLayer::RenderQueueLayer(Widget::RenderLayer layer) {
	this->layer = layer;
}

bool RenderQueueLayer::operator<(const RenderQueueLayer& other) const {
	return layer < other.layer;
}
