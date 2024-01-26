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

private:
	std::string buffer;
	bool new_line = true;

	Logger& writeString(std::string value);
	Logger& writeNewLine();
	Logger& writeInt(int value);
	Logger& writeInt(float value);
};

extern Logger logger;