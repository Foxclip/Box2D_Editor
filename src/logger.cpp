#include "../include/logger.h"
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

Logger& Logger::operator<<(unsigned int value) {
	if (!logger.is_active) {
		return *this;
	}
	return writeUnsignedInt(value);
}

Logger& Logger::operator<<(std::size_t value) {
	if (!logger.is_active) {
		return *this;
	}
	return writeSizet(value);
}

Logger& Logger::operator<<(ptrdiff_t value) {
	if (!logger.is_active) {
		return *this;
	}
	return writePtrdifft(value);
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
		line_buffer += indent_str;
	}
	line_buffer += value;
	new_line = false;
	return *this;
}

Logger& Logger::writeNewLine() {
	std::string line;
	if (write_time) {
		std::string current_time = utils::current_time();
		line += "[" + current_time + "] ";
	}
	line += line_buffer;
	OnLineWrite(line);
	line += "\n";
	total_buffer += line;
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
	return writeString(utils::bool_to_str(value));
}

void Logger::updateIndentStr() {
	indent_str = "";
	for (size_t i = 0; i < indent_level; i++) {
		indent_str += "|   ";
	}
}

void Logger::flush() {
	if (!test_mode) {
		std::cout << total_buffer;
		total_buffer = "";
	}
}

void Logger::updateAcive() {
	if (tags.empty()) {
		logger.is_active = active_switch;
		return;
	}
	std::string tag = tags.back();
	if (logger.active_switch) {
		auto it = logger.disabled_tags.find(tag);
		bool found = it != logger.disabled_tags.end();
		logger.is_active = !found;
	} else {
		auto it = logger.enabled_tags.find(tag);
		bool found = it != logger.enabled_tags.end();
		logger.is_active = found;
	}
}

void LoggerControl::close() {
	if (closed) {
		return;
	}
	internalClose();
	closed = true;
}

LoggerIndent::LoggerIndent(ptrdiff_t indent) {
	this->indent_level = indent;
	logger.addIndentLevel(indent);
}

LoggerIndent::~LoggerIndent() {
	close();
}

void LoggerIndent::internalClose() {
	logger.addIndentLevel(-indent_level);
}

LoggerLargeText::LoggerLargeText() {
	autoflush_was_enabled = logger.autoflush;
	logger.autoflush = false;
}

LoggerLargeText::~LoggerLargeText() {
	close();
}

void LoggerLargeText::internalClose() {
	logger.flush();
	logger.autoflush = autoflush_was_enabled;
}

LoggerDeactivate::LoggerDeactivate() {
	assert(logger.active_switch, "Not allowed to deactivate logger twice");
	assert(logger.tags.size() == 0, "LoggerDeactivate must be outside of all LoggerTags");
	logger.active_switch = false;
}

LoggerDeactivate::~LoggerDeactivate() {
	close();
}

void LoggerDeactivate::internalClose() {
	logger.active_switch = true;
}

LoggerTag::LoggerTag(const std::string& tag) {
	logger.tags.push_back(tag);
	logger.updateAcive();
}

LoggerTag::~LoggerTag() {
	close();
}

void LoggerTag::internalClose() {
	logger.tags.pop_back();
	logger.updateAcive();
}

LoggerEnableTag::LoggerEnableTag(const std::string& tag) {
	this->tag = tag;
	logger.enabled_tags.insert(tag);
	logger.updateAcive();
}

LoggerEnableTag::~LoggerEnableTag() {
	close();
}

void LoggerEnableTag::internalClose() {
	logger.enabled_tags.erase(tag);
	logger.updateAcive();
}

LoggerDisableTag::LoggerDisableTag(const std::string& tag) {
	this->tag = tag;
	logger.disabled_tags.insert(tag);
	logger.updateAcive();
}

LoggerDisableTag::~LoggerDisableTag() {
	close();
}

void LoggerDisableTag::internalClose() {
	logger.disabled_tags.erase(tag);
	logger.updateAcive();
}

#ifndef NDEBUG

LoggerTest logger_test;

