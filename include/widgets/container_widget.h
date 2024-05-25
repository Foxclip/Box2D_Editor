#pragma once

#include "rectangle_widget.h"

namespace fw {

	class WidgetList;

	class ContainerWidget : public RectangleWidget {
	public:
		ContainerWidget(WidgetList& widget_list, float width, float height);
		ContainerWidget(WidgetList& widget_list, const sf::Vector2f& size);
		bool getHorizontal() const;
		float getHorizontalPadding() const;
		float getVerticalPadding() const;
		Alignment getVerticalAlignment() const;
		Alignment getHorizontalAlignment() const;
		void setHorizontal(bool value);
		void setPadding(float padding);
		void setHorizontalPadding(float padding);
		void setVerticalPadding(float padding);
		void setVerticalAlignment(Alignment alignment);
		void setHorizontalAlignment(Alignment alignment);

	protected:
		void internalUpdate() override;
		void updateSizeX() override;
		void updateSizeY() override;
		Anchor alignmentToAnchor(Alignment alignment) const;
		float alignmentToOffset(Alignment alignment, float max_size) const;

	private:
		bool horizontal = true;
		float horizontal_padding = 0.0f;
		float vertical_padding = 0.0f;
		Alignment vertical_alignment = Alignment::ALIGN_TOP;
		Alignment horizontal_alignment = Alignment::ALIGN_LEFT;
		sf::FloatRect children_bounds;

	};

}
