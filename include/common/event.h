#pragma once

#include <vector>
#include <functional>

template<typename TFunc>
class Event {
public:
	void operator+=(const std::function<TFunc>& func);
	template<typename... Args>
	void operator()(Args&&... args);
	void clear();

private:
	std::vector<std::function<TFunc>> functions;

};

template<typename TFunc>
inline void Event<TFunc>::operator+=(const std::function<TFunc>& func) {
	functions.push_back(func);
}

template<typename TFunc>
template<typename... Args>
inline void Event<TFunc>::operator()(Args&&... args) {
	for (size_t i = 0; i < functions.size(); i++) {
		functions[i](args...);
	}
}

template<typename TFunc>
inline void Event<TFunc>::clear() {
	functions = std::vector<std::function<TFunc>>();
}
