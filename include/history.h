#pragma once

#include <string>
#include <functional>
#include "logger.h"

class HistoryEntry {
public:
	std::string tag;
	std::string str;

	HistoryEntry(const std::string& str, const std::string& tag);
};

class History {
public:
	History();
	History(
		const std::string& name,
		std::function<std::string(void)> get,
		std::function<void(std::string)> set
	);
	size_t size();
	void save(const std::string& tag);
	void updateLast(const std::string& tag);
	void undo();
	void redo();
	void clear();
	HistoryEntry& getCurrent();

private:
	std::string name = "<unnamed>";
	std::function<std::string(void)> get;
	std::function<void(std::string)> set;
	std::vector<HistoryEntry> history;
	ptrdiff_t current = 0;

};
