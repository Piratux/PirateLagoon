#pragma once

#include "olcPixelGameEngine.h"

struct TileMapRaycastData {
public:
	olc::vf2d hit_pos = olc::vf2d{0.0f, 0.0f}; // hit pos in pixels
	olc::vi2d tile_idx = olc::vi2d{0, 0}; // index into tilemap of tile that was hit
	float distance = 0.0f; // distance travelled in pixels
	bool hit = false;
};

