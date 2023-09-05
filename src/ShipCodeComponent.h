#pragma once

#include "CodeComponent.h"
#include "ShipDataComponent.h"

class ShipCodeComponent final : public CodeComponent {
private:
	ShipDataComponent& data_component;
	ShipDataComponent& player_data_component;

public:
	ShipCodeComponent(ShipDataComponent& data_component, ShipDataComponent& player_data_component);

	bool update(float delta) override;
	void draw() override;

private:
	void update_closest_enemy_arrow(float delta);
	void draw_closest_enemy_direction_arrow();
	olc::vf2d get_closest_enemy_pos();
	bool update_as_player_ship(float delta);
	bool update_as_enemy_ship(float delta);
	// generic enemy ship movement
	void update_movement_as_enemy_ship(float delta);
	// generic enemy ship cannon control
	void update_cannons_as_enemy_ship(float delta);
	void update_cannons_as_blue_ship(float delta);
	void update_cannons_as_black_ship(float delta);

	void shoot_as_player_ship(ShipSide ship_side);
	void shoot_as_enemy_ship(ShipSide ship_side);

	void add_death_explosion();
	void add_flying_crew_members();

	bool can_shoot();
	void update_timers(float delta);
	void update_animation();
	void shoot_cannon_ball(ShipSide ship_side, ColliderData& ball_collider_data);
	bool obstacles_in_front(olc::vf2d dir, float cone_angle_degrees);
	bool terrain_in_front(olc::vf2d dir, float cone_angle_degrees);
	void avoid_obstacles(bool switch_sides = true);
	void use_pathfinding(float delta);
	float calculate_rotation_timer();

	void ship_just_died();

	// returns side which is closer to player (used to determine which cannon side to use for enemy)
	ShipSide get_player_side();
};