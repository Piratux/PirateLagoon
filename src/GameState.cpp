#include "GameState.h"

void GameState::set(State new_state) {
	state = new_state;
}

GameState::State GameState::get() {
	return state;
}