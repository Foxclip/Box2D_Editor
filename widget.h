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
	std::function<void(const sf::Vector2f& pos)> OnClick = [](const sf::Vector2f&) { };
	std::function<void(const sf::Vector2f& pos)> OnRelease = [](const sf::Vector2f&) { };
	std::function<void(void)> OnMouseEnter = []() { };
	std::function<void(void)> OnMouseExit = []() { };

	static bool click_blocked;
	static bool release_blocked;
	WidgetVisibility checkVisibility() const;
	bool isMouseOver() const;
	void updateMouseState();
	void processClick(const sf::Vector2f& pos);
	void processRelease(const sf::Vector2f& pos);
	const CompoundVector<Widget*>& getChildren() const;
	virtual sf::FloatRect getLocalBounds() const = 0;
	virtual sf::FloatRect getParentLocalBounds() const = 0;
	virtual sf::FloatRect getGlobalBounds() const = 0;
	sf::Vector2f getSize() const;
	float getWidth() const;
	float getHeight() const;
	const sf::Vector2f& getOrigin() const;
	const sf::Vector2f& getPosition() const;
	sf::Vector2f getGlobalPosition() const;
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
	void setPosition(float x, float y);
	void setPosition(const sf::Vector2f& position);
	void setAdjustedPosition(float x, float y);
	void setAdjustedPosition(const sf::Vector2f& position);
	void setRotation(float angle);
	void setVisible(bool value);
	void setClickThrough(bool value);
	void render();
	virtual void render(sf::RenderTarget& target);
	void addChild(std::unique_ptr<Widget> child);

protected:
	friend class WidgetTransform;
	WidgetTransform transforms = WidgetTransform(this);
	Widget* parent = nullptr;
	CompoundVectorUptr<Widget> children;
	Anchor origin_anchor = CUSTOM;
	Anchor parent_anchor = CUSTOM;
	sf::Vector2f anchor_offset = sf::Vector2f(0.0f, 0.0f);
	bool visible = true;
	bool click_through = true;
	bool mouseIn = false;

	const sf::Transform& getGlobalTransform() const;
	const sf::Transform& getParentGlobalTransform() const;
	const sf::Transform& getInverseGlobalTransform() const;
	const sf::Transform& getInverseParentGlobalTransform() const;
	virtual sf::Drawable& getDrawable() = 0;
	virtual const sf::Drawable& getDrawable() const = 0;
	virtual sf::Transformable& getTransformable() = 0;
	virtual const sf::Transformable& getTransformable() const = 0;
	virtual void update();
	sf::Vector2f anchorToPos(Anchor p_anchor, const sf::Vector2f& orig, const sf::Vector2f& size);

private:

};

class ShapeWidget : public Widget {
public:
	sf::FloatRect getLocalBounds() const override;
	sf::FloatRect getParentLocalBounds() const override;
	sf::FloatRect getGlobalBounds() const override;
	const sf::Color& getFillColor() const override;
	void setFillColor(const sf::Color& color) override;
	void setOutlineColor(const sf::Color& color);
	void setOutlineThickness(float thickness);

protected:
	virtual sf::Shape& getShape() = 0;
	virtual const sf::Shape& getShape() const = 0;
};

class RectangleWidget : public ShapeWidget {
public:
	RectangleWidget();
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
	void setAutoResize(bool value);
	void setHorizontal(bool value);
	void setPadding(float padding);

protected:
	void update() override;

private:
	bool auto_resize = true;
	bool horizontal = true;
	float padding = 0.0f;

};

class CheckboxWidget : public RectangleWidget {
public:
	CheckboxWidget();
	bool isChecked() const;
	void setChecked(bool value);

protected:
	void update() override;

private:
	bool checked = false;
	const sf::Vector2f DEFAULT_SIZE = sf::Vector2f(20.0f, 20.0f);
	float check_size = 0.6f;
	RectangleWidget* check_widget;

};

class TextWidget : public Widget {
public:
	sf::FloatRect getLocalBounds() const override;
	sf::FloatRect getParentLocalBounds() const override;
	sf::FloatRect getGlobalBounds() const override;
	const sf::Color& getFillColor() const override;
	void setFont(const sf::Font& font);
	void setString(const std::string& string);
	void setCharacterSize(unsigned int size);
	void setFillColor(const sf::Color& color) override;
	void setOriginToTextCenter();

protected:
	sf::Drawable& getDrawable() override;
	const sf::Drawable& getDrawable() const override;
	sf::Transformable& getTransformable() override;
	const sf::Transformable& getTransformable() const override;

private:
	sf::Text text;

};

extern RectangleWidget root_widget;
