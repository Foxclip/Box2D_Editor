#pragma once

#include "rectangle_widget.h"

namespace fw {

	class WidgetList;

	class ContainerWidget : public RectangleWidget {
	public:
		ContainerWidget(WidgetList& widget_list, float width, float height);
		ContainerWidget(WidgetList& widget_list, const sf::Vector2f& size);
		bool getHorizontal() const;
		float getInnerPaddingX() const;
		float getInnerPaddingY() const;
		float getTopPadding() const;
		float getBottomPadding() const;
		float getLeftPadding() const;
		float getRightPadding() const;
		Alignment getAlignmentY() const;
		Alignment getAlignmentX() const;
		float getContentWidth() const;
		float getContentHeight() const;
		void setHorizontal(bool value);
		void setPadding(float padding);
		void setPaddingX(float padding);
		void setPaddingY(float padding);
		void setInnerPaddingX(float padding);
		void setInnerPaddingY(float padding);
		void setTopPadding(float padding);
		void setBottomPadding(float padding);
		void setLeftPadding(float padding);
		void setRightPadding(float padding);
		void setAlignmentX(Alignment alignment);
		void setAlignmentY(Alignment alignment);
		ContainerWidget* clone(bool with_children = true) override;

	protected:
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
		float inner_padding_x = 0.0f;
		float inner_padding_y = 0.0f;
		float top_padding = 0.0f;
		float bottom_padding = 0.0f;
		float left_padding = 0.0f;
		float right_padding = 0.0f;
		Alignment alignment_y = Alignment::ALIGN_TOP;
		Alignment alignment_x = Alignment::ALIGN_LEFT;
		sf::FloatRect children_bounds;

	};

}
