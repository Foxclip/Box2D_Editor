#pragma once

#include <string>
#include <functional>

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

	class Widget;

	class WidgetUpdateTarget {
	public:
		WidgetUpdateTarget();
		WidgetUpdateTarget(Widget* widget, WidgetUpdateType type);
		Widget* getWidget() const;
		WidgetUpdateType getType() const;
		std::string toStr() const;
		float getValue() const;
		void setValue(float value) const;

	private:
		Widget* widget = nullptr;
		WidgetUpdateType type = WidgetUpdateType::NONE;

	};

	class WidgetLink {
	public:
		using FuncType = std::function<void(const WidgetUpdateTarget&, const WidgetUpdateTarget&)>;
		WidgetLink(const WidgetUpdateTarget& src, const WidgetUpdateTarget& dst);
		WidgetLink(const WidgetUpdateTarget& src, const WidgetUpdateTarget& dst, const FuncType& func);

	private:
		WidgetUpdateTarget src;
		WidgetUpdateTarget dst;
		FuncType func;

	};

}