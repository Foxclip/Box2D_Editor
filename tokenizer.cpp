#include "tokenizer.h"

WordToken::WordToken() { }

WordToken::WordToken(std::string str, int line) {
	this->str = str;
	this->line = line;
}

TokensPointer::TokensPointer(std::vector<WordToken>* tokens, int pos) {
	this->tokens = tokens;
	this->pos = pos;
}

WordToken TokensPointer::get() {
	if (fail_state) {
		return WordToken("", peek(tokens->size() - 1).line);
	}
	pos++;
	if (isValidPos(pos - 1)) {
		return peek(-1);
	} else {
		fail_state = true;
		return WordToken("", peek(tokens->size() - 1).line);
	}
}

std::string TokensPointer::gets() {
	if (fail_state) {
		return "";
	}
	return get().str;
}

int TokensPointer::geti() {
	if (fail_state) {
		return 0;
	}
	std::string str = gets();
	const char* start = str.c_str();
	char* end;
	int result = std::strtol(start, &end, 10);
	if (end == start) {
		fail_state = true;
		return 0;
	}
	return result;
}

float TokensPointer::getf() {
	if (fail_state) {
		return 0.0f;
	}
	std::string str = gets();
	const char* start = str.c_str();
	char* end;
	float result = std::strtof(start, &end);
	if (end == start) {
		fail_state = true;
		return 0.0f;
	}
	return result;
}

sf::Color TokensPointer::getColor() {
	if (fail_state) {
		return sf::Color();
	}
	sf::Color color;
	color.r = geti();
	color.g = geti();
	color.b = geti();
	color.a = geti();
	return color;
}

b2Vec2 TokensPointer::getb2Vec2() {
	if (fail_state) {
		return b2Vec2(0.0f, 0.0f);
	}
	b2Vec2 vec;
	vec.x = getf();
	vec.y = getf();
	return vec;
}

WordToken TokensPointer::peek(int offset) {
	return tokens->at(pos + offset);
}

void TokensPointer::move(int offset) {
	pos += offset;
}

bool TokensPointer::eof() {
	return pos == tokens->size() - 1;
}

bool TokensPointer::valid_range() {
	return isValidPos(pos);
}

bool TokensPointer::fail() {
	return fail_state;
}

void TokensPointer::reset() {
	fail_state = false;
}

int TokensPointer::getLine(int offset) {
	return peek(offset).line;
}

bool TokensPointer::isValidPos(int position) {
	return position >= 0 && position < tokens->size();
}
