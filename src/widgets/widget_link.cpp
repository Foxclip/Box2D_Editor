#include "widgets/widget_link.h"
#include "widgets/widgets_common.h"
#include "widgets/widget.h"

namespace fw {

	std::string update_type_to_str(WidgetUpdateType update_type) {
		std::string type_str;
		switch (update_type) {
			case WidgetUpdateType::NORMAL: type_str = "NORMAL"; break;
			case WidgetUpdateType::POS_X: type_str = "POS_X"; break;
			case WidgetUpdateType::POS_Y: type_str = "POS_Y"; break;
			case WidgetUpdateType::SIZE_X: type_str = "SIZE_X"; break;
			case WidgetUpdateType::SIZE_Y: type_str = "SIZE_Y"; break;
			case WidgetUpdateType::CHILDREN_X: type_str = "CHILDREN_X"; break;
			case WidgetUpdateType::CHILDREN_Y: type_str = "CHILDREN_Y"; break;
			default: wAssert("Unknown update type"); type_str = "<unknown>"; break;
		}
		return type_str;
	}

	WidgetUpdateSocket::WidgetUpdateSocket() { }

	WidgetUpdateSocket::WidgetUpdateSocket(Widget* widget, WidgetUpdateType type) {
		this->widget = widget;
		this->update_type = type;
	}

	Widget* WidgetUpdateSocket::getWidget() const {
		return widget;
	}

	WidgetUpdateType WidgetUpdateSocket::getType() const {
		return update_type;
	}

	float WidgetUpdateSocket::getValue() const {
		switch (update_type) {
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

	void WidgetUpdateSocket::setValue(float value) const {
		switch (update_type) {
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

	void WidgetUpdateSocket::execute() {
		switch (update_type) {
			case fw::WidgetUpdateType::NONE: wAssert(false, "Update type is not set") break;
			case fw::WidgetUpdateType::NORMAL: widget->update(); break;
			case fw::WidgetUpdateType::POS_X: widget->updatePositionX(); break;
			case fw::WidgetUpdateType::POS_Y: widget->updatePositionY(); break;
			case fw::WidgetUpdateType::SIZE_X: widget->updateSizeX(); break;
			case fw::WidgetUpdateType::SIZE_Y: widget->updateSizeY(); break;
			case fw::WidgetUpdateType::CHILDREN_X: widget->updateChildrenX(); break;
			case fw::WidgetUpdateType::CHILDREN_Y: widget->updateChildrenY(); break;
			default: wAssert(false, "Unknown type") break;
		}
	}

	std::string WidgetUpdateSocket::toStr() const {
		return "[Socket] " + widget->getFullName() + " (" + update_type_to_str(update_type) + ")";
	}

	WidgetLink::WidgetLink(
		const std::vector<WidgetUpdateTarget*>& targets,
		Widget* widget,
		WidgetUpdateType update_type,
		const FuncType& func
	) {
		this->targets = targets;
		this->widget  = widget;
		this->update_type = update_type;
		this->func = func;
	}

	const std::vector<WidgetUpdateTarget*>& WidgetLink::getTargets() const {
		return targets;
	}

	Widget* WidgetLink::getWidget() const {
		return widget;
	}

	std::string WidgetLink::toStr() const {
		return "[Link] " + widget->getFullName() + " (" + update_type_to_str(update_type) + ")";
	}

	void WidgetLink::execute() {
		func(targets);
	}

}
