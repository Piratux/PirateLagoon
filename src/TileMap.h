#pragma once
#include "olcPixelGameEngine.h"
#include "PhysicsEngine.h"
#include "TileMapRaycastData.h"

// must be initialised before physics engine
// NOTE: assumes that same tilesheet is used for all maps
// NOTE: to avoid tile gap seems from appearing, tilesheet edges are extended and "Floor" layer
//  tiles are drawn slightly bigger (this solution is not ideal, because it assumes that floor 
//  layer alone has no transparent pixels or if it has, then that there are no tiles drawn on top that don't have transparency)\
//  TLDR: Make sure when using this class, to make Floor layer have no transparent pixels for no occasional visual tile gaps
class TileMap {
public:
	enum class Layer {
		Floor = 0,
		Floor2 = 1,
		Walls = 2,
		Objects = 3
	};

	enum class TileIdx {
		Water = 73,
		WhiteShip = 97, // used for player
		BlackShip = 98,
		YellowShip = 99,
		RedShip = 100,
		GreenShip = 101,
		BlueShip = 102
	};

private:
	bool initialised = false;
	bool tilesheet_with_extended_edges_created = false; // dirty hack to avoid recreating tilesheet with extended edges every time map is loaded
	bool map_loaded = false;
	olc::vi2d map_size; // in tiles
	olc::vi2d tile_count; // total tiles in x and y direction in tilesheet
	olc::vi2d tile_size; // in pixels

	int edge_length = 16; // the more the map can be zoomed out, the larger edge needs to be

	olc::Renderable tilesheet; // tilesheet with extended edges to avoid gaps
	olc::Renderable* original_tilesheet = nullptr;
	std::vector<int32_t> floor_layer;
	std::vector<int32_t> floor2_layer;
	std::vector<int32_t> walls_layer;
	std::vector<int32_t> objects_layer;

	// need to be stored, so that later it can be removed when another map is loaded
	std::vector<b2Body*> tile_walls;

public:
	void init(olc::Renderable* tilesheet);
	void update(float delta);
	void draw();
	void draw_layer(Layer layer);
	void load_map(int idx);

	olc::vi2d get_map_size();
	olc::vi2d get_tile_count();
	olc::vi2d get_tile_size();

	int32_t get_tile_idx(Layer layer, int idx_x, int idx_y);

	// start_pos: counted in pixels
	// max_distance: counted in pixels
	// return:
	// - distance will be max distance if ray didn't hit wall
	TileMapRaycastData raycast(olc::vf2d start_pos, olc::vf2d ray_dir, float max_distance);

private:
	void draw_tile(olc::vi2d tile_pos, int tile_idx, bool draw_extended_tile = false);

	std::vector<int32_t>& get_layer_data(Layer layer);
	int32_t get_tile_idx(const std::vector<int32_t>& layer, int idx_x, int idx_y);

	// Modifies tilesheet with extended tiles to avoid gaps between tiles
	void extend_tilesheet_edges();
	void set_map_wall_colliders();
};

