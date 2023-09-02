#include "Events.h"

std::queue<Events::Type> Events::events;

void Events::add(Type type) {
	events.push(type);
}

Events::Type Events::get_next() {
	if (events.empty()) {
		return Type::NONE;
	}

	Type type = events.back();
	events.pop();
	return type;
}

bool Events::have_events() {
	return !events.empty();
}