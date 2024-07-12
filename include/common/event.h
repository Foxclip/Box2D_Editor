#pragma once

#include <vector>
#include <functional>

template<typename ...TArgs>
class Event {
public:
	void operator+=(const std::function<void(TArgs...)>& func);
	template<typename ...TFuncArgs>
	void operator()(TFuncArgs&&... args);
	void clear();

private:
	std::vector<std::function<void(TArgs...)>> functions;

	void invoke(TArgs&&... args);

};

template<typename... TArgs>
inline void Event<TArgs...>::operator+=(const std::function<void(TArgs...)>& func) {
	functions.push_back(func);
}

template<typename ...TArgs>
template<typename ...TFuncArgs>
inline void Event<TArgs...>::operator()(TFuncArgs&&... args) {
	invoke(std::forward<TArgs&&>(const_cast<TArgs&&>(args))...);
}

template<typename ...TArgs>
inline void Event<TArgs...>::clear() {
	functions = std::vector<std::function<TArgs...>>();
}

template<typename ...TArgs>
inline void Event<TArgs...>::invoke(TArgs&&... args) {
	for (size_t i = 0; i < functions.size(); i++) {
		functions[i](args...);
	}
}
