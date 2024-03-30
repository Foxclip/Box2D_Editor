#pragma once

#include "rectangle_widget.h"

namespace fw {

	class WidgetList;

	class ContainerWidget : public RectangleWidget {
	public:
		ContainerWidget(WidgetList& widget_list);
		void setAutoResize(bool value);
		void setHorizontal(bool value);
		void setPadding(float padding);
		void setHorizontalPadding(float padding);
		void setVerticalPadding(float padding);
		void setVerticalAlignment(Alignment alignment);
		void setHorizontalAlignment(Alignment alignment);

	protected:
		void update() override;
		Anchor alignmentToAnchor(Alignment alignment) const;
		float alignmentToOffset(Alignment alignment, float max_size) const;

	private:
		bool auto_resize = true;
		bool horizontal = true;
		float horizontal_padding = 0.0f;
		float vertical_padding = 0.0f;
		Alignment vertical_alignment = ALIGN_TOP;
		Alignment horizontal_alignment = ALIGN_LEFT;

	};

}
