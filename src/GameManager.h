#pragma once

#include "olcPixelGameEngine.h"
#include "EnumShipType.h"
#include "ShipDataComponent.h"

class GameManager {
private:
	ShipDataComponent* player_data_component = nullptr;

	int current_level = 1;
	int total_levels = 10;

	int total_enemies_left = 0;

	bool very_first_fade_in_queued = false;
	bool very_first_enemy_killed = false;

public:
	void init();
	void update(float delta);
	void draw();

private:
	void create_ships();
	void create_player_ship(olc::vf2d pos);
	void create_enemy_ship(olc::vf2d pos, ShipType ship_type);

	void update_screen_fader(float delta);
	void update_events();
	void enemy_destroyed();
	void load_level(int level_idx);
	void reset_entities();

	void set_camera_boundaries();
	void set_default_camera_zoom();
};

