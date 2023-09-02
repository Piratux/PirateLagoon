#pragma once
#include "GameState.h"

class GlobalGameState {
public:
	static GameState* game_state;

public:
	static void set(GameState* game_state);
	static GameState* get();
};
