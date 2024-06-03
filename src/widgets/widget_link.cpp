#include "widgets/widget_link.h"
#include "widgets/widgets_common.h"
#include "widgets/widget.h"

namespace fw {

	WidgetUpdateTarget::WidgetUpdateTarget() { }

	WidgetUpdateTarget::WidgetUpdateTarget(Widget* widget, WidgetUpdateType type) {
		this->widget = widget;
		this->type = type;
	}

	Widget* WidgetUpdateTarget::getWidget() const {
		return widget;
	}

	WidgetUpdateType WidgetUpdateTarget::getType() const {
		return type;
	}

	float WidgetUpdateTarget::getValue() const {
		switch (type) {
			case WidgetUpdateType::NONE: wAssert(false, "Invalid update type"); return 0.0f;
			case WidgetUpdateType::NORMAL: wAssert(false, "Can't get a value from NORMAL"); return 0.0f;
			case WidgetUpdateType::POS_X: return widget->getPosition().x;
			case WidgetUpdateType::POS_Y: return widget->getPosition().y;
			case WidgetUpdateType::SIZE_X: return widget->getWidth();
			case WidgetUpdateType::SIZE_Y: return widget->getHeight();
			case WidgetUpdateType::CHILDREN_X: wAssert(false, "Can't get a value from CHILDREN_X"); return 0.0f;
			case WidgetUpdateType::CHILDREN_Y: wAssert(false, "Can't get a value from CHILDREN_Y"); return 0.0f;
			default: wAssert(false, "Unknown update type") return 0.0f;
		}
	}

	void WidgetUpdateTarget::setValue(float value) const {
		switch (type) {
			case WidgetUpdateType::NONE: wAssert(false, "Invalid update type"); break;
			case WidgetUpdateType::NORMAL: wAssert(false, "Can't set value to NORMAL"); break;
			case WidgetUpdateType::POS_X: return widget->setPositionX(value); break;
			case WidgetUpdateType::POS_Y: return widget->setPositionY(value); break;
			case WidgetUpdateType::SIZE_X: return widget->setWidth(value); break;
			case WidgetUpdateType::SIZE_Y: return widget->setHeight(value); break;
			case WidgetUpdateType::CHILDREN_X: wAssert(false, "Can't set value to CHILDREN_X"); break;
			case WidgetUpdateType::CHILDREN_Y: wAssert(false, "Can't set value to CHILDREN_Y"); break;
			default: wAssert(false, "Unknown update type");
		}
	}

	std::string WidgetUpdateTarget::toStr() const {
		std::string type_str;
		switch (type) {
			case WidgetUpdateType::NORMAL: type_str = "NORMAL"; break;
			case WidgetUpdateType::POS_X: type_str = "POS_X"; break;
			case WidgetUpdateType::POS_Y: type_str = "POS_Y"; break;
			case WidgetUpdateType::SIZE_X: type_str = "SIZE_X"; break;
			case WidgetUpdateType::SIZE_Y: type_str = "SIZE_Y"; break;
			case WidgetUpdateType::CHILDREN_X: type_str = "CHILDREN_X"; break;
			case WidgetUpdateType::CHILDREN_Y: type_str = "CHILDREN_Y"; break;
			default: wAssert("Unknown update type"); type_str = "<unknown>"; break;
		}
		return widget->getFullName() + " (" + type_str + ")";
	}

	WidgetLink::WidgetLink(const WidgetUpdateTarget& src, const WidgetUpdateTarget& dst) {
		this->src = src;
		this->dst = dst;
		this->func = [](const WidgetUpdateTarget& src, const WidgetUpdateTarget& dst) {
			dst.setValue(src.getValue());
		};
	}

	WidgetLink::WidgetLink(const WidgetUpdateTarget& src, const WidgetUpdateTarget& dst, const FuncType& func)
		: WidgetLink(src, dst) {
		this->func = func;
	}

}