#pragma once

#include "olcPixelGameEngine.h"

class GlobalPixelGameEngine {
public:
	static olc::PixelGameEngine* pixel_game_engine;

public:
	static void set(olc::PixelGameEngine* pixel_game_engine);
	static olc::PixelGameEngine* get();
};

