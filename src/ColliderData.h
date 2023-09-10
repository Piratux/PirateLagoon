#pragma once

#include <cstdint>

// Used to pack data for collider creation functions in physics engine
struct ColliderData {
public:
	uintptr_t user_data = 0; // Can be used to store custom data to identify entity that this body belongs to
	float offset_x = 0.0f; // Collider position offset in x direction from body center
	float offset_y = 0.0f; // Collider position offset in y direction from body center
	float friction = 0.0f;
	uint16_t category = 0x0001; // Usually a single bit that represents collision group
	uint16_t mask = 0xFFFF; // Bits that tell with what group it can collide with
	bool is_sensor = false; // If true, the collider doesn't collide, but still calls "BeginContact()"
};