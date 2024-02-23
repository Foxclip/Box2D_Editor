#pragma once

#include <SFML/Graphics.hpp>
#include "box2d/box2d.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "utils.h"

class WordToken {
public:
	WordToken();
	WordToken(std::string str, size_t line);
	std::string str;
	size_t line = 0;
};

class TokenReader {
public:
	TokenReader(std::string& str);
	TokenReader(std::vector<WordToken>* tokens, ptrdiff_t pos = 0);
	WordToken get();
	void eat(std::string expected);
	bool tryEat(std::string str);
	std::string readString();
	long long readLL();
	unsigned long long readULL();
	float readFloat();
	bool readBool();
	std::vector<float> readFloatArr();
	sf::Color readColor();
	b2Vec2 readb2Vec2();
	std::vector<b2Vec2> readb2Vec2Arr();
	WordToken peek(ptrdiff_t offset = 0);
	void move(ptrdiff_t offset);
	bool eof();
	bool validRange();
	bool fail();
	void reset();
	size_t getLine(ptrdiff_t offset = 0);
private:
	std::vector<WordToken> internal_tokens;
	std::vector<WordToken>* tokens;
	ptrdiff_t pos = 0;
	bool fail_state = false;

	std::vector<WordToken> tokenize(std::string str);
	bool isValidPos(ptrdiff_t pos);
};

class TokenWriter {
public:
	TokenWriter(int indent_level = 0);
	TokenWriter(TokenWriter& parent);
	TokenWriter(std::string* target, int indent_level = 0);
	TokenWriter& operator<<(const char* value);
	TokenWriter& operator<<(std::string value);
	TokenWriter& operator<<(int value);
	TokenWriter& operator<<(size_t value);
	TokenWriter& operator<<(ptrdiff_t value);
	TokenWriter& operator<<(float value);
	TokenWriter& operator<<(bool value);
	TokenWriter& operator<<(std::vector<float> value);
	TokenWriter& operator<<(sf::Color value);
	TokenWriter& operator<<(b2Vec2 value);
	void writeStringParam(std::string name, std::string value);
	void writeIntParam(std::string name, int value);
	void writeSizetParam(std::string name, size_t value);
	void writePtrdiffParam(std::string name, ptrdiff_t value);
	void writeFloatParam(std::string name, float value);
	void writeBoolParam(std::string name, bool value);
	void writeFloatArrParam(std::string name, std::vector<float> value);
	void writeColorParam(std::string name, sf::Color value);
	void writeb2Vec2Param(std::string name, b2Vec2 value);
	int getIndentLevel();
	std::string toStr();

private:
	std::string* target;
	std::string internal_str;
	int indent_level = 0;
	std::string indent_str;
	bool new_line = true;

	void addIndentLevel(int add);
	void updateIndentStr();
	void writeLine(std::string str);
	void writeNewLine();
	TokenWriter& writeString(std::string value);
	TokenWriter& writeInt(int value);
	TokenWriter& writeInt(float value);
	TokenWriter& writeSizet(size_t value);
	TokenWriter& writePtrdifft(ptrdiff_t value);
	TokenWriter& writeFloat(float value);
	TokenWriter& writeBool(bool value);
	TokenWriter& writeFloatArr(std::vector<float> value);
	TokenWriter& writeColor(sf::Color value);
	TokenWriter& writeb2Vec2(b2Vec2 value);

	friend class TokenWriterIndent;
};

class TokenWriterControl {
public:
	virtual void close() = 0;

protected:
	bool closed = false;
};

class TokenWriterIndent : public TokenWriterControl {
public:
	TokenWriterIndent(TokenWriter& tw, ptrdiff_t indent = 1);
	~TokenWriterIndent();
	void close() override;

private:
	TokenWriter& tw;
	ptrdiff_t indent_level;

};
