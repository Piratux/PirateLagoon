#include "GlobalTileMap.h"
#include "Utils.h"

TileMap* GlobalTileMap::tile_map = nullptr;

void GlobalTileMap::set(TileMap* tile_map) {
	Utils::check_null(tile_map);
	GlobalTileMap::tile_map = tile_map;
}

TileMap* GlobalTileMap::get() {
	Utils::check_null(GlobalTileMap::tile_map);
	return GlobalTileMap::tile_map;
}
