#pragma once

#include "olcPixelGameEngine.h"
#include "GlobalPixelGameEngine.h"

class ScreenFader {
private:
	enum class State {
		SCREEN_FADE_IN_DELAY,
		SCREEN_FADE_IN,
		SCREEN_FADE_OUT_DELAY,
		SCREEN_FADE_OUT,
	};

	olc::Pixel screen_colour = olc::BLACK;
	State state = State::SCREEN_FADE_IN_DELAY;
	float fade_in_delay_timer_length_seconds = 1.0f; // how long screen will stay in "screen_colour" before fading in
	float fade_in_delay_timer_seconds = 0.0f;
	float fade_out_delay_timer_length_seconds = 1.0f; // how long screen will stay in "screen_colour" after fading out
	float fade_out_delay_timer_seconds = 0.0f;
	float fade_in_timer_length_seconds = 2.0f; // how long it takes to fade into menu
	float fade_in_timer_seconds = 0.0f;
	float fade_out_timer_length_seconds = 2.0f; // how long it takes to fade out of menu
	float fade_out_timer_seconds = 0.0f;
	bool complete = true;

public:
	// draws fade in/out rectangle on screen
	// returns true if fading in/out is finished
	bool update(float delta);
	void draw();

	void set_fade_in();
	void set_fade_out();

private:
	// t - 0 no transparency
	// t - 1 fully transparent
	void draw_transparent_rect(float t);

};

