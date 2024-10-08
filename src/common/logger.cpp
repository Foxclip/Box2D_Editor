﻿#include "common/logger.h"
#include <cassert>
#include <sstream>
#include <iomanip>

#ifndef NDEBUG

void _loggerAssert_print_msg(bool value) { }

void _loggerAssert_print_msg(bool value, const std::string& message) {
	if (!value) {
		std::cout << message << "\n";
	}
}

#define loggerAssert(value, ...) \
	_loggerAssert_print_msg(value, __VA_ARGS__); \
	assert(value);

#else

#define loggerAssert(value, ...)

#endif // !NDEBUG

Logger logger;

Logger::Logger(bool test) {
	if (test) {
		test_mode = true;
		write_time = false;
	}
}

#define LOGGER_CHECKS() \
	loggerAssert(!locked); \
	if (!is_active || !manual_switch_active) { \
		return *this; \
	}

Logger& Logger::operator<<(const char* value) {
	LOGGER_CHECKS();
	return writeString(std::string(value));
}

Logger& Logger::operator<<(std::string value) {
	LOGGER_CHECKS();
	return writeString(value);
}

Logger& Logger::operator<<(int value) {
	LOGGER_CHECKS();
	return writeInt(value);
}

Logger& Logger::operator<<(unsigned int value) {
	LOGGER_CHECKS();
	return writeUnsignedInt(value);
}

Logger& Logger::operator<<(std::size_t value) {
	LOGGER_CHECKS();
	return writeSizet(value);
}

Logger& Logger::operator<<(ptrdiff_t value) {
	LOGGER_CHECKS();
	return writePtrdifft(value);
}

Logger& Logger::operator<<(float value) {
	LOGGER_CHECKS();
	return writeFloat(value);
}

Logger& Logger::operator<<(double value) {
	LOGGER_CHECKS();
	return writeDouble(value);
}

Logger& Logger::operator<<(bool value) {
	LOGGER_CHECKS();
	return writeBool(value);
}

Logger& Logger::operator<<(const std::filesystem::path& value) {
	LOGGER_CHECKS();
	return writePath(value);
}

Logger& Logger::operator<<(const LoggerFlush& value) {
	LOGGER_CHECKS();
	flushLineBuffer();
	return *this;
}

void Logger::lock() {
	loggerAssert(!locked);
	locked = true;
}

void Logger::unlock() {
	locked = false;
}

void Logger::manualActivate() {
	loggerAssert(!locked);
	manual_switch_active = true;
}

void Logger::manualDeactivate() {
	loggerAssert(!locked);
	manual_switch_active = false;
}

void Logger::addIndentLevel(ptrdiff_t level) {
	loggerAssert(!locked);
	indent_level += level;
	indent_level = std::max((ptrdiff_t)0, indent_level);
	updateIndentStr();
}

void Logger::flush() {
	loggerAssert(!locked);
	internalFlush();
}

bool Logger::getAutoFlush() const {
	return autoflush;
}

void Logger::setAutoFlush(bool value) {
	loggerAssert(!locked);
	this->autoflush = value;
}

bool Logger::getActiveSwitch() const {
	return active_switch;
}

void Logger::setActiveSwitch(bool value) {
	loggerAssert(!locked);
	this->active_switch = value;
}

void Logger::disableStdWrite() {
	std_write = false;
}

void Logger::enableStdWrite() {
	std_write = true;
}

std::vector<std::string>& Logger::getTags() {
	loggerAssert(!locked);
	return tags;
}

const std::vector<std::string>& Logger::getTags() const {
	return tags;
}

std::set<std::string>& Logger::getEnabledTags() {
	loggerAssert(!locked);
	return enabled_tags;
}

const std::set<std::string>& Logger::getEnabledTags() const {
	return enabled_tags;
}

std::set<std::string>& Logger::getDisabledTags() {
	loggerAssert(!locked);
	return disabled_tags;
}

const std::set<std::string>& Logger::getDisabledTags() const {
	return disabled_tags;
}