void LoggerTest::testLogger() {
	{
		// simple test
		logger << "Test\n";
		assert(logger.total_buffer == "Test\n");
		logger.total_buffer = "";
	}
	{
		// multiple lines
		logger << "Line1\n";
		logger << "Line2\n";
		assert(logger.total_buffer == "Line1\nLine2\n");
		logger.total_buffer = "";
	}
	{
		// tag
		LoggerTag tag1("tag1");
		logger << "tag1\n";
		assert(logger.total_buffer == "tag1\n");
		logger.total_buffer = "";
	}
	{
		// LoggerDeactivate
		LoggerDeactivate deact;
		LoggerTag tag1("tag1");
		logger << "tag1\n";
		assert(logger.total_buffer == "");
		logger.total_buffer = "";
	}
	{
		// enable tag
		LoggerDeactivate deact;
		LoggerEnableTag enable_tag1("tag1");
		LoggerEnableTag enable_someTag("someTag");
		LoggerDisableTag disable_someOtherTag("someOtherTag");
		{
			LoggerTag tag1("tag1");
			logger << "tag1\n";
		}
		{
			LoggerTag tag2("tag2");
			logger << "tag2\n";
		}
		assert(logger.total_buffer == "tag1\n");
		logger.total_buffer = "";
	}
	{
		// disable tag
		LoggerDisableTag disable_tag2("tag2");
		LoggerEnableTag disable_someTag("someTag");
		LoggerDisableTag disable_someOtherTag("someOtherTag");
		{
			LoggerTag tag1("tag1");
			logger << "tag1\n";
		}
		{
			LoggerTag tag2("tag2");
			logger << "tag2\n";
		}
		assert(logger.total_buffer == "tag1\n");
		logger.total_buffer = "";
	}
	{
		// return to disabled tag
		LoggerDisableTag disable_tag1("tag1");
		LoggerTag tag1("tag1");
		logger << "tag1 frist\n";
		{
			LoggerTag tag2("tag2");
			logger << "tag2\n";
		}
		logger << "tag1 second\n";
		assert(logger.total_buffer == "tag2\n");
		logger.total_buffer = "";
	}
	{
		// nested tags
		LoggerDeactivate deact;
		LoggerEnableTag enable_tag1("tag1");
		LoggerEnableTag enable_tag3("tag3");
		{
			LoggerTag tag1("tag1");
			logger << "tag1\n";
			{
				LoggerTag tag2("tag2");
				logger << "tag2\n";
				{
					LoggerTag tag3("tag3");
					logger << "tag3\n";
				}
			}
		}
		assert(logger.total_buffer == "tag1\ntag3\n");
		logger.total_buffer = "";
	}
	{
		// reenable tag
		{
			LoggerTag tag1("tag1");
			logger << "tag1\n";
			{
				LoggerDisableTag disable_tag2("tag2");
				LoggerTag tag2("tag2");
				logger << "tag2 first\n";
			}
			{
				LoggerTag tag2("tag2");
				logger << "tag2 second\n";
			}
		}
		assert(logger.total_buffer == "tag1\ntag2 second\n");
		logger.total_buffer = "";
	}
	{
		// LoggerDisableTag after LoggerTag
		{
			LoggerTag tag1("tag1");
			logger << "tag1 first\n";
			LoggerDisableTag disable_tag1("tag1");
			logger << "tag1 second\n";
			{
				LoggerTag tag2("tag2");
				logger << "tag2\n";
			}
			logger << "tag1 third\n";
		}
		assert(logger.total_buffer == "tag1 first\ntag2\n");
		logger.total_buffer = "";
	}
	{
		// reenabling tag
		{
			{
				LoggerTag tag1("tag1");
				{
					LoggerDisableTag disable_tag1("tag1");
					LoggerTag tag2("tag2");
				}
				logger << "tag1 first\n";
			}
			{
				LoggerTag tag1("tag1");
				{
					LoggerTag tag2("tag2");
					LoggerDisableTag disable_tag1("tag1");
				}
				logger << "tag1 second\n";
			}
		}
		assert(logger.total_buffer == "tag1 first\ntag1 second\n");
		logger.total_buffer = "";
	}
	{
		// more nested tags
		LoggerTag tag1("tag1");
		LoggerDisableTag disable_tag2("tag2");
		logger << "tag1 first\n";
		{
			LoggerTag tag2("tag2");
			logger << "tag2 1\n";
			{
				LoggerTag tag3("tag3");
				logger << "tag3 1\n";
				{
					LoggerTag tag2("tag2");
					logger << "tag2 2\n";
					{
						LoggerTag tag4("tag4");
						logger << "tag4 1\n";
						{
							LoggerTag tag1_again("tag1");
							logger << "tag1 second\n";
						}
						logger << "tag4 2\n";
					}
					logger << "tag2 3\n";
				}
				logger << "tag3 2\n";
			}
			logger << "tag2 4\n";
		}
		logger << "tag1 third\n";
		assert(logger.total_buffer == 
			"tag1 first\n"
			"tag3 1\n"
			"tag4 1\n"
			"tag1 second\n"
			"tag4 2\n"
			"tag3 2\n"
			"tag1 third\n"
		);
		logger.total_buffer = "";
	}
}

LoggerTest::LoggerTest() {
	logger.test_mode = true;
	logger.write_time = false;
	testLogger();
	logger = Logger();
}

#endif // !NDEBUG
