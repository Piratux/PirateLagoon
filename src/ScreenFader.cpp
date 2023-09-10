#include "ScreenFader.h"
#include "GlobalPixelGameEngine.h"

bool ScreenFader::update(float delta) {
	if (complete) {
		return true;
	}

	switch (state) {
	case State::SCREEN_FADE_IN_DELAY:
		fade_in_delay_timer_seconds += delta;
		if (fade_in_delay_timer_seconds >= fade_in_delay_timer_length_seconds) {
			state = State::SCREEN_FADE_IN;
		}
		break;

	case State::SCREEN_FADE_IN:
		fade_in_timer_seconds += delta;
		if (fade_in_timer_seconds >= fade_in_timer_length_seconds) {
			complete = true;
		}
		break;

	case State::SCREEN_FADE_OUT:
		fade_out_timer_seconds += delta;
		if (fade_out_timer_seconds >= fade_out_timer_length_seconds) {
			state = State::SCREEN_FADE_OUT_DELAY;
		}
		break;

	case State::SCREEN_FADE_OUT_DELAY:
		fade_out_delay_timer_seconds += delta;
		if (fade_out_delay_timer_seconds >= fade_out_delay_timer_length_seconds) {
			complete = true;
		}
		break;
	}

	return complete;
}

void ScreenFader::draw() {
	switch (state) {
	case State::SCREEN_FADE_IN_DELAY:
		draw_transparent_rect(0.0f);
		break;

	case State::SCREEN_FADE_IN:
		draw_transparent_rect(fade_in_timer_seconds / fade_in_timer_length_seconds);
		break;

	case State::SCREEN_FADE_OUT:
		draw_transparent_rect(1.0f - (fade_out_timer_seconds / fade_out_timer_length_seconds));
		break;

	case State::SCREEN_FADE_OUT_DELAY:
		draw_transparent_rect(0.0f);
		break;
	}
}

void ScreenFader::set_fade_in() {
	state = State::SCREEN_FADE_IN_DELAY;
	fade_in_delay_timer_seconds = 0.0f;
	fade_in_timer_seconds = 0.0f;
	complete = false;
}

void ScreenFader::set_fade_out() {
	state = State::SCREEN_FADE_OUT;
	fade_out_delay_timer_seconds = 0.0f;
	fade_out_timer_seconds = 0.0f;
	complete = false;
}

void ScreenFader::draw_transparent_rect(float t) {
	olc::PixelGameEngine* pge = GlobalPixelGameEngine::get();

	olc::Pixel color = screen_colour;
	t = std::clamp(t, 0.0f, 1.0f);
	color.a = (uint8_t)((1.0f - t) * 255.0f);
	pge->FillRectDecal(olc::vf2d{0, 0}, pge->GetScreenSize(), color);
};