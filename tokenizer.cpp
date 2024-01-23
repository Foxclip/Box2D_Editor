#include "tokenizer.h"

WordToken::WordToken() { }

WordToken::WordToken(std::string str, int line) {
	this->str = str;
	this->line = line;
}

TokenReader::TokenReader(std::string& str) {
	internal_tokens = tokenize(str);
	tokens = &internal_tokens;
}

TokenReader::TokenReader(std::vector<WordToken>* tokens, int pos) {
	this->tokens = tokens;
	this->pos = pos;
}

WordToken TokenReader::get() {
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

std::string TokenReader::gets() {
	if (fail_state) {
		return "";
	}
	return get().str;
}

void TokenReader::eat(std::string expected) {
	try {
		std::string str = gets();
		if (str != expected) {
			throw std::runtime_error("Expected: \"" + expected + "\", got: \"" + str + "\"");
		}
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

int TokenReader::geti() {
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

float TokenReader::getf() {
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

bool TokenReader::getb() {
	if (fail_state) {
		return false;
	}
	std::string str = gets();
	if (str == "true") {
		return true;
	} else if (str == "false") {
		return false;
	} else {
		fail_state = true;
		return false;
	}
}

std::vector<float> TokenReader::getfArr() {
	std::vector<float> result;
	while (!fail()) {
		float f = getf();
		if (!fail()) {
			result.push_back(f);
		}
	}
	reset();
	move(-1);
	return result;
}

sf::Color TokenReader::getColor() {
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

b2Vec2 TokenReader::getb2Vec2() {
	if (fail_state) {
		return b2Vec2(0.0f, 0.0f);
	}
	b2Vec2 vec;
	vec.x = getf();
	vec.y = getf();
	return vec;
}

WordToken TokenReader::peek(int offset) {
	return tokens->at(pos + offset);
}

void TokenReader::move(int offset) {
	pos += offset;
}

bool TokenReader::eof() {
	return pos == tokens->size() - 1;
}

bool TokenReader::valid_range() {
	return isValidPos(pos);
}

bool TokenReader::fail() {
	return fail_state;
}

void TokenReader::reset() {
	fail_state = false;
}

int TokenReader::getLine(int offset) {
	return peek(offset).line;
}

bool TokenReader::isEntityName(std::string str) {
	return str == "camera" || str == "object" || str == "joint";
}

std::vector<WordToken> TokenReader::tokenize(std::string str) {
	std::vector<WordToken> results;
	std::string current_word;
	str += EOF;
	int current_line = 1;
	for (int i = 0; i < str.size(); i++) {
		char c = str[i];
		if (isspace(c) || c == EOF) {
			if (current_word.size() > 0) {
				results.push_back(WordToken(current_word, current_line));
			}
			current_word = "";
		} else {
			current_word += c;
		}
		if (c == '\n') {
			current_line++;
		}
	}
	return results;
}

bool TokenReader::isValidPos(int position) {
	return position >= 0 && position < tokens->size();
}
