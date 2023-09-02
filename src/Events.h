#pragma once

#include <queue>

class Events {
public:
	enum class Type {
		NONE,
		ENEMY_DESTROYED,
		PLAYER_DESTROYED,
	};

private:
	static std::queue<Type> events;

public:
	static void add(Type type);
	static Type get_next(); // if no events left, NONE is returned
	static bool have_events(); // returnes true if there are events left
};

