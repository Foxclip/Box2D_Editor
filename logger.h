#pragma once

#include <string>
#include <iostream>
#include <vector>
#include "utils.h"

class Logger {
public:
	Logger& operator<<(const char* value);
	Logger& operator<<(std::string value);
	Logger& operator<<(int value);
	Logger& operator<<(std::size_t value);
	Logger& operator<<(float value);
	Logger& operator<<(bool value);

private:
	std::string buffer;
	bool new_line = true;

	Logger& writeString(std::string value);
	Logger& writeNewLine();
	Logger& writeInt(int value);
	Logger& writeInt(float value);
	Logger& writeSizet(std::size_t value);
	Logger& writeFloat(float value);
	Logger& writeBool(bool value);
};

extern Logger logger;