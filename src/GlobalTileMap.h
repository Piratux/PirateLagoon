#pragma once
#include "TileMap.h"

class GlobalTileMap {
public:
	static TileMap* tile_map;

public:
	static void set(TileMap* tile_map);
	static TileMap* get();
};
