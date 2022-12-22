#pragma once
#include <vector>
#include <functional>
#include <string>
#include <memory>

template<typename... T>
class GameAction {
public:
	GameAction() {}
	~GameAction() { events.clear(); }

	void operator+=(std::function<void(T...)> func) {
		events.emplace_back(func);
	}

	std::vector<std::function<void(T...)>> events;
};

template<typename... T>
class GameEvent {
public:
	GameEvent() {}
	~GameEvent() {}

	void AddListener(GameAction<T...>* action_) {
		action.reset(action_);
	}

	void Invoke(T... parameters) {
		for (int i = 0; i < action->events.size(); ++i) {
			action->events[i](parameters...);
		}
	}
private:
	std::shared_ptr<GameAction<T...>> action;
};