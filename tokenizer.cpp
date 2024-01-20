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

float TokensPointer::getf() {
	return std::stof(gets());
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
