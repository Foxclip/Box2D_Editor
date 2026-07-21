#pragma once

#include <string>
#include <functional>
#include "widgets_common.h"
#include "common/compvector.h"

namespace fw {

	enum class WidgetUpdateType {
		NONE,
		POS_X,
		POS_Y,
		SIZE_X,
		SIZE_Y,
		CHILDREN_X,
		CHILDREN_Y,
	};

	std::string update_type_to_str(WidgetUpdateType update_type);

	class Widget;
	class WidgetLink;

	class WidgetUpdateTarget {
	public:
		Widget* getWidget() const;
		virtual void execute() = 0;
		virtual std::string toStr() const = 0;
		virtual const CompVector<WidgetUpdateTarget*> getTargets() const;
		void addTarget(WidgetUpdateTarget* target);
		void removeTarget(WidgetUpdateTarget* target);

	protected:
		friend class Widget;
		Widget* widget = nullptr;
		CompVector<WidgetUpdateTarget*> targets;
		CompVector<WidgetLink*> dependent_links;

	private:

	};

	class WidgetUpdateSocket : public WidgetUpdateTarget {
	public:
		WidgetUpdateSocket();
		WidgetUpdateSocket(Widget* widget, WidgetUpdateType type);
		WidgetUpdateType getType() const;
		std::string toStr() const override;
		void execute() override;

	private:
		friend class Widget;
		WidgetUpdateType update_type = WidgetUpdateType::NONE;

	};

	using ExecuteFuncType = std::function<void(void)>;
	using TargetsFuncType = std::function<CompVector<WidgetUpdateTarget*>(void)>;

	class WidgetLink : public WidgetUpdateTarget {
	public:
		WidgetLink(
			const std::string& name,
			const CompVector<WidgetUpdateTarget*>& targets,
			Widget* widget,
			const ExecuteFuncType& func
		);
		WidgetLink(
			const std::string& name,
			const TargetsFuncType& targets_func,
			Widget* widget,
			const ExecuteFuncType& func
		);
		const CompVector<WidgetUpdateTarget*> getTargets() const override;
		std::string toStr() const override;
		void execute() override;
		void remove();

	private:
		friend class Widget;
		bool fixed_targets = true;
		TargetsFuncType targets_func;
		std::string name;
		ExecuteFuncType func;

	};

}
