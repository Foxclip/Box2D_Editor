#pragma once

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
	float getf();
	WordToken peek(int offset);
	void move(int offset);
	bool eof();
	bool valid();
	int getLine(int offset = 0);
private:
	std::vector<WordToken>* tokens;
	int pos;
};