void Logger::updateAcive() {
	loggerAssert(!locked);
	if (tags.empty()) {
		is_active = active_switch;
		return;
	}
	std::string tag = tags.back();
	if (active_switch) {
		auto it = disabled_tags.find(tag);
		bool found = it != disabled_tags.end();
		is_active = !found;
	} else {
		auto it = enabled_tags.find(tag);
		bool found = it != enabled_tags.end();
		is_active = found;
	}
}

const std::string& Logger::getLineBuffer() const {
	return line_buffer;
}

const std::string& Logger::getTotalBuffer() const {
	return total_buffer;
}

std::vector<std::string> Logger::splitString(const std::string& str) {
	std::vector<std::string> results;
	std::string current_word;
	for (size_t i = 0; i < str.size() + 1; i++) {
		char c;
		if (i < str.size()) {
			c = str[i];
		} else {
			c = EOF;
		}
		if (c == '\n' || c == EOF) {
			if (current_word.size() > 0) {
				results.push_back(current_word);
			}
			if (c == '\n') {
				results.push_back("\n");
			}
			current_word = "";
		} else {
			current_word += c;
		}
	}
	return results;
}

std::string Logger::currentTime() {
	time_t t;
	std::time(&t);
	tm l;
	localtime_s(&l, &t);
	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << l.tm_hour;
	ss << ":" << std::setw(2) << std::setfill('0') << l.tm_min;
	ss << ":" << std::setw(2) << std::setfill('0') << l.tm_sec;
	return ss.str();
}

std::string Logger::boolToStr(bool value) {
	return value ? "true" : "false";
}

Logger& Logger::writeString(std::string value) {
	std::vector<std::string> lines = splitString(value);
	for (size_t i = 0; i < lines.size(); i++) {
		std::string line = lines[i];
		if (line == "\n") {
			writeNewLine();
		} else {
			writeToLineBuffer(line);
		}
	}
	return *this;
}

Logger& Logger::writeToLineBuffer(std::string value) {
	if (new_line) {
		if (write_time) {
			std::string current_time = currentTime();
			line_buffer += "[" + current_time + "] ";
		}
		line_buffer += indent_str;
	}
	line_buffer += value;
	new_line = false;
	return *this;
}

Logger& Logger::writeNewLine() {
	flushLineBuffer(true);
	new_line = true;
	return *this;
}

Logger& Logger::writeInt(int value) {
	return writeString(std::to_string(value));
}

Logger& Logger::writeUnsignedInt(unsigned int value) {
	return writeString(std::to_string(value));
}

Logger& Logger::writeInt(float value) {
	assert(false && "Use writeFloat for writing floats");
	return *this;
}

Logger& Logger::writeInt(double value) {
	assert(false && "Use writeDouble for writing double");
	return *this;
}

Logger& Logger::writeSizet(std::size_t value) {
	return writeString(std::to_string(value));
}

Logger& Logger::writePtrdifft(ptrdiff_t value) {
	return writeString(std::to_string(value));
}

Logger& Logger::writeFloat(float value) {
	return writeString(std::to_string(value));
}

Logger& Logger::writeDouble(double value) {
	return writeString(std::to_string(value));
}

Logger& Logger::writeBool(bool value) {
	return writeString(boolToStr(value));
}

Logger& Logger::writePath(const std::filesystem::path& value) {
	return writeString(value.string());
}

void Logger::updateIndentStr() {
	indent_str = "";
	for (size_t i = 0; (ptrdiff_t)i < indent_level; i++) {
		indent_str += "|   ";
	}
}

void Logger::internalFlush() {
	if (std_write) {
		std::cout << total_buffer;
	}
	if (!test_mode) {
		total_buffer = "";
	}
}

void Logger::flushLineBuffer(bool write_newline) {
	OnLineWrite(line_buffer);
	if (write_newline) {
		line_buffer += "\n";
	}
	total_buffer += line_buffer;
	if (autoflush) {
		internalFlush();
	}
	new_line = false;
	line_buffer = "";
}

