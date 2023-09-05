#pragma once

#include "olcPixelGameEngine.h"
#include "EnumShipSide.h"
#include "PhysicsEngine.h"
#include "AnimationGroup.h"
#include "DataComponent.h"
#include "EnumShipType.h"

class ShipDataComponent final : public DataComponent {
public:
	const ShipType ship_type;

	float collider_radius = 50.0f;

	bool do_full_stop = false;

	float acceleration = 1.0f; // how much velocity is gained/lost a second
	float vel = 0.0f; // current velocity
	float min_vel = 0.0f; // preferably should always stay at 0
	float max_vel = 3.0f;
	float rotation_speed = 100.0f; // degrees rotated/second at maximum turn speed
	float min_rotation_scale = 0.5f; // ship rotation speed is multiplied by this scale when it doesn't move
	float max_rotation_scale = 1.0f; // ship rotation speed is multiplied by this scale when it moves at full speed

	int damage = 20;
	float cannon_strength = 5.0f; // force at which cannon ball is shot out
	float cannon_ball_size_scale = 1.0f; // scales cannon ball texture and hitbox
	float cannon_ball_pos_offset_scale = 30.0f; // pos offset telling how far from center cannon balls will spawn
	//float reload_timer_length_seconds = 1.0f;
	float reload_timer_length_seconds = 1.0f;
	float reload_timer_seconds = 0.0f;

	olc::vf2d pos = olc::vf2d{ 0.0f, 0.0f };

	b2Body* body = nullptr;
	olc::vf2d body_offset = olc::vf2d{ 0.0f, 0.0f };

	AnimationGroup animation_group;

	int max_health = 100;
	int health = max_health;

	bool death_handled = false;


	// used to indicate extra body and cannon ball size (used for black ship because it's bigger)
	olc::vf2d texture_scale = olc::vf2d{ 1.0f, 1.0f };

	// TODO: split this data into seperate components

	// -- AI DATA --
	// How far enemy can wander away before wanting to return
	float patrol_range = 1000; // from spawn position
	// How close enemy should come to its spawn home, before it starts targeting player again
	float retreat_range = 400; // from spawn position
	// How close player must be, for enemy to start combat against player (long range is need to stop player from simply outranging enemy)
	float attack_range = 1200; // from enemy position
	// How close enemy should get to player, before it starts turning its sides to him
	float combat_range = 400; // from enemy position
	// True while enemy is going back to its spawn position
	bool is_retreating = false;
	// Ship side that will be used to rotate ship when avoiding obstacles or navigating
	ShipSide ship_side = ShipSide::RIGHT;
	// Angle at which AI will re-calculate its rotation if it goes too far from its course
	float wiggle_degrees = 20.0f;
	// Angle at which AI will go straight if it has a waypoint or target to face
	float no_rotation_degrees = 5.0f;

	// Angle in degrees describing how big the cone is, that checks for obstacles/terrain in front of the ship (unused)
	float avoid_angle = 30; // currently, not used
	// How close enemy should be in range with obstacle, before it starts using obstacle avoidance.
	// Calculated using max speed and turn speed
	float avoid_range = 300;
	// How long ship will have to keep rotating after it avoided terrain
	float rotation_timer_seconds = 0;

	// After how many seconds dead ship will be automatically destroyed
	float dead_timer_seconds = 2.0f;
	olc::vf2d spawn_pos;

	ShipDataComponent* player = nullptr;

	bool just_stopped_avoiding_terrain = false; // shitty way to make sure that enemy doesn't get stuck on the wall turning left then right

	// blue ship AI data
	int cannonball_barrage_amount_total = 8;
	int cannonball_barrage_amount_left = 0;
	float cannonball_barrage_interval_seconds = 0.2f;
	float cannoball_barrage_cooldown_timer_seconds = 0.0f;

	// black ship AI data
	float black_ship_scale = 2.5f;

	// -- PLAYER DATA --

	float time_passed_since_death = 0.0f;

	// How long player has to wait after combat before starting to regenerate health
	float no_regeneration_timer_length_seconds = 15;
	// When combat_timer == 0, player starts regenerating health
	float no_regeneration_timer_seconds = no_regeneration_timer_length_seconds;
	// When combat_timer == 0, how often player will regenerate health
	float regeneration_speed_seconds = 1; // in seconds
	// How much of total hp player will regenerate every "regeneration_speed" seconds when out of combat
	float regeneration_percent = 10;
	// Damage done in percentages to player ship when hitting wall
	float ram_wall_damage_percentage = 10;
	// Damage done in percentages to both player and enemy ship when hitting enemy ship
	float ram_damage_percentage = 10;

	float no_ram_damage_timer_length_seconds = 0.25f;
	float no_ram_damage_timer_seconds = 0.0f;
	float ram_impact_lost_speed_percentage = 40.0f; // percentage of max velocity
	bool is_colliding = false;

	float nearest_enemy_arrow_alpha = 1.0f;
	bool show_arrow = true;

public:
	ShipDataComponent(olc::vf2d pos, ShipType ship_type);
	~ShipDataComponent() override;

	void rotate_ship(ShipSide ship_side, float delta);
	void set_velocity(float new_velocity);
	float get_velocity();
	void set_rotation_degrees(float degrees);
	float get_rotation_degrees();
	// pass negative number to de-accelerate
	void accelerate(float scale, float delta);

	void reset_regeneration_timer();
	void reset_reload_timer();
	void reset_no_ram_damage_timer();

	bool is_dead() {
		return health <= 0;
	}

	bool can_take_ram_damage() {
		return no_ram_damage_timer_seconds < 0.01f;
	}

	// returns true, if damage was dealt to ship
	// NOTE: Don't use this function to heal the ship
	bool take_damage(int amount) {
		if (is_dead())
			return false;

		if (amount <= 0)
			return false;

		health -= amount;
		health = std::max(health, 0);

		reset_regeneration_timer();

		return true;

		//game_manager.floating_texts.add(new FloatingText(game_manager.font,
		//    "-" + amount,
		//    new Vector2(get_pos().x, get_pos().y - get_curr_frame().getRegionHeight()),
		//    true)
		//);
	}
	// returns true, if damage was dealt to ship
	bool take_ram_damage(int amount) {
		if (can_take_ram_damage() == false) {
			return false;
		}

		set_velocity(get_velocity() - ram_impact_lost_speed_percentage * 0.01f * max_vel);

		reset_no_ram_damage_timer();
		return take_damage(amount);
	}
	int get_health() {
		return health;
	}
	int get_max_health() {
		return max_health;
	}
	// returns amount healed
	int heal(int amount) {
		if (amount <= 0)
			return 0;

		int amount_healed = std::min(amount, get_max_health() - health);

		health += amount_healed;
		return amount_healed;
	}
	void set_max_health(int new_max_health) {
		if (new_max_health <= 0) {
			return;
		}

		if (new_max_health < health) {
			health = new_max_health;
		}

		if (new_max_health > get_max_health()) {
			health += new_max_health - get_max_health();
		}

		max_health = new_max_health;
	}
	float get_health_percentage() {
		return (float)get_health() / get_max_health() * 100.0f;
	}
};