#pragma once
#include "olcPixelGameEngine.h"
#include "AnimationFrame.h"

#include <map>

class Animation {
private:
	// assumes all frames are from same spritesheet
	std::vector<AnimationFrame> frames;
	int current_frame_index = 0;
	float current_frame_time = 0.0f;
	bool loop = true; // Animation frame index gets reset to 0 if true. Otherwise, finished_once is set to true
	bool finished_once = false;

public:
	// Convenience function. Overwrites frames.
	// - spritesheet_size: size of image that contains animation frames
	// - grid_size: { x: frame columns, y: frame rows }
	// - frame_indexes: indexes of frames. If empty, frame_indexes are automatically used as array taking all indexes from top left to bottom right, going row by row. (in 2x2 grid, that would be {0, 0}, {0, 1}, {1, 0}, {1, 1})
	// - animation_speed: animation frames per second
	void create(olc::Renderable* renderable, olc::vi2d spritesheet_size, olc::vi2d grid_size, std::vector<olc::vi2d> frame_indexes = {}, int animation_speed = 15);
	void add_frame(olc::Renderable* renderable, int x, int y, int width, int height, float frameTime);
	// if false, animation returns empty frame when animation finishes
	void set_looping(bool looping);
	void update(float delta);
	const AnimationFrame& get_frame();
	void reset();
	bool animation_finished_once();
};

