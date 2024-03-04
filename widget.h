#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include "utils.h"
#include "logger.h"
#include "global.h"
#include "compvector.h"

struct WidgetVisibility {
	bool addedToRoot = false;
	bool visibleSetting = false;
	bool onScreen = false;
	bool nonZeroSize = false;
	bool opaque = false;
	//bool notCovered = false;
};

class Widget;
class WidgetList;

class WidgetTransform {
public:
	WidgetTransform(Widget* widget);
	const sf::Transform& getTransform() const;
	const sf::Transform& getGlobalTransform() const;
	const sf::Transform& getInverseGlobalTransform() const;
	const sf::Vector2f& getPosition() const;
	float getRotation() const;
	const sf::Vector2f& getScale() const;
	void invalidateGlobalTransform();
	void setPosition(const sf::Vector2f& position);
	void setRotation(float angle);
	void setScale(const sf::Vector2f& scale);

private:
	Widget* widget = nullptr;
	mutable sf::Transform global_transform;
	mutable sf::Transform inv_global_transform;
	mutable bool global_transform_valid = false;
	mutable bool inv_global_transform_valid = false;

	void recalcGlobalTransform() const;
	void recalcInverseGlobalTransform() const;

};

class Widget {
public:
	enum Anchor {
		CUSTOM,
		TOP_LEFT,
		TOP_CENTER,
		TOP_RIGHT,
		CENTER_LEFT,
		CENTER,
		CENTER_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		BOTTOM_RIGHT,
	};
	ptrdiff_t debug_id = -1;
	std::string name = "<unnamed>";

	std::function<void(const sf::Vector2f& pos)> OnClick = [](const sf::Vector2f& pos) { };
	std::function<void(const sf::Vector2f& pos)> OnRelease = [](const sf::Vector2f& pos) { };
	std::function<void(const sf::Vector2f& pos)> OnMouseEnter = [](const sf::Vector2f& pos) { };
	std::function<void(const sf::Vector2f& pos)> OnMouseExit = [](const sf::Vector2f& pos) { };

	bool isMouseOver() const;
	void updateMouseState();
	WidgetVisibility checkVisibility() const;
	void processClick(const sf::Vector2f& pos);
	void processRelease(const sf::Vector2f& pos);
	Widget* getParent() const;
	const CompoundVector<Widget*>& getChildren() const;
	virtual sf::FloatRect getLocalBounds() const = 0;
	virtual sf::FloatRect getParentLocalBounds() const = 0;
	virtual sf::FloatRect getGlobalBounds() const = 0;
	const sf::Vector2f& toGlobal(const sf::Vector2f& pos) const;
	const sf::Vector2f& toLocal(const sf::Vector2f& pos) const;
	sf::Vector2f getSize() const;
	float getWidth() const;
	float getHeight() const;
	const sf::Vector2f& getOrigin() const;
	virtual const sf::Vector2f& getPosition() const;
	virtual const sf::Vector2f& getGlobalPosition() const;
	const sf::Vector2f getTopLeft() const;
	const sf::Vector2f getTopRight() const;
	const sf::Vector2f getBottomLeft() const;
	const sf::Vector2f getBottomRight() const;
	virtual const sf::Color& getFillColor() const = 0;
	void setOrigin(Anchor anchor);
	void setOrigin(float x, float y);
	void setOrigin(const sf::Vector2f& origin);
	void setParentAnchor(Anchor anchor);
	void setAnchorOffset(float x, float y);
	void setAnchorOffset(const sf::Vector2f& offset);
	virtual void setFillColor(const sf::Color& color) = 0;
	virtual void setPosition(float x, float y);
	virtual void setPosition(const sf::Vector2f& position);
	void setRotation(float angle);
	void setVisible(bool value);
	void setClickThrough(bool value);
	void setParent(Widget* new_parent);
	virtual void render(sf::RenderTarget& target);
	void renderBounds(sf::RenderTarget& target);
	void renderOrigin(sf::RenderTarget& target);

protected:
	friend class WidgetList;
	friend class WidgetTransform;
	WidgetList* widget_list = nullptr;
	WidgetTransform transforms = WidgetTransform(this);
	Widget* parent = nullptr;
	CompoundVector<Widget*> children;
	Anchor origin_anchor = CUSTOM;
	Anchor parent_anchor = CUSTOM;
	sf::Vector2f anchor_offset = sf::Vector2f(0.0f, 0.0f);
	bool visible = true;
	bool click_through = true;
	bool mouseIn = false;

	sf::Vector2f anchorToPos(Anchor p_anchor, const sf::Vector2f& orig, const sf::Vector2f& size);
	const sf::Transform& getGlobalTransform() const;
	const sf::Transform& getParentGlobalTransform() const;
	const sf::Transform& getInverseGlobalTransform() const;
	const sf::Transform& getInverseParentGlobalTransform() const;
	virtual sf::Drawable& getDrawable() = 0;
	virtual const sf::Drawable& getDrawable() const = 0;
	virtual sf::Transformable& getTransformable() = 0;
	virtual const sf::Transformable& getTransformable() const = 0;
	virtual void update();
	virtual void internalOnClick(const sf::Vector2f& pos);
	virtual void internalOnMouseEnter(const sf::Vector2f& pos);
	virtual void internalOnMouseExit(const sf::Vector2f& pos);

private:

};

