#pragma once

#include <vector>
#include <functional>
#include <cassert>

template<typename ...TArgs>
class Event;

template<typename ...TArgs>
class EventTarget {
public:
	EventTarget();
	virtual ptrdiff_t getId() const;
	virtual void operator()(TArgs... args) = 0;

private:
	ptrdiff_t id = -1;
	inline static size_t event_target_id = 0;

};

template<typename ...TArgs>
class EventHandlerFunc : public EventTarget<TArgs...> {
public:
	EventHandlerFunc(const std::function<void(TArgs...)>& func);
	void operator()(TArgs... args) override;

private:
	std::function<void(TArgs...)> func;
};

template<typename ...TArgs>
class EventHandlerEvent : public EventTarget<TArgs...> {
public:
	EventHandlerEvent(const Event<TArgs...>& event);
	ptrdiff_t getId() const override;
	void operator()(TArgs... args) override;

private:
	Event<TArgs...>* event;
};

template<typename ...TArgs>
class Event : public EventTarget<TArgs...> {
public:
	void operator+=(const std::function<void(TArgs...)>& func);
	void operator+=(const Event<TArgs...>& event);
	void operator+=(const EventHandlerFunc<TArgs...>& handler);
	void operator-=(const EventTarget<TArgs...>& target);
	void operator()(TArgs... args);
	void clear();

private:
	std::vector<std::unique_ptr<EventTarget<TArgs...>>> targets;

};

template<typename ...TArgs>
inline void Event<TArgs...>::operator+=(const std::function<void(TArgs...)>& func) {
	std::unique_ptr<EventTarget<TArgs...>> uptr = std::make_unique<EventHandlerFunc<TArgs...>>(func);
	targets.push_back(std::move(uptr));
}

template<typename ...TArgs>
inline void Event<TArgs...>::operator+=(const Event<TArgs...>& event) {
	std::unique_ptr<EventTarget<TArgs...>> uptr = std::make_unique<EventHandlerEvent<TArgs...>>(event);
	targets.push_back(std::move(uptr));
}

template<typename ...TArgs>
inline void Event<TArgs...>::operator+=(const EventHandlerFunc<TArgs...>& handler) {
	std::unique_ptr<EventTarget<TArgs...>> uptr = std::make_unique<EventHandlerFunc<TArgs...>>(handler);
	targets.push_back(std::move(uptr));
}

template<typename ...TArgs>
inline void Event<TArgs...>::operator-=(const EventTarget<TArgs...>& target) {
	auto pred = [&](const std::unique_ptr<EventTarget<TArgs...>>& element) {
		return element->getId() == target.getId();
	};
	std::erase_if(targets, pred);
}

template<typename ...TArgs>
inline void Event<TArgs...>::operator()(TArgs ...args) {
	for (std::unique_ptr<EventTarget<TArgs...>>& target : targets) {
		EventTarget<TArgs...>* ptr = target.get();
		(*ptr)(args...);
	}
}

template<typename ...TArgs>
inline void Event<TArgs...>::clear() {
	targets = std::vector<std::unique_ptr<EventTarget<TArgs...>>>();
}

template<typename ...TArgs>
inline EventTarget<TArgs...>::EventTarget() {
	this->id = event_target_id++;
}

template<typename ...TArgs>
inline ptrdiff_t EventTarget<TArgs...>::getId() const {
	return id;
}


template<typename ...TArgs>
inline EventHandlerFunc<TArgs...>::EventHandlerFunc(const std::function<void(TArgs...)>& func) : EventTarget<TArgs...>() {
	this->func = func;
}

template<typename ...TArgs>
inline void EventHandlerFunc<TArgs...>::operator()(TArgs...args) {
	func(args...);
}

template<typename ...TArgs>
inline EventHandlerEvent<TArgs...>::EventHandlerEvent(const Event<TArgs...>& event) : EventTarget<TArgs...>() {
	this->event = const_cast<Event<TArgs...>*>(&event);
}

template<typename ...TArgs>
inline ptrdiff_t EventHandlerEvent<TArgs...>::getId() const {
	return event->getId();
}

template<typename ...TArgs>
inline void EventHandlerEvent<TArgs...>::operator()(TArgs...args) {
	Event<TArgs...>* ptr = this->event;
	(*ptr)(args...);
}
