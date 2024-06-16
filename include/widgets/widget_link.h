#pragma once

#include <string>
#include <functional>
#include "widgets_common.h"
#include "compvector.h"

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
		const CompVector<WidgetUpdateTarget*>& getTargets() const;
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

	using FuncType = std::function<void(void)>;

	class WidgetLink : public WidgetUpdateTarget {
	public:
		WidgetLink(
			const std::string& name,
			const CompVector<WidgetUpdateTarget*>& targets,
			Widget* widget,
			const FuncType& func
		);
		std::string toStr() const override;
		void execute() override;
		void remove();

	private:
		friend class Widget;
		std::string name;
		FuncType func;

	};

}
