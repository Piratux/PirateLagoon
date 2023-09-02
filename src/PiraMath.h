#pragma once

#include "olcPixelGameEngine.h"

class PiraMath {
public:
	static const double PI;
	static const float  PI_F;

public:
	static float degrees_to_radians(float degrees);
	static float radians_to_degrees(float radians);

	// returns normalised vec2 that rotates clockwise, where up is {x: 0, y: -1}
	//   0 degrees {x:  0, y: -1}
	//  90 degrees {x:  1, y:  0}
	// 180 degrees {x:  0, y:  1}
	// 270 degrees {x: -1, y:  0}
	static olc::vf2d degrees_to_vec2(float degrees);

	// returns normalised vec2 that rotates clockwise, where up is {x: 0, y: -1}
	// 0   PI radians {x:  0, y: -1}
	// 0.5 PI radians {x:  1, y:  0}
	// 1   PI radians {x:  0, y:  1}
	// 1.5 PI radians {x: -1, y:  0}
	static olc::vf2d radians_to_vec2(float radians);

	// returns degrees in range [0, 360] from direction vector that rotates clockwise, where up is 0 degrees.
	//   0 degrees {x:  0, y: -1}
	//  90 degrees {x:  1, y:  0}
	// 180 degrees {x:  0, y:  1}
	// 270 degrees {x: -1, y:  0}
	static float vec2_to_degrees(olc::vf2d dir);

	// returns radians in range [0, 2PI] from direction vector that rotates clockwise, where up is 0 radians.
	// 0   PI radians {x:  0, y: -1}
	// 0.5 PI radians {x:  1, y:  0}
	// 1   PI radians {x:  0, y:  1}
	// 1.5 PI radians {x: -1, y:  0}
	static float vec2_to_radians(olc::vf2d dir);

	// returns distance between 2 points
	static float distance(olc::vf2d pos1, olc::vf2d pos2);

	// returns vector that points from "from" to "to"
	static olc::vf2d vec2_from_to(olc::vf2d from, olc::vf2d to);

	// returns "vec" clockwise by "degrees"
	static olc::vf2d vec2_rotate_degrees(olc::vf2d vec, float degrees);

	// returns "vec" clockwise by "radians"
	static olc::vf2d vec2_rotate_radians(olc::vf2d vec, float radians);

	// Calculates determinant of 2 vectors
	// Returned number is
	// - positive: vec1 is on right side of vec2
	// - negative: vec1 is on left side of vec2
	// - zero: vec1 and vec2 are collinear
	static float vec2_det(olc::vf2d vec1, olc::vf2d vec2);

	// Calculates dot product of 2 vectors
	// Returned number is
	// - positive: smallest angle between vec1 and vec2 is less than 90 degrees
	// - negative: smallest angle between vec1 and vec2 is greater than 90 degrees
	// - zero: angle between vec1 and vec2 is 90 degrees
	static float vec2_dot(olc::vf2d vec1, olc::vf2d vec2);

	// returns -1, 0 or 1, depending on sign
	// taken from https://stackoverflow.com/a/4609795
	template <typename T>
	static int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}
};