void LoggerControl::close() {
	if (closed) {
		return;
	}
	internalClose();
	closed = true;
}

LoggerIndent::LoggerIndent(ptrdiff_t indent, bool condition) : m_logger(logger) {
	if (condition) {
		action(indent);
	} else {
		close();
	}
}

LoggerIndent::LoggerIndent(Logger& p_logger, ptrdiff_t indent, bool condition) : m_logger(p_logger) {
	if (condition) {
		action(indent);
	} else {
		close();
	}
}

LoggerIndent::~LoggerIndent() {
	close();
}

void LoggerIndent::internalClose() {
	m_logger.addIndentLevel(-indent_level);
}

void LoggerIndent::action(ptrdiff_t indent) {
	this->indent_level = indent;
	m_logger.addIndentLevel(indent);
}

LoggerLargeText::LoggerLargeText() : m_logger(logger) {
	action();
}

LoggerLargeText::LoggerLargeText(Logger& p_logger) : m_logger(p_logger) {
	action();
}

LoggerLargeText::~LoggerLargeText() {
	close();
}

void LoggerLargeText::internalClose() {
	m_logger.flush();
	m_logger.setAutoFlush(autoflush_was_enabled);
}

void LoggerLargeText::action() {
	autoflush_was_enabled = m_logger.getAutoFlush();
	m_logger.setAutoFlush(false);
}

LoggerDeactivate::LoggerDeactivate() : m_logger(logger) {
	action();
}

LoggerDeactivate::LoggerDeactivate(Logger& p_logger) : m_logger(p_logger) {
	action();
}

LoggerDeactivate::~LoggerDeactivate() {
	close();
}

void LoggerDeactivate::internalClose() {
	m_logger.setActiveSwitch(true);
}

void LoggerDeactivate::action() {
	loggerAssert(m_logger.getActiveSwitch(), "Not allowed to deactivate logger twice");
	loggerAssert(m_logger.getTags().size() == 0, "LoggerDeactivate must be outside of all LoggerTags");
	m_logger.setActiveSwitch(false);
}

LoggerTag::LoggerTag(const std::string& tag) : m_logger(logger) {
	action(tag);
}

LoggerTag::LoggerTag(Logger& p_logger, const std::string& tag) : m_logger(p_logger) {
	action(tag);
}

LoggerTag::~LoggerTag() {
	close();
}

void LoggerTag::internalClose() {
	m_logger.getTags().pop_back();
	m_logger.updateAcive();
}

void LoggerTag::action(const std::string& tag) {
	m_logger.getTags().push_back(tag);
	m_logger.updateAcive();
}

LoggerEnableTag::LoggerEnableTag(const std::string& tag) : m_logger(logger) {
	action(tag);
}

LoggerEnableTag::LoggerEnableTag(Logger& p_logger, const std::string& tag) : m_logger(p_logger) {
	action(tag);
}

LoggerEnableTag::~LoggerEnableTag() {
	close();
}

void LoggerEnableTag::internalClose() {
	m_logger.getEnabledTags().erase(tag);
	m_logger.updateAcive();
}

void LoggerEnableTag::action(const std::string& tag) {
	this->tag = tag;
	m_logger.getEnabledTags().insert(tag);
	m_logger.updateAcive();
}

LoggerDisableTag::LoggerDisableTag(const std::string& tag) : m_logger(logger) {
	action(tag);
}

LoggerDisableTag::LoggerDisableTag(Logger& p_logger, const std::string& tag) : m_logger(p_logger) {
	action(tag);
}

LoggerDisableTag::~LoggerDisableTag() {
	close();
}

void LoggerDisableTag::internalClose() {
	m_logger.getDisabledTags().erase(tag);
	m_logger.updateAcive();
}

void LoggerDisableTag::action(const std::string& tag) {
	this->tag = tag;
	m_logger.getDisabledTags().insert(tag);
	m_logger.updateAcive();
}
