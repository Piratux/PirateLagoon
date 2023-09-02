#pragma once

#include "olcPixelGameEngine.h"

struct AnimationFrame {
public:
	olc::Renderable* image;
	olc::vf2d source_pos; // position of sprite in the texture.
	olc::vf2d source_size; // sprite dimensions
	float display_time_seconds; // How long to display the frame.

	AnimationFrame();
	AnimationFrame(olc::Renderable* image, int x, int y, int width, int height, float display_time_seconds);

	static AnimationFrame empty_frame;
};

