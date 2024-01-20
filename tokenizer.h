#pragma once

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include <string>
#include <vector>

class WordToken {
public:
	WordToken(std::string str, int line);
	std::string str;
	int line;
};

class TokensPointer {
public:
	TokensPointer(std::vector<WordToken>* tokens, int pos = 0);
	WordToken get();
	std::string gets();
	int geti();
	float getf();
	sf::Color getColor();
	b2Vec2 getb2Vec2();
	WordToken peek(int offset);
	void move(int offset);
	bool eof();
	bool valid();
	int getLine(int offset = 0);
private:
	std::vector<WordToken>* tokens;
	int pos;
};
