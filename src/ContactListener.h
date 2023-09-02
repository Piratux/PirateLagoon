// This class defines what needs to be done when bodies collide

// What could be problem when bodies don't collide although they should:
// - Entity doesn't have "this->name = typeid(Type).name();" line
// - Collision callback doesn't match the desired objects in "AddCallback()"
// - Not enough collision flags
// - Collision flags aren't combined with "|" operator

// WARNING: Collision detection assumes, that if 2 objects collide, they're both valid (that is, their pointers are valid).

#pragma once

#include "PhysicsEngine.h"

class ContactListener : public b2ContactListener {
private:

public:
	ContactListener(PhysicsEngine* physics_engine);
	~ContactListener();

protected:
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;

private:
	void wall_vs_player_projectile_begin(uintptr_t wall, uintptr_t player_projectile);
	void wall_vs_enemy_projectile_begin(uintptr_t wall, uintptr_t enemy_projectile);
	void player_ship_vs_enemy_ship_begin(uintptr_t player_ship, uintptr_t enemy_ship);
	void player_ship_vs_enemy_projectile_begin(uintptr_t player_ship, uintptr_t enemy_projectile);
	void enemy_ship_vs_player_projectile_begin(uintptr_t enemy_ship, uintptr_t player_projectile);
	void player_ship_vs_wall_begin(uintptr_t player_ship, uintptr_t wall);
	void enemy_ship_vs_wall_begin(uintptr_t enemy_ship, uintptr_t wall);
	void player_projectile_vs_enemy_projectile_begin(uintptr_t player_projectile, uintptr_t enemy_projectile);

	void wall_vs_player_projectile_end(uintptr_t wall, uintptr_t player_projectile);
	void wall_vs_enemy_projectile_end(uintptr_t wall, uintptr_t enemy_projectile);
	void player_ship_vs_enemy_ship_end(uintptr_t player_ship, uintptr_t enemy_ship);
	void player_ship_vs_enemy_projectile_end(uintptr_t player_ship, uintptr_t enemy_projectile);
	void enemy_ship_vs_player_projectile_end(uintptr_t enemy_ship, uintptr_t player_projectile);
	void player_ship_vs_wall_end(uintptr_t player_ship, uintptr_t wall);
	void enemy_ship_vs_wall_end(uintptr_t enemy_ship, uintptr_t wall);
	void player_projectile_vs_enemy_projectile_end(uintptr_t player_projectile, uintptr_t enemy_projectile);

	void add_floating_text_particle(olc::vf2d pos, std::string text);
};
