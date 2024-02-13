#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include "utils.h"

class Logger {
public:
	Logger& operator<<(const char* value);
	Logger& operator<<(std::string value);
	Logger& operator<<(int value);
	Logger& operator<<(size_t value);
	Logger& operator<<(ptrdiff_t value);
	Logger& operator<<(float value);
	Logger& operator<<(double value);
	Logger& operator<<(bool value);
	std::function<void(std::string line)> OnLineWrite = [](std::string line) { };

private:
	std::string line_buffer;
	std::string total_buffer;
	bool new_line = true;
	size_t indent_level = 0;
	std::string indent_str;
	bool autoflush = true;
	bool is_active = true;

	Logger& writeString(std::string value);
	Logger& writeToLineBuffer(std::string value);
	Logger& writeNewLine();
	Logger& writeInt(int value);
	Logger& writeInt(float value);
	Logger& writeInt(double value);
	Logger& writeSizet(size_t value);
	Logger& writePtrdifft(ptrdiff_t value);
	Logger& writeFloat(float value);
	Logger& writeDouble(double value);
	Logger& writeBool(bool value);
	void addIndentLevel(ptrdiff_t level);
	void updateIndentStr();
	void flush();
	friend class LoggerIndent;
	friend class LoggerLargeText;
	friend class LoggerDeactivate;
};

extern Logger logger;

class LoggerControl {
public:
	virtual void close() = 0;

protected:
	bool closed = false;
};

class LoggerIndent : public LoggerControl {
public:
	LoggerIndent(ptrdiff_t indent = 1);
	~LoggerIndent();
	void close() override;

private:
	ptrdiff_t indent_level;

};

class LoggerLargeText : public LoggerControl {
public:
	LoggerLargeText();
	~LoggerLargeText();
	void close() override;

private:
	bool autoflush_was_enabled = true;

};

class LoggerDeactivate : public LoggerControl {
public:
	LoggerDeactivate();
	~LoggerDeactivate();
	void close() override;

private:
	bool logger_was_active = true;

};
