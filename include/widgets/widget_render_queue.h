#pragma once

#include <SFML/Graphics.hpp>
#include "compvector.h"

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
	CompoundVector<Widget*> widgets;
	RenderQueueLayer(RenderLayer layer);
	bool operator<(const RenderQueueLayer& other) const;
	bool operator==(const RenderQueueLayer& other) const;
};

class WidgetRenderQueue {
public:
	WidgetRenderQueue(WidgetList& widget_list);
	bool isValid() const;
	void update() const;
	const std::set<RenderQueueLayer>& getSilent() const;
	const std::set<RenderQueueLayer>& get() const;
	void invalidate();

private:
	WidgetList& widget_list;
	mutable std::set<RenderQueueLayer> layers;
	mutable bool valid = false;

};
