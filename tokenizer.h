#pragma once

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include <string>
#include <vector>

class WordToken {
public:
	WordToken();
	WordToken(std::string str, int line);
	std::string str;
	int line = 0;
};

class TokenReader {
public:
	TokenReader(std::string& str);
	TokenReader(std::vector<WordToken>* tokens, int pos = 0);
	WordToken get();
	std::string gets();
	void eat(std::string expected);
	int geti();
	float getf();
	bool getb();
	std::vector<float> getfArr();
	sf::Color getColor();
	b2Vec2 getb2Vec2();
	WordToken peek(int offset = 0);
	void move(int offset);
	bool eof();
	bool valid_range();
	bool fail();
	void reset();
	int getLine(int offset = 0);
	static bool isEntityName(std::string str);
private:
	std::vector<WordToken> internal_tokens;
	std::vector<WordToken>* tokens;
	int pos = 0;
	bool fail_state = false;

	std::vector<WordToken> tokenize(std::string str);
	bool isValidPos(int pos);
};