class ShapeWidget : public Widget {
public:
	sf::FloatRect getLocalBounds() const override;
	sf::FloatRect getParentLocalBounds() const override;
	sf::FloatRect getGlobalBounds() const override;
	virtual const sf::Color& getFillColor() const override;
	virtual void setFillColor(const sf::Color& color) override;
	void setOutlineColor(const sf::Color& color);
	void setOutlineThickness(float thickness);

protected:
	virtual sf::Shape& getShape() = 0;
	virtual const sf::Shape& getShape() const = 0;
};

class RectangleWidget : public ShapeWidget {
public:
	RectangleWidget();
	RectangleWidget(WidgetList* widget_list);
	void setSize(const sf::Vector2f& size);

protected:
	sf::RectangleShape rect;

	sf::Drawable& getDrawable() override;
	const sf::Drawable& getDrawable() const override;
	sf::Transformable& getTransformable() override;
	const sf::Transformable& getTransformable() const override;
	sf::Shape& getShape() override;
	const sf::Shape& getShape() const override;

private:

};

class ContainerWidget : public RectangleWidget {
public:
	ContainerWidget();
	ContainerWidget(WidgetList* widget_list);
	void setAutoResize(bool value);
	void setHorizontal(bool value);
	void setPadding(float padding);
	void setHorizontalPadding(float padding);
	void setVerticalPadding(float padding);

protected:
	void update() override;

private:
	bool auto_resize = true;
	bool horizontal = true;
	float horizontal_padding = 0.0f;
	float vertical_padding = 0.0f;

};

class CheckboxWidget : public RectangleWidget {
public:
	CheckboxWidget();
	CheckboxWidget(WidgetList* widget_list);
	bool isChecked() const;
	const sf::Color& getFillColor() const override;
	const sf::Color& getHighlightFillColor() const;
	const sf::Color& getCheckFillColor() const;
	void setFillColor(const sf::Color& color) override;
	void setHighlightFillColor(const sf::Color& color);
	void setCheckFillColor(const sf::Color& color);
	void setChecked(bool value);

protected:
	void update() override;
	void internalOnClick(const sf::Vector2f& pos) override;
	void internalOnMouseEnter(const sf::Vector2f& pos) override;
	void internalOnMouseExit(const sf::Vector2f& pos) override;

private:
	bool checked = false;
	const sf::Vector2f DEFAULT_SIZE = sf::Vector2f(20.0f, 20.0f);
	float check_size = 0.6f;
	RectangleWidget* check_widget = nullptr;
	sf::Color background_fill_color = sf::Color(50, 50, 50);
	sf::Color highlight_fill_color = sf::Color(128, 128, 128);
	sf::Color check_fill_color = sf::Color(255, 128, 0);

	void toggleChecked();
};

class TextWidget : public Widget {
public:
	TextWidget();
	TextWidget(WidgetList* widget_list);
	sf::FloatRect getLocalBounds() const override;
	sf::FloatRect getParentLocalBounds() const override;
	sf::FloatRect getGlobalBounds() const override;
	sf::FloatRect getExactLocalBounds() const;
	const sf::Vector2f& getPosition() const override;
	const sf::Color& getFillColor() const override;
	void setPosition(float x, float y) override;
	void setPosition(const sf::Vector2f& position) override;
	void setFont(const sf::Font& font);
	void setString(const std::string& string);
	void setCharacterSize(unsigned int size);
	void setFillColor(const sf::Color& color) override;
	void setAdjustLocalBounds(bool value);

protected:
	bool adjust_local_bounds = true;

	sf::Drawable& getDrawable() override;
	const sf::Drawable& getDrawable() const override;
	sf::Transformable& getTransformable() override;
	const sf::Transformable& getTransformable() const override;

private:
	sf::Text text;

};

class WidgetList {
public:
	WidgetList();
	bool isClickBlocked() const;
	bool isReleaseBlocked() const;
	Widget* getRootWidget();
	template<typename T>
	requires std::derived_from<T, Widget>
	T* createWidget();
	void processClick(const sf::Vector2f pos);
	void processRelease(const sf::Vector2f pos);
	void render(sf::RenderTarget& target);
	void reset(const sf::Vector2f& root_size);

private:
	friend class Widget;
	bool click_blocked = false;
	bool release_blocked = false;
	bool debug_render = true;
	float render_origin_size = 10.0f;
	sf::Color render_bounds_color = sf::Color::Green;
	sf::Color render_origin_color = sf::Color::Red;
	CompoundVectorUptr<Widget> widgets;
	RectangleWidget* root_widget;

};

template<typename T>
requires std::derived_from<T, Widget>
inline T* WidgetList::createWidget() {
	std::unique_ptr<T> uptr = std::make_unique<T>(this);
	T* ptr = uptr.get();
	ptr->widget_list = this;
	if (root_widget) {
		ptr->setParent(root_widget);
	}
	widgets.add(std::move(uptr));
	return ptr;
}
