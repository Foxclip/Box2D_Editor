#pragma once

#include "rectangle_widget.h"

namespace fw {

	class WidgetList;

	class ContainerWidget : public RectangleWidget {
	public:
		ContainerWidget(WidgetList& widget_list, float width, float height);
		ContainerWidget(WidgetList& widget_list, const sf::Vector2f& size);
		bool getHorizontal() const;
		float getPaddingX() const;
		float getPaddingY() const;
		Alignment getAlignmentY() const;
		Alignment getAlignmentX() const;
		void setHorizontal(bool value);
		void setPadding(float padding);
		void setPaddingX(float padding);
		void setPaddingY(float padding);
		void setAlignmentX(Alignment alignment);
		void setAlignmentY(Alignment alignment);

	protected:
		void internalUpdate() override;
		void updateSizeX() override;
		void updateSizeY() override;
		void updateChildrenX() override;
		void updateChildrenY() override;
		Anchor alignmentToAnchor(Alignment alignment) const;
		float alignmentToOffset(Alignment alignment, float max_size) const;

	private:
		void updatePrimary();
		void updateSecondary();
		bool horizontal = true;
		float padding_x = 0.0f;
		float padding_y = 0.0f;
		Alignment alignment_y = Alignment::ALIGN_TOP;
		Alignment alignment_x = Alignment::ALIGN_LEFT;
		sf::FloatRect children_bounds;

	};

}
