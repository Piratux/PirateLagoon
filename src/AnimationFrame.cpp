#include "AnimationFrame.h"

AnimationFrame AnimationFrame::empty_frame;

AnimationFrame::AnimationFrame() {

}

AnimationFrame::AnimationFrame(olc::Renderable* image, int x, int y, int width, int height, float display_time_seconds)
	: image(image)
	, source_pos(olc::vi2d{x, y})
	, source_size(olc::vi2d{width, height})
	, display_time_seconds(display_time_seconds) {
}