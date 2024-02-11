#include "logger.h"
#include <cassert>

Logger logger;

Logger& Logger::operator<<(const char* value) {
	if (!logger.is_active) {
		return *this;
	}
	return writeString(std::string(value));
}

Logger& Logger::operator<<(std::string value) {
	if (!logger.is_active) {
		return *this;
	}
	return writeString(value);
}

Logger& Logger::operator<<(int value) {
	if (!logger.is_active) {
		return *this;
	}
	return writeInt(value);
}

Logger& Logger::operator<<(std::size_t value) {
	if (!logger.is_active) {
		return *this;
	}
	return writeSizet(value);
}

Logger& Logger::operator<<(float value) {
	if (!logger.is_active) {
		return *this;
	}
	return writeFloat(value);
}

Logger& Logger::operator<<(double value) {
	if (!logger.is_active) {
		return *this;
	}
	return writeDouble(value);
}

Logger& Logger::operator<<(bool value) {
	if (!logger.is_active) {
		return *this;
	}
	return writeBool(value);
}

void Logger::addIndentLevel(ptrdiff_t level) {
	indent_level += level;
	updateIndentStr();
}

Logger& Logger::writeString(std::string value) {
	std::vector<std::string> lines = utils::splitString(value);
	for (int i = 0; i < lines.size(); i++) {
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
		line_buffer += indent_str;
	}
	line_buffer += value;
	new_line = false;
	return *this;
}

Logger& Logger::writeNewLine() {
	std::string current_time = utils::current_time();
	total_buffer += "[" + current_time + "] " + line_buffer + "\n";
	if (autoflush) {
		flush();
	}
	line_buffer = "";
	new_line = true;
	return *this;
}

Logger& Logger::writeInt(int value) {
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

Logger& Logger::writeFloat(float value) {
	return writeString(std::to_string(value));
}

Logger& Logger::writeDouble(double value) {
	return writeString(std::to_string(value));
}

Logger& Logger::writeBool(bool value) {
	return writeString(utils::bool_to_str(value));
}

void Logger::updateIndentStr() {
	indent_str = "";
	for (size_t i = 0; i < indent_level; i++) {
		indent_str += "|   ";
	}
}

void Logger::flush() {
	std::cout << total_buffer;
	total_buffer = "";
}

LoggerIndent::LoggerIndent(ptrdiff_t indent) {
	this->indent_level = indent;
	logger.addIndentLevel(indent);
}

LoggerIndent::~LoggerIndent() {
	close();
}

void LoggerIndent::close() {
	if (closed) {
		return;
	}
	logger.addIndentLevel(-indent_level);
	closed = true;
}

LoggerLargeText::LoggerLargeText() {
	autoflush_was_enabled = logger.autoflush;
	logger.autoflush = false;
}

LoggerLargeText::~LoggerLargeText() {
	close();
}

void LoggerLargeText::close() {
	if (closed) {
		return;
	}
	logger.flush();
	logger.autoflush = autoflush_was_enabled;
	closed = true;
}

LoggerDeactivate::LoggerDeactivate() {
	logger_was_active = logger.is_active;
	logger.is_active = false;
}

LoggerDeactivate::~LoggerDeactivate() {
	close();
}

void LoggerDeactivate::close() {
	if (closed) {
		return;
	}
	logger.is_active = logger_was_active;
	closed = true;
}
