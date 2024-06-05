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

	Widget* WidgetUpdateTarget::getWidget() const {
		return widget;
	}

	WidgetUpdateSocket::WidgetUpdateSocket() { }

	WidgetUpdateSocket::WidgetUpdateSocket(Widget* widget, WidgetUpdateType type) {
		this->widget = widget;
		this->update_type = type;
	}

	WidgetUpdateType WidgetUpdateSocket::getType() const {
		return update_type;
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
		const std::string& name,
		const std::vector<WidgetUpdateTarget*>& targets,
		Widget* widget,
		const FuncType& func
	) {
		this->name = name;
		this->targets = targets;
		this->widget  = widget;
		this->func = func;
	}

	const std::vector<WidgetUpdateTarget*>& WidgetLink::getTargets() const {
		return targets;
	}

	std::string WidgetLink::toStr() const {
		return "[Link] " + widget->getFullName() + " (" + name + ")";
	}

	void WidgetLink::execute() {
		func(targets);
	}

	void WidgetLink::remove() {
		widget->removeLink(this);
	}

}
