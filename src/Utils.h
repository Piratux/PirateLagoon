#pragma once

#include "PhysicsEngine.h"

class Utils {
public:
	static void check_null(void* ptr);

	static olc::vf2d b2Vec2_to_vf2d(b2Vec2 vec2);
	static b2Vec2 vf2d_to_b2Vec2(olc::vf2d vec2);

	// Returns random int in range [0, to]
	static int random_int(int to = 1);

	// Returns random int in range [from, to]
	static int random_int(int from, int to);

	// Returns random float in range [0.0f, to]
	static float random_float(float to = 1.0f);

	// Returns random float in range [from, to]
	static float random_float(float from, float to);

	// Returns random normalised direction in radian range [from, to]
	static olc::vf2d random_direction(float from = 0.0f, float to = 2 * b2_pi);

	static void draw_cirlce(olc::vf2d center, float radius, olc::Pixel color, int polygon_edges = 20);
};

