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
		Widget* getWidget() const;
		WidgetUpdateType getType() const;
		virtual void execute() = 0;
		virtual std::string toStr() const = 0;

	protected:
		Widget* widget = nullptr;
		WidgetUpdateType update_type = WidgetUpdateType::NONE;

	private:

	};

	class WidgetUpdateSocket : public WidgetUpdateTarget {
	public:
		WidgetUpdateSocket();
		WidgetUpdateSocket(Widget* widget, WidgetUpdateType type);
		std::string toStr() const override;
		void execute() override;

	private:

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
		std::string toStr() const override;
		void execute() override;
		void remove();

	private:
		std::vector<WidgetUpdateTarget*> targets;
		FuncType func;

	};

}
