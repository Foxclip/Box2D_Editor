#include "serializer.h"

WordToken::WordToken() { }

WordToken::WordToken(std::string str, size_t line) {
	this->str = str;
	this->line = line;
}

TokenReader::TokenReader(std::string& str) {
	internal_tokens = tokenize(str);
	tokens = &internal_tokens;
}

TokenReader::TokenReader(std::vector<WordToken>* tokens, ptrdiff_t pos) {
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

std::string TokenReader::readString() {
	if (fail_state) {
		return "";
	}
	return get().str;
}

void TokenReader::eat(std::string expected) {
	try {
		std::string str = readString();
		if (str != expected) {
			throw std::runtime_error("Expected: \"" + expected + "\", got: \"" + str + "\"");
		}
	} catch (std::exception exc) {
		throw std::runtime_error(__FUNCTION__": " + std::string(exc.what()));
	}
}

bool TokenReader::tryEat(std::string str) {
	std::string rstr = readString();
	if (rstr == str) {
		return true;
	} else {
		move(-1);
		return false;
	}
}

int TokenReader::readInt() {
	if (fail_state) {
		return 0;
	}
	std::string str = readString();
	const char* start = str.c_str();
	char* end;
	int result = std::strtol(start, &end, 10);
	if (end == start) {
		fail_state = true;
		return 0;
	}
	return result;
}

float TokenReader::readFloat() {
	if (fail_state) {
		return 0.0f;
	}
	std::string str = readString();
	const char* start = str.c_str();
	char* end;
	float result = std::strtof(start, &end);
	if (end == start) {
		fail_state = true;
		return 0.0f;
	}
	return result;
}

bool TokenReader::readBool() {
	if (fail_state) {
		return false;
	}
	std::string str = readString();
	if (str == "true") {
		return true;
	} else if (str == "false") {
		return false;
	} else {
		fail_state = true;
		return false;
	}
}

std::vector<float> TokenReader::readFloatArr() {
	std::vector<float> result;
	if (fail_state) {
		return result;
	}
	while (!fail()) {
		float f = readFloat();
		if (!fail()) {
			result.push_back(f);
		}
	}
	reset();
	move(-1);
	return result;
}

sf::Color TokenReader::readColor() {
	if (fail_state) {
		return sf::Color();
	}
	sf::Color color;
	color.r = readInt();
	color.g = readInt();
	color.b = readInt();
	color.a = readInt();
	return color;
}

b2Vec2 TokenReader::readb2Vec2() {
	if (fail_state) {
		return b2Vec2(0.0f, 0.0f);
	}
	b2Vec2 vec;
	vec.x = readFloat();
	vec.y = readFloat();
	return vec;
}

std::vector<b2Vec2> TokenReader::readb2Vec2Arr() {
	std::vector<b2Vec2> result;
	if (fail_state) {
		return result;
	}
	while (!fail()) {
		b2Vec2 vec = readb2Vec2();
		if (!fail()) {
			result.push_back(vec);
		}
	}
	reset();
	move(-1);
	return result;
}

WordToken TokenReader::peek(ptrdiff_t offset) {
	return tokens->at(pos + offset);
}

void TokenReader::move(ptrdiff_t offset) {
	pos += offset;
}

bool TokenReader::eof() {
	return pos == tokens->size() - 1;
}

bool TokenReader::validRange() {
	return isValidPos(pos);
}

bool TokenReader::fail() {
	return fail_state;
}

void TokenReader::reset() {
	fail_state = false;
}

size_t TokenReader::getLine(ptrdiff_t offset) {
	return peek(offset).line;
}

std::vector<WordToken> TokenReader::tokenize(std::string str) {
	std::vector<WordToken> results;
	std::string current_word;
	str += EOF;
	size_t current_line = 1;
	for (size_t i = 0; i < str.size(); i++) {
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

bool TokenReader::isValidPos(ptrdiff_t position) {
	return position >= 0 && position < tokens->size();
}

TokenWriter::TokenWriter(int indent_level) {
	this->target = &internal_str;
	this->indent_level = indent_level;
	updateIndentStr();
}

TokenWriter::TokenWriter(TokenWriter& parent) {
	this->target = &internal_str;
	this->indent_level = parent.getIndentLevel();
	updateIndentStr();
}

TokenWriter::TokenWriter(std::string* target, int indent_level) {
	this->target = target;
	this->indent_level = indent_level;
	updateIndentStr();
}

TokenWriter& TokenWriter::writeString(std::string value) {
	std::vector<std::string> lines = utils::splitString(value);
	for (size_t i = 0; i < lines.size(); i++) {
		std::string str = lines[i];
		if (str == "\n") {
			writeNewLine();
		} else {
			writeLine(str);
		}
	}
	return *this;
}

TokenWriter& TokenWriter::writeInt(int value) {
	writeString(std::to_string(value));
	return *this;
}

TokenWriter& TokenWriter::writeInt(float value) {
	assert(false && "Use writeFloat for writing floats");
	return *this;
}

TokenWriter& TokenWriter::writeFloat(float value) {
	std::stringstream ss;
	ss << std::setprecision(9) << value;
	writeString(ss.str());
	return *this;
}

TokenWriter& TokenWriter::writeBool(bool value) {
	writeString(value ? "true" : "false");
	return *this;
}

TokenWriter& TokenWriter::writeFloatArr(std::vector<float> value) {
	for (size_t i = 0; i < value.size(); i++) {
		writeFloat(value[i]);
	}
	return *this;
}

TokenWriter& TokenWriter::writeColor(sf::Color value) {
	writeInt(value.r);
	writeInt(value.g);
	writeInt(value.b);
	writeInt(value.a);
	return *this;
}

TokenWriter& TokenWriter::writeb2Vec2(b2Vec2 value) {
	writeFloat(value.x);
	writeFloat(value.y);
	return *this;
}

void TokenWriter::writeStringParam(std::string name, std::string value) {
	writeString(name).writeString(value).writeNewLine();
}

void TokenWriter::writeIntParam(std::string name, int value) {
	writeString(name).writeInt(value).writeNewLine();
}

void TokenWriter::writeFloatParam(std::string name, float value) {
	writeString(name).writeFloat(value).writeNewLine();
}

void TokenWriter::writeBoolParam(std::string name, bool value) {
	writeString(name).writeBool(value).writeNewLine();
}

void TokenWriter::writeFloatArrParam(std::string name, std::vector<float> value) {
	writeString(name).writeFloatArr(value).writeNewLine();
}

void TokenWriter::writeColorParam(std::string name, sf::Color value) {
	writeString(name).writeColor(value).writeNewLine();
}

void TokenWriter::writeb2Vec2Param(std::string name, b2Vec2 value) {
	writeString(name).writeb2Vec2(value).writeNewLine();
}

int TokenWriter::getIndentLevel() {
	return indent_level;
}

void TokenWriter::addIndentLevel(int add) {
	indent_level += add;
	updateIndentStr();
}

std::string TokenWriter::toStr() {
	return std::string(*target);
}

TokenWriter& TokenWriter::operator<<(const char* value) {
	return writeString(std::string(value));
}

TokenWriter& TokenWriter::operator<<(std::string value) {
	return writeString(value);
}

TokenWriter& TokenWriter::operator<<(int value) {
	return writeInt(value);
}

TokenWriter& TokenWriter::operator<<(float value) {
	return writeFloat(value);
}

TokenWriter& TokenWriter::operator<<(bool value) {
	return writeBool(value);
}

TokenWriter& TokenWriter::operator<<(std::vector<float> value) {
	return writeFloatArr(value);
}

TokenWriter& TokenWriter::operator<<(sf::Color value) {
	return writeColor(value);
}

TokenWriter& TokenWriter::operator<<(b2Vec2 value) {
	return writeb2Vec2(value);
}

void TokenWriter::updateIndentStr() {
	indent_str = "";
	for (int i = 0; i < indent_level; i++) {
		indent_str += "    ";
	}
}

void TokenWriter::writeLine(std::string str) {
	if (new_line) {
		target->append(indent_str);
	} else {
		target->append(" ");
	}
	target->append(str);
	new_line = false;
}

void TokenWriter::writeNewLine() {
	target->append("\n");
	new_line = true;
}

TokenWriterIndent::TokenWriterIndent(TokenWriter& tw, ptrdiff_t indent) : tw(tw) {
	this->indent_level = indent;
	tw.addIndentLevel(indent);
}

TokenWriterIndent::~TokenWriterIndent() {
	close();
}

void TokenWriterIndent::close() {
	if (closed) {
		return;
	}
	tw.addIndentLevel(-indent_level);
	closed = true;
}
