#include "../include/history.h"

History::History() {}

History::History(
    const std::string& name,
    std::function<std::string(void)> get,
    std::function<void(std::string)> set
) {
    this->name = name;
    this->get = get;
    this->set = set;
    current = -1;
}

size_t History::size() {
    return history.size();
}

void History::save(const std::string& tag) {
    LoggerTag tag_history("history");
    if (current < history.size()) {
        history.erase(history.begin() + current + 1, history.end());
    }
    std::string state = get();
    HistoryEntry entry(state, tag);
    history.push_back(entry);
    current++;
    logger << "History " << name << ": save " << tag << ", current: " << current << ", size : " << history.size() << "\n";
}

void History::updateLast(const std::string& tag) {
    LoggerTag tag_history("history");
    logger << "History " << name << ": updateLast " << tag << "\n";
    LoggerIndent update_last_indent;
    if (current > 0) {
        std::string state = get();
        HistoryEntry entry(state, tag);
        history[current] = entry;
        logger << "History " << name << ": updateLast " << tag << ", current: " << current << ", size : " << history.size() << "\n";
    } else {
        save(tag);
    }
}

void History::undo() {
    LoggerTag tag_history("history");
    if (current > 0) {
        current--;
        std::string state = history[current].str;
        set(state);
        logger << "History " << name << ": undo, current: " << current << ", size: " << history.size() << "\n";
    } else {
        logger << "History " << name << ": can't undo\n";
    }
}

void History::redo() {
    LoggerTag tag_history("history");
    if (current < history.size() - 1) {
        current++;
        std::string state = history[current].str;
        set(state);
        logger << "History " << name << ": redo, current: " << current << ", size: " << history.size() << "\n";
    } else {
        logger << "History " << name << ": can't redo\n";
    }
}

void History::clear() {
    if (history.size() > 1) {
        history.erase(history.begin() + 1, history.end());
    }
}

HistoryEntry& History::getCurrent() {
    return history[current];
}

HistoryEntry::HistoryEntry(const std::string& str, const std::string& tag) {
    this->str = str;
    this->tag = tag;
}
