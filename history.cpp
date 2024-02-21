#include "history.h"

History::History() {}

History::History(std::function<std::string(void)> get, std::function<void(std::string)> set) {
    this->get = get;
    this->set = set;
    current = -1;
}

size_t History::size() {
    return history.size();
}

void History::save(HistoryEntry::Type type) {
    if (current < history.size()) {
        history.erase(history.begin() + current + 1, history.end());
    }
    std::string state = get();
    HistoryEntry entry(state, type);
    history.push_back(entry);
    current++;
    logger << "Save " << HistoryEntry::typeToStr(type) << ", current: " << current << ", topSize : " << history.size() << "\n";
}

void History::undo() {
    if (current > 0) {
        current--;
        std::string state = history[current].str;
        set(state);
        logger << "Undo, current: " << current << ", topSize: " << history.size() << "\n";
    } else {
        logger << "Can't undo\n";
    }
}

void History::redo() {
    if (current < history.size() - 1) {
        current++;
        std::string state = history[current].str;
        set(state);
        logger << "Redo, current: " << current << ", topSize: " << history.size() << "\n";
    } else {
        logger << "Can't redo\n";
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

std::string HistoryEntry::typeToStr(Type type) {
    switch (type) {
        case HistoryEntry::BASE: return "base";
        case HistoryEntry::NORMAL: return "normal";
        case HistoryEntry::LOAD: return "load";
        case HistoryEntry::QUICKLOAD: return "quickload";
        default: return "Unknown";
    }
}

HistoryEntry::HistoryEntry(std::string str, Type type) {
    this->str = str;
    this->type = type;
}