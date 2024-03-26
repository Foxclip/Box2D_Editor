#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include "utils.h"
#include <stack>
#include <set>

class Logger {
public:
	Logger& operator<<(const char* value);
	Logger& operator<<(std::string value);
	Logger& operator<<(int value);
	Logger& operator<<(unsigned int value);
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
	bool active_switch = true;
	bool is_active = true;
	bool test_mode = false;
	bool write_time = true;
	std::vector<std::string> tags;
	std::set<std::string> enabled_tags;
	std::set<std::string> disabled_tags;

	Logger& writeString(std::string value);
	Logger& writeToLineBuffer(std::string value);
	Logger& writeNewLine();
	Logger& writeInt(int value);
	Logger& writeUnsignedInt(unsigned int value);
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
	void updateAcive();
	friend class LoggerIndent;
	friend class LoggerLargeText;
	friend class LoggerDeactivate;
	friend class LoggerTag;
	friend class LoggerEnableTag;
	friend class LoggerDisableTag;
	friend class LoggerTest;
};

extern Logger logger;

class LoggerControl {
public:

protected:
	void close();
	virtual void internalClose() = 0;
	bool closed = false;
};

class LoggerIndent : public LoggerControl {
public:
	LoggerIndent(ptrdiff_t indent = 1);
	~LoggerIndent();
	void internalClose() override;

private:
	ptrdiff_t indent_level;

};

class LoggerLargeText : public LoggerControl {
public:
	LoggerLargeText();
	~LoggerLargeText();
	void internalClose() override;

private:
	bool autoflush_was_enabled = true;

};

class LoggerDeactivate : public LoggerControl {
public:
	LoggerDeactivate();
	~LoggerDeactivate();
	void internalClose() override;

private:

};

class LoggerTag : public LoggerControl {
public:
	LoggerTag(const std::string& tag);
	~LoggerTag();
	void internalClose() override;

private:

};

class LoggerEnableTag : public LoggerControl {
public:
	LoggerEnableTag(const std::string& tag);
	~LoggerEnableTag();
	void internalClose() override;

private:
	std::string tag;

};

class LoggerDisableTag : public LoggerControl {
public:
	LoggerDisableTag(const std::string& tag);
	~LoggerDisableTag();
	void internalClose() override;

private:
	std::string tag;

};

#ifndef NDEBUG

class LoggerTest {
public:
	LoggerTest();
	void testLogger();
};

#endif // NDEBUG
