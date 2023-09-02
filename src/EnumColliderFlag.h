#pragma once

#include <cstdint>

enum ColliderFlag : uint16_t {
	PLAYER_SHIP = 1 << 0,
	ENEMY_SHIP = 1 << 1,
	PLAYER_PROJECTILE = 1 << 2,
	ENEMY_PROJECTILE = 1 << 3,
	WALL = 1 << 4,
	CREW_PARTICLE = 1 << 5,
};