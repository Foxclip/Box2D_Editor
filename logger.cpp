#include "logger.h"

Logger logger;

Logger& Logger::operator<<(const char* value) {
	return writeString(std::string(value));
}

Logger& Logger::operator<<(std::string value) {
	return writeString(value);
}

Logger& Logger::operator<<(int value) {
	return writeInt(value);
}

Logger& Logger::writeString(std::string value) {
	std::vector<std::string> lines = utils::splitString(value);
	for (int i = 0; i < lines.size(); i++) {
		std::string line = lines[i];
		if (line == "\n") {
			writeNewLine();
		} else {
			buffer += line;
		}
	}
	return *this;
}

Logger& Logger::writeNewLine() {
	std::string current_time = utils::current_time();
	std::cout << "[" << current_time << "] " << buffer << "\n";
	buffer = "";
	return *this;
}

Logger& Logger::writeInt(int value) {
	return writeString(std::to_string(value));
}

Logger& Logger::writeInt(float value) {
	assert(false && "Use writeFloat for writing floats");
	return *this;
}

