#pragma once

#include "widgets/container_widget.h"

namespace fw {

	class TreeViewEntry;
	class TextWidget;
	class PolygonWidget;
	class EmptyWidget;

	class TreeViewEntryWidget : public ContainerWidget {
	public:
		TreeViewEntryWidget(WidgetList& widget_list, TreeViewEntry& entry, const std::string& name, float width, float height);
		void updateWidgets();
		void setGrabbedVisualMode();
		TreeViewEntryWidget* clone(bool with_children = true);

	private:
		friend class TreeViewEntry;
		TreeViewEntry& entry;
		RectangleWidget* rectangle_widget = nullptr;
		TextWidget* text_widget = nullptr;
		RectangleWidget* arrow_area_widget = nullptr;
		PolygonWidget* arrow_widget = nullptr;
		ContainerWidget* children_box_widget = nullptr;
		EmptyWidget* children_spacing_widget = nullptr;
		ContainerWidget* children_widget = nullptr;

	};

}
