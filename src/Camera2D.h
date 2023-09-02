#pragma once

#include "olcPixelGameEngine.h"

#include <limits>

class Camera2D {
public:
	float lerp_speed = 0.2f; // position follow delay (1 means no delay)
	olc::vf2d* follow_pos = nullptr; // middle of position of object being followed
	olc::vf2d min_pos = olc::vf2d{std::numeric_limits<float>::min(), std::numeric_limits<float>::min()};
	olc::vf2d max_pos = olc::vf2d{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};

	olc::vf2d shake_offset;
	float shake_duration = 0.0f;
	float shake_magnitude = 1.0f;

public:
	void update(float delta);
	void draw();

	void set_follow_pos(olc::vf2d* follow_pos);
	void set_min_pos(olc::vf2d min_pos);
	void set_max_pos(olc::vf2d max_pos);

	void shake(olc::vf2d offset, float duration, float magnitude);
	void small_shake(olc::vf2d offset);
	void big_shake(olc::vf2d offset);

private:
	void apply_shake(olc::vf2d& pos, float delta);
};