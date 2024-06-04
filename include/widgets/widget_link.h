#pragma once

#include <string>
#include <functional>
#include "widgets_common.h"

namespace fw {

	enum class WidgetUpdateType {
		NONE,
		NORMAL,
		POS_X,
		POS_Y,
		SIZE_X,
		SIZE_Y,
		CHILDREN_X,
		CHILDREN_Y,
	};

	std::string update_type_to_str(WidgetUpdateType update_type);

	class Widget;

	class WidgetUpdateTarget {
	public:
		virtual void execute() = 0;
		virtual std::string toStr() const = 0;

	private:
	};

	class WidgetUpdateSocket : public WidgetUpdateTarget {
	public:
		WidgetUpdateSocket();
		WidgetUpdateSocket(Widget* widget, WidgetUpdateType type);
		Widget* getWidget() const;
		WidgetUpdateType getType() const;
		std::string toStr() const override;
		float getValue() const;
		void setValue(float value) const;
		void execute() override;

	private:
		Widget* widget = nullptr;
		WidgetUpdateType update_type = WidgetUpdateType::NONE;

	};

	using FuncType = std::function<void(const std::vector<WidgetUpdateTarget*>& targets)>;

	class WidgetLink : public WidgetUpdateTarget {
	public:
		WidgetLink(
			const std::vector<WidgetUpdateTarget*>& targets,
			Widget* widget,
			WidgetUpdateType update_type,
			const FuncType& func
		);
		const std::vector<WidgetUpdateTarget*>& getTargets() const;
		Widget* getWidget() const;
		std::string toStr() const override;
		void execute() override;

	private:
		std::vector<WidgetUpdateTarget*> targets;
		Widget* widget = nullptr;
		WidgetUpdateType update_type;
		FuncType func;

	};

}
