#pragma once

#include <SFML/Graphics.hpp>
#include "widgets_common.h"

namespace fw {

	class Widget;

	class WidgetUnclippedRegion {
	public:
		WidgetUnclippedRegion(Widget* widget);
		const sf::FloatRect& get() const;
		const sf::FloatRect& getQuantized() const;
		bool isNonZero() const;
		bool isQuantizedNonZero() const;
		void recalc() const;
		void invalidate();

	private:
		Widget* widget = nullptr;
		mutable sf::FloatRect unclippedRegion;
		mutable sf::FloatRect quantizedUnclippedRegion;
		mutable bool valid = false;

	};

}
