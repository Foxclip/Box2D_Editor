#include "tokenizer.h"

WordToken::WordToken(std::string str, int line) {
	this->str = str;
	this->line = line;
}

TokensPointer::TokensPointer(std::vector<WordToken>* tokens, int pos) {
	this->tokens = tokens;
	this->pos = pos;
}

WordToken TokensPointer::get() {
	pos++;
	WordToken result = peek(-1);
	return result;
}

std::string TokensPointer::gets() {
	return get().str;
}

int TokensPointer::geti() {
	return std::stoi(gets());
}

float TokensPointer::getf() {
	return std::stof(gets());
}

sf::Color TokensPointer::getColor() {
	sf::Color color;
	color.r = geti();
	color.g = geti();
	color.b = geti();
	color.a = geti();
	return color;
}

b2Vec2 TokensPointer::getb2Vec2() {
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

bool TokensPointer::valid() {
	return pos >= 0 && pos < tokens->size();
}

int TokensPointer::getLine(int offset) {
	return peek(offset).line;
}
