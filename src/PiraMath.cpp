#include "PiraMath.h"
#include <cmath>

const double PiraMath::PI = 3.141592653589793238463;
const float  PiraMath::PI_F = 3.14159265358979f;

float PiraMath::degrees_to_radians(float degrees) {
	return PiraMath::PI_F * degrees / 180.0f;
}

float PiraMath::radians_to_degrees(float radians) {
	return 180.0f * radians / PiraMath::PI_F;
}

olc::vf2d PiraMath::degrees_to_vec2(float degrees) {
	return radians_to_vec2(degrees_to_radians(degrees));
}

olc::vf2d PiraMath::radians_to_vec2(float radians) {
	return olc::vf2d{ std::sinf(radians), -std::cosf(radians) };
}

float PiraMath::vec2_to_degrees(olc::vf2d dir) {
	return radians_to_degrees(vec2_to_radians(dir));
}

float PiraMath::vec2_to_radians(olc::vf2d dir) {
	//dir = dir.norm();
	float angle_rad = std::atan2(-dir.y, -dir.x);
	if (angle_rad < 0.0f) {
		angle_rad += 2.0f * PiraMath::PI_F; // Ensure the angle is positive
	}
	angle_rad -= PiraMath::PI_F / 2.0f;

	angle_rad = std::fmodf(angle_rad, 2 * PiraMath::PI_F);
	if (angle_rad < 0.0f) {
		angle_rad += 2 * PiraMath::PI_F;
	}
	return angle_rad;
}

float PiraMath::distance(olc::vf2d pos1, olc::vf2d pos2) {
	return (pos1 - pos2).mag();
}

olc::vf2d PiraMath::vec2_from_to(olc::vf2d from, olc::vf2d to) {
	return to - from;
}

olc::vf2d PiraMath::vec2_rotate_degrees(olc::vf2d vec, float degrees) {
	return degrees_to_vec2(vec2_to_degrees(vec) + degrees);
}

olc::vf2d PiraMath::vec2_rotate_radians(olc::vf2d vec, float radians) {
	return radians_to_vec2(vec2_to_radians(vec) + radians);
}

float PiraMath::angle_between_vec2_degrees(olc::vf2d vec1, olc::vf2d vec2) {
	return radians_to_degrees(angle_between_vec2_radians(vec1, vec2));
}

float PiraMath::angle_between_vec2_radians(olc::vf2d vec1, olc::vf2d vec2) {
	return (float)sign(vec2_det(vec1, vec2)) * std::acos(vec2_dot(vec1, vec2));
}

float PiraMath::vec2_det(olc::vf2d vec1, olc::vf2d vec2) {
	vec1 = vec1.norm();
	vec2 = vec2.norm();
	return vec1.x * vec2.y - vec2.x * vec1.y;
}

float PiraMath::vec2_dot(olc::vf2d vec1, olc::vf2d vec2) {
	vec1 = vec1.norm();
	vec2 = vec2.norm();
	return vec1.x * vec2.x + vec1.y * vec2.y;
}

float PiraMath::lerp(float a, float b, float t) {
	return a + t * (b - a);
}