#include "Animation.h"
#include "GlobalTransformedView.h"

void Animation::create(olc::Renderable* renderable, olc::vi2d spritesheet_size, olc::vi2d grid_size, std::vector<olc::vi2d> frame_indexes, int animation_speed) {
	reset();

	olc::vi2d frame_size = spritesheet_size / grid_size;
	float frame_duration = 1.0f / (float)animation_speed;
	if (frame_indexes.size() == 0) {
		for (int y = 0; y < grid_size.y; y++) {
			for (int x = 0; x < grid_size.x; x++) {
				add_frame(
					renderable,
					frame_size.x * x,
					frame_size.y * y,
					frame_size.x,
					frame_size.y,
					frame_duration
				);
			}
		}
	}
	else {
		for (olc::vi2d idx_pos : frame_indexes) {
			add_frame(
				renderable,
				frame_size.x * idx_pos.x,
				frame_size.y * idx_pos.y,
				frame_size.x,
				frame_size.y,
				frame_duration
			);
		}
	}
}

void Animation::add_frame(olc::Renderable* renderable, int x, int y, int width, int height, float display_time_seconds) {
	frames.push_back(AnimationFrame{renderable, x, y, width, height, display_time_seconds});
}

void Animation::set_looping(bool looping) {
	loop = looping;
	if (loop == true) {
		finished_once = false;
		current_frame_index %= frames.size();
	}
}

void Animation::update(float delta) {
	if (finished_once == true) {
		return;
	}

	current_frame_time += delta;

	AnimationFrame& current_frame = frames[current_frame_index];

	while (current_frame_time >= current_frame.display_time_seconds) {
		current_frame_time -= current_frame.display_time_seconds;
		current_frame_index++;

		if (current_frame_index >= frames.size()) {
			if (loop == true) {
				current_frame_index = 0;
			}
			else {
				finished_once = true;
			}
		}
	}

	current_frame_index = std::clamp(current_frame_index, 0, (int)frames.size() - 1);
}

const AnimationFrame& Animation::get_frame() {
	if (finished_once) {
		return AnimationFrame::empty_frame;
	}

	return frames[current_frame_index];
}

void Animation::reset() {
	current_frame_index = 0;
	current_frame_time = 0.0f;
	finished_once = false;
}

bool Animation::animation_finished_once() {
	return finished_once;
}