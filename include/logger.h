#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <stack>
#include <set>
#include "test.h"

// if a method can modify logger object
// in a way unrelated to logging,
// add loggerAssert(!locked);

class Logger {
public:
	std::function<void(std::string line)> OnLineWrite = [](std::string line) { };

	Logger(bool test = false);
	Logger& operator<<(const char* value);
	Logger& operator<<(std::string value);
	Logger& operator<<(int value);
	Logger& operator<<(unsigned int value);
	Logger& operator<<(size_t value);
	Logger& operator<<(ptrdiff_t value);
	Logger& operator<<(float value);
	Logger& operator<<(double value);
	Logger& operator<<(bool value);
	void lock();
	void unlock();
	void addIndentLevel(ptrdiff_t level);
	void flush();
	bool getAutoFlush() const;
	void setAutoFlush(bool value);
	bool getActiveSwitch() const;
	void setActiveSwitch(bool value);
	std::vector<std::string>& getTags();
	const std::vector<std::string>& getTags() const;
	std::set<std::string>& getEnabledTags();
	const std::set<std::string>& getEnabledTags() const;
	std::set<std::string>& getDisabledTags();
	const std::set<std::string>& getDisabledTags() const;
	void updateAcive();
	const std::string& getTotalBuffer() const;

private:
	bool locked = false;
	std::string line_buffer;
	std::string total_buffer;
	bool new_line = true;
	ptrdiff_t indent_level = 0;
	std::string indent_str;
	bool autoflush = true;
	bool active_switch = true;
	bool is_active = true;
	bool test_mode = false;
	bool write_time = true;
	std::vector<std::string> tags;
	std::set<std::string> enabled_tags;
	std::set<std::string> disabled_tags;

	std::vector<std::string> splitString(const std::string& str);
	std::string currentTime();
	std::string boolToStr(bool value);
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
	void updateIndentStr();
	void internalFlush();
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
	LoggerIndent(ptrdiff_t indent = 1, bool condition = true);
	LoggerIndent(Logger& p_logger, ptrdiff_t indent = 1, bool condition = true);
	~LoggerIndent();
	void internalClose() override;

private:
	Logger& m_logger;
	ptrdiff_t indent_level = 0;

	void action(ptrdiff_t indent);
};

class LoggerLargeText : public LoggerControl {
public:
	LoggerLargeText();
	LoggerLargeText(Logger& p_logger);
	~LoggerLargeText();
	void internalClose() override;

private:
	Logger& m_logger;
	bool autoflush_was_enabled = true;

	void action();
};

class LoggerDeactivate : public LoggerControl {
public:
	LoggerDeactivate();
	LoggerDeactivate(Logger& p_logger);
	~LoggerDeactivate();
	void internalClose() override;

private:
	Logger& m_logger;

	void action();

};

class LoggerTag : public LoggerControl {
public:
	LoggerTag(const std::string& tag);
	LoggerTag(Logger& p_logger, const std::string& tag);
	~LoggerTag();
	void internalClose() override;

private:
	Logger& m_logger;

	void action(const std::string& tag);

};

class LoggerEnableTag : public LoggerControl {
public:
	LoggerEnableTag(const std::string& tag);
	LoggerEnableTag(Logger& p_logger, const std::string& tag);
	~LoggerEnableTag();
	void internalClose() override;

private:
	Logger& m_logger;
	std::string tag;

	void action(const std::string& tag);

};

class LoggerDisableTag : public LoggerControl {
public:
	LoggerDisableTag(const std::string& tag);
	LoggerDisableTag(Logger& p_logger, const std::string& tag);
	~LoggerDisableTag();
	void internalClose() override;

private:
	Logger& m_logger;
	std::string tag;

	void action(const std::string& tag);

};

#ifndef NDEBUG

class LoggerTests : public test::TestModule {
public:
	LoggerTests();

protected:
	void createTestLists();
};

#endif // NDEBUG
