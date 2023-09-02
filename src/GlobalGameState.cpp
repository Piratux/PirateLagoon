#include "GlobalGameState.h"
#include "Utils.h"

GameState* GlobalGameState::game_state = nullptr;

void GlobalGameState::set(GameState* game_state) {
	Utils::check_null(game_state);
	GlobalGameState::game_state = game_state;
}

GameState* GlobalGameState::get() {
	Utils::check_null(GlobalGameState::game_state);
	return GlobalGameState::game_state;
}
