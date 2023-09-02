#pragma once
class GameState {
public:
	enum class State {
		MENU_SCREEN,
		INTRO_SCREEN,
		ACTION, // player is controlling his ship and battling enemies
		PLAYER_IS_DEAD,
		LEVEL_CLEARED_WAITING_FOR_ENTER,
		LEVEL_CLEARED_FADING_OUT,
		GAME_COMPLETED,
		OUTRO_SCREEN,
	};

private:
	State state = State::MENU_SCREEN;
	//State state = State::ACTION;

public:
	void set(State new_state);
	State get();
};

