#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include "utils.h"
#include "logger.h"

extern std::unique_ptr<sf::RenderWindow> window;

class Widget {
public:
	enum Anchor {
		CUSTOM,
		TOP_LEFT,
		TOP_CENTER,
		TOP_RIGHT,
		LEFT_CENTER,
		CENTER,
		RIGHT_CENTER,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		BOTTOM_RIGHT,
	};
	std::function<void(sf::Vector2f pos)> OnClick = [](sf::Vector2f) { };

	bool isMouseOver();
	virtual sf::FloatRect getLocalBounds() = 0;
	virtual sf::FloatRect getGlobalBounds() = 0;
	float getWidth();
	float getHeight();
	const sf::Vector2f& getPosition();
	const sf::Vector2f getTopLeft();
	const sf::Vector2f getTopRight();
	const sf::Vector2f getBottomLeft();
	const sf::Vector2f getBottomRight();
	void setOrigin(Anchor anchor);
	void setOrigin(float x, float y);
	void setOrigin(const sf::Vector2f& origin);
	void setParentAnchor(Anchor anchor);
	virtual void setFillColor(const sf::Color& color) = 0;
	void setPosition(float x, float y);
	void setPosition(const sf::Vector2f& position);
	void setAdjustedPosition(float x, float y);
	void setAdjustedPosition(const sf::Vector2f& position);
	void setRotation(float angle);
	void setVisible(bool value);
	void render();
	virtual void render(sf::RenderTarget& target);
	void addChild(std::unique_ptr<Widget> child);

protected:
	Widget* parent = nullptr;
	std::vector<std::unique_ptr<Widget>> children;
	Anchor origin_anchor = CUSTOM;
	Anchor parent_anchor = CUSTOM;
	bool visible = true;
	sf::Transform getTransform();
	sf::Transform getParentTransform();
	virtual sf::Drawable& getDrawable() = 0;
	virtual sf::Transformable& getTransformable() = 0;
	virtual void update();

private:

};

class ShapeWidget : public Widget {
public:
	sf::FloatRect getLocalBounds();
	sf::FloatRect getGlobalBounds();
	void setFillColor(const sf::Color& color);
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

	sf::Drawable& getDrawable();
	sf::Transformable& getTransformable();
	sf::Shape& getShape();

private:

};

class ContainerWidget : public RectangleWidget {
public:
	ContainerWidget();
	void setHorizontal(bool value);
	void setPadding(float padding);

protected:
	void update();

private:
	bool horizontal = true;
	float padding = 0.0f;

};

class ButtonWidget : public RectangleWidget {

};

class TextWidget : public Widget {
public:
	sf::FloatRect getLocalBounds();
	sf::FloatRect getGlobalBounds();
	void setFont(const sf::Font& font);
	void setString(const std::string& string);
	void setCharacterSize(unsigned int size);
	void setFillColor(const sf::Color& color);
	void setOriginToTextCenter();

protected:
	sf::Drawable& getDrawable();
	sf::Transformable& getTransformable();

private:
	sf::Text text;

};