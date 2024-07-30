#pragma once

#include "common/compvector.h"

namespace fw {

	class Widget;

	class WidgetParentChain {
	public:
		WidgetParentChain(Widget& widget);
		const CompVector<Widget*>& get() const;
		void invalidate();

	private:
		Widget& widget;
		mutable bool valid = false;
		mutable CompVector<Widget*> chain;

		void recalc() const;

	};

}
