#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include "utils.h"
#include "logger.h"
#include "global.h"

struct WidgetVisibility {
	bool addedToRoot = false;
	bool visibleSetting = false;
	bool onScreen = false;
	bool nonZeroSize = false;
	bool opaque = false;
	//bool notCovered = false;
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
	std::function<void(sf::Vector2f pos)> OnClick = [](sf::Vector2f) { };
	std::function<void(void)> OnMouseEnter = []() { };
	std::function<void(void)> OnMouseExit = []() { };

	static bool click_blocked;
	WidgetVisibility checkVisibility();
	bool isMouseOver();
	void updateMouseState();
	void processClick(const sf::Vector2f& pos);
	std::vector<Widget*> getChildren();
	virtual sf::FloatRect getLocalBounds() = 0;
	virtual sf::FloatRect getGlobalBounds() = 0;
	float getWidth();
	float getHeight();
	const sf::Vector2f& getPosition();
	const sf::Vector2f getTopLeft();
	const sf::Vector2f getTopRight();
	const sf::Vector2f getBottomLeft();
	const sf::Vector2f getBottomRight();
	virtual const sf::Color& getFillColor() = 0;
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
	Widget* parent = nullptr;
	std::vector<std::unique_ptr<Widget>> children;
	Anchor origin_anchor = CUSTOM;
	Anchor parent_anchor = CUSTOM;
	sf::Vector2f anchor_offset = sf::Vector2f(0.0f, 0.0f);
	bool visible = true;
	bool click_through = true;
	bool mouseIn = false;
	sf::Transform getGlobalTransform();
	sf::Transform getParentGlobalTransform();
	virtual sf::Drawable& getDrawable() = 0;
	virtual sf::Transformable& getTransformable() = 0;
	virtual void update();
	sf::Vector2f anchorToPos(Anchor p_anchor, const sf::Vector2f& orig, const sf::Vector2f& size);

private:

};

class ShapeWidget : public Widget {
public:
	sf::FloatRect getLocalBounds() override;
	sf::FloatRect getGlobalBounds() override;
	const sf::Color& getFillColor() override;
	void setFillColor(const sf::Color& color) override;
	void setOutlineColor(const sf::Color& color);
	void setOutlineThickness(float thickness);

protected:
	virtual sf::Shape& getShape() = 0;
};

class RectangleWidget : public ShapeWidget {
public:
	RectangleWidget();
	void setSize(const sf::Vector2f& size);

protected:
	sf::RectangleShape rect;

	sf::Drawable& getDrawable() override;
	sf::Transformable& getTransformable() override;
	sf::Shape& getShape() override;

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

class TextWidget : public Widget {
public:
	sf::FloatRect getLocalBounds() override;
	sf::FloatRect getGlobalBounds() override;
	const sf::Color& getFillColor() override;
	void setFont(const sf::Font& font);
	void setString(const std::string& string);
	void setCharacterSize(unsigned int size);
	void setFillColor(const sf::Color& color) override;
	void setOriginToTextCenter();

protected:
	sf::Drawable& getDrawable() override;
	sf::Transformable& getTransformable() override;

private:
	sf::Text text;

};

extern RectangleWidget root_widget;
