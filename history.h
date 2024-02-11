#pragma once

#include <string>
#include <functional>
#include "logger.h"

class HistoryEntry {
public:
	enum Type {
		BASE,
		NORMAL,
		QUICKLOAD,
	};
	std::string str;
	Type type;
	static std::string typeToStr(Type type);

	HistoryEntry(std::string str, Type type);
};

class History {
public:
	History();
	History(std::function<std::string(void)> get, std::function<void(std::string)> set);
	size_t size();
	void save(HistoryEntry::Type type);
	void undo();
	void redo();
	void clear();
	HistoryEntry& getCurrent();

private:
	std::function<std::string(void)> get;
	std::function<void(std::string)> set;
	std::vector<HistoryEntry> history;
	ptrdiff_t current = 0;
};