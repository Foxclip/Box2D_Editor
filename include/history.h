#pragma once

#include <string>
#include <functional>
#include "logger.h"

template<typename T>
class HistoryEntry {
public:
	std::string tag;
	T value;

	HistoryEntry(const T& value, const std::string& tag);
};

template<typename T>
class History {
public:
	History();
	History(
		const std::string& name,
		std::function<T(void)> get,
		std::function<void(const T&)> set
	);
	size_t size() const;
	void save(const std::string& tag);
	void updateLast(const std::string& tag);
	void undo();
	void redo();
	void clear();
	const HistoryEntry<T>& getCurrent() const;

private:
	std::string name = "<unnamed>";
	std::function<T(void)> get;
	std::function<void(const T&)> set;
	std::vector<HistoryEntry<T>> history;
	ptrdiff_t current = 0;

};

template<typename T>
HistoryEntry<T>::HistoryEntry(const T& value, const std::string& tag) {
    this->value = value;
    this->tag = tag;
}

template<typename T>
History<T>::History() { }

template<typename T>
History<T>::History(
    const std::string& name,
    std::function<T(void)> get,
    std::function<void(const T&)> set
) {
    this->name = name;
    this->get = get;
    this->set = set;
    current = -1;
}

template<typename T>
size_t History<T>::size() const {
    return history.size();
}

template<typename T>
void History<T>::save(const std::string& tag) {
    LoggerTag tag_history("history");
    if (current < history.size()) {
        history.erase(history.begin() + current + 1, history.end());
    }
    T state = get();
    HistoryEntry entry(state, tag);
    history.push_back(entry);
    current++;
    logger << "History " << name << ": save " << tag << ", current: " << current << ", size : " << history.size() << "\n";
}

template<typename T>
void History<T>::updateLast(const std::string& tag) {
    LoggerTag tag_history("history");
    logger << "History " << name << ": updateLast " << tag << "\n";
    LoggerIndent update_last_indent;
    if (current > 0) {
        T state = get();
        HistoryEntry entry(state, tag);
        history[current] = entry;
        logger << "History " << name << ": updateLast " << tag << ", current: " << current << ", size : " << history.size() << "\n";
    } else {
        save(tag);
    }
}

template<typename T>
void History<T>::undo() {
    LoggerTag tag_history("history");
    if (current > 0) {
        current--;
        T state = history[current].value;
        set(state);
        logger << "History " << name << ": undo, current: " << current << ", size: " << history.size() << "\n";
    } else {
        logger << "History " << name << ": can't undo\n";
    }
}

template<typename T>
void History<T>::redo() {
    LoggerTag tag_history("history");
    if (current < history.size() - 1) {
        current++;
        T state = history[current].value;
        set(state);
        logger << "History " << name << ": redo, current: " << current << ", size: " << history.size() << "\n";
    } else {
        logger << "History " << name << ": can't redo\n";
    }
}

template<typename T>
void History<T>::clear() {
    if (history.size() > 1) {
        history.erase(history.begin() + 1, history.end());
    }
}

template<typename T>
const HistoryEntry<T>& History<T>::getCurrent() const {
    return history[current];
}
