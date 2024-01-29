#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include "utils.h"

extern std::unique_ptr<sf::RenderWindow> window;

class Widget {
public:
	enum Anchor {
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
	virtual void setFillColor(const sf::Color& color) = 0;
	void setPosition(float x, float y);
	void setPosition(const sf::Vector2f& position);
	void setRotation(float angle);
	void render();
	void render(sf::RenderTarget& target);

protected:
	virtual sf::Drawable* getDrawable() = 0;
	virtual sf::Transformable* getTransformable() = 0;

private:

};

class RectangleWidget : public Widget {
public:
	RectangleWidget();
	sf::FloatRect getLocalBounds();
	sf::FloatRect getGlobalBounds();
	void setFillColor(const sf::Color& color);
	void setSize(const sf::Vector2f& size);

protected:
	sf::Drawable* getDrawable();
	sf::Transformable* getTransformable();

private:
	sf::RectangleShape rect;

};

class ButtonPanelWidget : public RectangleWidget {

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

protected:
	sf::Drawable* getDrawable();
	sf::Transformable* getTransformable();

private:
	sf::Text text;

};