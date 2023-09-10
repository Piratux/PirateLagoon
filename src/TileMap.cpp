#include "TileMap.h"
#include "GlobalPixelGameEngine.h"
#include "GlobalTransformedView.h"
#include "GlobalPhysicsEngine.h"
#include "EnumColliderFlag.h"

void TileMap::init(olc::Renderable* original_tilesheet) {
	initialised = true;
	this->original_tilesheet = original_tilesheet;
}

void TileMap::update(float delta) {
	return;
}

void TileMap::draw() {
	olc::TransformedView* transformed_view = GlobalTransformedView::get();

	transformed_view->DrawDecal(
		olc::vi2d{-1000, -1000},
		tilesheet.Decal()
	);

	draw_layer(Layer::Floor);
	draw_layer(Layer::Floor2);
}

void TileMap::draw_layer(Layer layer) {
	if (!(initialised && map_loaded)) {
		return;
	}

	olc::PixelGameEngine* pge = GlobalPixelGameEngine::get();
	olc::TransformedView* transformed_view = GlobalTransformedView::get();

	// calculate which part of map player can actually see
	olc::vf2d top_left = transformed_view->GetWorldTL();
	int width_start = (int)(top_left.x / (float)tile_size.x);
	int height_start = (int)(top_left.y / (float)tile_size.y);

	height_start = std::max(height_start, 0);
	width_start = std::max(width_start, 0);

	olc::vi2d camera_size;
	camera_size.x = (int)((float)pge->ScreenWidth() / transformed_view->GetWorldScale().x);
	camera_size.y = (int)((float)pge->ScreenHeight() / transformed_view->GetWorldScale().y);

	int height_end = height_start + 2 + camera_size.y / tile_size.y;
	int width_end = width_start + 2 + camera_size.x / tile_size.x;

	height_end = std::min(height_end, map_size.y);
	width_end = std::min(width_end, map_size.x);

	bool draw_extended_tile = layer == Layer::Floor;
	// iterate over the tilemap and draw each tile
	for (int height_idx = height_start; height_idx < height_end; ++height_idx) {
		for (int width_idx = width_start; width_idx < width_end; ++width_idx) {
			int index = get_tile_idx(layer, width_idx, height_idx);
			draw_tile(olc::vi2d{ width_idx* tile_size.x, height_idx* tile_size.y }, index, draw_extended_tile);
		}
	}
}

void TileMap::load_map(int idx) {
	if (initialised == false) {
		return;
	}

	// TODO: use json parser instead of this abomination
	std::cout << "Loading map data\n";

	std::string root = "./assets/maps/";

	// find "data" fields in .json
	std::string file_path = root + "level" + std::to_string(idx) + ".json";
	std::ifstream map_data(file_path);
	if (map_data.is_open() == false)
		throw std::logic_error("TileMap::load_map: couldn't open: " + file_path);

	floor_layer.clear();
	floor2_layer.clear();
	walls_layer.clear();
	objects_layer.clear();

	std::string temp_line;
	char total_data_fields_found = 0;
	bool search_for_width_and_height = true;
	while (std::getline(map_data, temp_line)) {
		if (search_for_width_and_height) {
			auto read_field = [](const std::string& line, int32_t& field) {
				std::stringstream ss(line);
				ss.ignore(100, ':');
				ss >> field;
			};

			if (temp_line.find("\"width\":") != std::string::npos)
				read_field(temp_line, map_size.x);
			else if (temp_line.find("\"height\":") != std::string::npos)
				read_field(temp_line, map_size.y);
			else if (temp_line.find("\"tilewidth\":") != std::string::npos)
				read_field(temp_line, tile_size.x);
			else if (temp_line.find("\"tileheight\":") != std::string::npos)
				read_field(temp_line, tile_size.y);
			else if (temp_line.find("\"layers\":") != std::string::npos)
				search_for_width_and_height = false;
			else if (temp_line.find("\"tilesets\":") != std::string::npos)
				search_for_width_and_height = false;
		}
		else {
			if (temp_line.find("}]") != std::string::npos)
				search_for_width_and_height = true;

			// if data field found read all the data into vector
			else if (temp_line.find("\"data\":") != std::string::npos) {
				// the order of data fields depends on order in ".json" file
				std::vector<int32_t>& layer_data = get_layer_data((Layer)total_data_fields_found);

				total_data_fields_found++;

				// prescale to avoid reallocations
				layer_data.reserve((size_t)map_size.x * (size_t)map_size.y);

				// reading part begins here
				std::stringstream ss(temp_line);
				ss.ignore(100, ':');
				while (ss.peek() != ']') {
					ss.ignore(1);

					// I have to read it into size_t first and then cast it, because 
					// stringstream doesn't read number into int32_t directly properly
					size_t tile_idx;
					ss >> tile_idx;
					layer_data.push_back((int32_t)tile_idx);
				}
			}
		}
	}

	if (map_size.x == 0)
		throw std::logic_error("TileMap::load_map: width field not found or it's 0");
	if (map_size.y == 0)
		throw std::logic_error("TileMap::load_map: height field not found or it's 0");
	if (tile_size.x == 0)
		throw std::logic_error("TileMap::load_map: tilewidth field not found or it's 0");
	if (tile_size.y == 0)
		throw std::logic_error("TileMap::load_map: tileheight field not found or it's 0");

	tile_count.x = original_tilesheet->Sprite()->width / tile_size.x;
	tile_count.y = original_tilesheet->Sprite()->height / tile_size.y;

	map_data.close();

	map_loaded = true;

	set_map_wall_colliders();
	extend_tilesheet_edges();
}

olc::vi2d TileMap::get_map_size() {
	return map_size;
}

olc::vi2d TileMap::get_tile_count() {
	return tile_count;
}

olc::vi2d TileMap::get_tile_size() {
	return tile_size;
}

int32_t TileMap::get_tile_idx(Layer layer, int idx_x, int idx_y) {
	return get_tile_idx(get_layer_data(layer), idx_x, idx_y);
}

TileMapRaycastData TileMap::raycast(olc::vf2d start_pos, olc::vf2d ray_dir, float max_distance) {
	// NOTE: this function assumes tile width == tile height
	// Thanks javidx9 https://www.youtube.com/watch?v=NbSee-XM7WA
	max_distance /= (float)tile_size.x;

	ray_dir = ray_dir.norm();

	// algorithm is implemented with tile_size being 1x1
	start_pos = olc::vi2d{ start_pos / olc::vf2d{tile_size} };

	olc::vf2d ray_unit_step_size = {
		sqrt(1 + (ray_dir.y / ray_dir.x) * (ray_dir.y / ray_dir.x)),
		sqrt(1 + (ray_dir.x / ray_dir.y) * (ray_dir.x / ray_dir.y))
	};
	olc::vi2d map_check = start_pos;
	olc::vf2d ray_length;
	olc::vi2d step;

	// Establish Starting Conditions
	if (ray_dir.x < 0) {
		step.x = -1;
		ray_length.x = (start_pos.x - float(map_check.x)) * ray_unit_step_size.x;
	}
	else {
		step.x = 1;
		ray_length.x = (float(map_check.x + 1) - start_pos.x) * ray_unit_step_size.x;
	}

	if (ray_dir.y < 0) {
		step.y = -1;
		ray_length.y = (start_pos.y - float(map_check.y)) * ray_unit_step_size.y;
	}
	else {
		step.y = 1;
		ray_length.y = (float(map_check.y + 1) - start_pos.y) * ray_unit_step_size.y;
	}


	float distance = 0.0f;
	TileMapRaycastData raycast_data;
	raycast_data.distance = max_distance;

	// Perform "Walk" until collision or range check
	while (distance < max_distance) {
		// Walk along shortest path
		if (ray_length.x < ray_length.y) {
			map_check.x += step.x;
			distance = ray_length.x;
			ray_length.x += ray_unit_step_size.x;
		}
		else {
			map_check.y += step.y;
			distance = ray_length.y;
			ray_length.y += ray_unit_step_size.y;
		}

		if (map_check.x < 0 || map_check.x >= map_size.x || map_check.y < 0 || map_check.y >= map_size.y) {
			return raycast_data;
		}

		// Test tile at new test point
		if (get_tile_idx(TileMap::Layer::Walls, map_check.x, map_check.y) != 0) {
			raycast_data.hit = true;
			raycast_data.hit_pos = (start_pos + ray_dir * distance) * tile_size.x;
			raycast_data.tile_idx = map_check;
			raycast_data.distance = distance;
			return raycast_data;
		}
	}

	return raycast_data;
}

void TileMap::draw_tile(olc::vi2d tile_pos, int tile_idx, bool draw_extended_tile) {
	// 0 means empty tile
	if (tile_idx == 0)
		return;

	// Read tile flip information
	// - 1st to last: horizontal flip bit
	// - 2nd to last: vertical flip bit
	// - 3rd to last: diagonal flip bit (from top left to bottom right) (or horizontal flip + left rotation)

	bool horizontal_flip = (tile_idx & 0x80000000);
	bool vertical_flip = (tile_idx & 0x40000000);
	bool diagonal_flip = (tile_idx & 0x20000000);

	// now that we got flip information, clear the flip bits
	tile_idx &= 0x1FFFFFFF;

	olc::vi2d tilesheet_pos;

	// -1 because indexes start from 1 here since 0 is reserved for empty tile
	tilesheet_pos.x = ((tile_idx - 1) % tile_count.x) * (tile_size.x + 2 * edge_length);
	tilesheet_pos.y = ((tile_idx - 1) / tile_count.x) * (tile_size.y + 2 * edge_length);

	// calculate tile rotation and flip
	olc::vf2d scale;
	float angle;
	if (diagonal_flip) {
		if (horizontal_flip && vertical_flip) {
			scale.x = 1.0f;
			scale.y = -1.0f;
			angle = -3.14f / 2.0f;
		}
		else if (!horizontal_flip && vertical_flip) {
			scale.x = 1.0f;
			scale.y = 1.0f;
			angle = -3.14f / 2.0f;
		}
		else if (horizontal_flip && !vertical_flip) {
			scale.x = 1.0f;
			scale.y = 1.0f;
			angle = 3.14f / 2.0f;
		}
		else if (!horizontal_flip && !vertical_flip) {
			scale.x = 1.0f;
			scale.y = -1.0f;
			angle = 3.14f / 2.0f;
		}
	}
	else {
		scale.x = horizontal_flip ? -1.0f : 1.0f;
		scale.y = vertical_flip ? -1.0f : 1.0f;
		angle = 0;
	}

	olc::TransformedView* transformed_view = GlobalTransformedView::get();
	olc::vi2d final_tile_size = tile_size;
	if (draw_extended_tile) {
		int extra_edge = std::min(2, edge_length); // make sure to not draw extended edge if tilesheet doesn't have it
		final_tile_size += olc::vi2d{ extra_edge, extra_edge };
	}

	transformed_view->DrawPartialRotatedDecal(
		tile_pos + tile_size / 2, // render pos
		tilesheet.Decal(), // decal ptr
		angle, // angle
		tile_size / 2, // rotation center (and also acts as -offset)
		tilesheet_pos + olc::vi2d{ edge_length, edge_length }, // source pos
		final_tile_size, // source size
		scale // scale
	);
}

std::vector<int32_t>& TileMap::get_layer_data(Layer layer) {
	switch (layer) {
	case Layer::Floor:
		return floor_layer;

	case Layer::Floor2:
		return floor2_layer;

	case Layer::Walls:
		return walls_layer;

	case Layer::Objects:
		return objects_layer;

	default: // should never happen
		throw std::logic_error("TileMap::get_layer: unknown layer");
		break;
	}
}

int32_t TileMap::get_tile_idx(const std::vector<int32_t>& layer, int idx_x, int idx_y) {
	return layer[map_size.x * idx_y + idx_x];
}

void TileMap::extend_tilesheet_edges() {
	// OpenGL (or PGE) for some reason doesn't precisely sample within given source rectangle,
	// so edges need to be extended to avoid line gaps

	if (tilesheet_with_extended_edges_created) {
		return;
	}

	tilesheet_with_extended_edges_created = true;

	tilesheet.Create(tile_count.x * (tile_size.x + 2 * edge_length), tile_count.y * (tile_size.y + 2 * edge_length));

	// Iterate over each tile in the original tilesheet and copy it to the extended tilesheet
	for (int y = 0; y < tile_count.y; y++) {
		for (int x = 0; x < tile_count.x; x++) {
			// Copy the tile from the original tilesheet to the extended tilesheet
			olc::Sprite* original_sheet = original_tilesheet->Decal()->sprite;
			olc::Sprite* extended_sheet = tilesheet.Decal()->sprite;

			int src_x = x * tile_size.x;
			int src_y = y * tile_size.y;

			int dst_x = x * (tile_size.x + 2 * edge_length) + edge_length;
			int dst_y = y * (tile_size.y + 2 * edge_length) + edge_length;

			// Copy the interior of the original tile
			for (int dy = 0; dy < tile_size.y; dy++) {
				for (int dx = 0; dx < tile_size.x; dx++) {
					olc::vi2d dst_pos = olc::vi2d{
						dst_x + dx,
						dst_y + dy
					};
					olc::vi2d src_pos = olc::vi2d{
						src_x + dx,
						src_y + dy
					};
					olc::Pixel pixel = original_sheet->GetPixel(src_pos);

					extended_sheet->SetPixel(dst_pos, pixel);
				}
			}

			// Extend left edge
			for (int dy = 0; dy < tile_size.y; dy++) {
				for (int dx = 0; dx < edge_length; dx++) {
					olc::vi2d dst_pos = olc::vi2d{
						dst_x - dx - 1,
						dst_y + dy
					};
					olc::vi2d src_pos = olc::vi2d{
						dst_x,
						dst_y + dy
					};
					olc::Pixel pixel = extended_sheet->GetPixel(src_pos);

					extended_sheet->SetPixel(dst_pos, pixel);
				}
			}

			// Extend right edge
			for (int dy = 0; dy < tile_size.y; dy++) {
				for (int dx = 0; dx < edge_length; dx++) {
					olc::vi2d dst_pos = olc::vi2d{
						dst_x + dx + tile_size.x,
						dst_y + dy
					};
					olc::vi2d src_pos = olc::vi2d{
						dst_x + tile_size.x - 1,
						dst_y + dy
					};
					olc::Pixel pixel = extended_sheet->GetPixel(src_pos);

					extended_sheet->SetPixel(dst_pos, pixel);
				}
			}

			// Extend top edge and top corners
			for (int dy = 0; dy < edge_length; dy++) {
				for (int dx = 0; dx < tile_size.x + 2 * edge_length; dx++) {
					olc::vi2d dst_pos = olc::vi2d{
						dst_x + dx - edge_length,
						dst_y - dy - 1
					};
					olc::vi2d src_pos = olc::vi2d{
						dst_x + dx - edge_length,
						dst_y
					};
					olc::Pixel pixel = extended_sheet->GetPixel(src_pos);

					extended_sheet->SetPixel(dst_pos, pixel);
				}
			}

			// Extend bottom edge and bottom corners
			for (int dy = 0; dy < edge_length; dy++) {
				for (int dx = 0; dx < tile_size.x + 2 * edge_length; dx++) {
					olc::vi2d dst_pos = olc::vi2d{
						dst_x + dx - edge_length,
						dst_y + dy + tile_size.y
					};
					olc::vi2d src_pos = olc::vi2d{
						dst_x + dx - edge_length,
						dst_y + tile_size.y - 1
					};
					olc::Pixel pixel = extended_sheet->GetPixel(src_pos);

					extended_sheet->SetPixel(dst_pos, pixel);
				}
			}
		}
	}

	tilesheet.Decal()->Update();
}

void TileMap::set_map_wall_colliders() {
	// TODO: make sure walls get optimised even if they're placed in a grid with no spaces
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	// destroy old walls
	for (b2Body* body : tile_walls) {
		physics_engine->destroy_body(body);
	}
	tile_walls.clear();

	auto create_tile_hitbox = [&](int32_t x_idx, int32_t y_idx, int32_t x_length, int32_t y_length, bool horizontal_hitbox) {
		olc::vi2d tile_pos{ x_idx, y_idx };
		tile_pos *= tile_size;

		olc::vi2d tile_offset{ tile_size / 2 };

		if (horizontal_hitbox)
			tile_offset *= olc::vi2d{ x_length, -1 };
		else
			tile_offset *= olc::vi2d{ -1, y_length };

		tile_pos -= tile_offset;

		b2Body* body = physics_engine->create_body(b2BodyType::b2_staticBody, static_cast<float>(tile_pos.x), static_cast<float>(tile_pos.y));
		tile_walls.push_back(body);

		ColliderData data;
		//data.user_data = reinterpret_cast<uintptr_t>(static_cast<GameObject*>(body));
		data.category = ColliderFlag::WALL;

		physics_engine->add_rectangle_collider(body, &data,
			static_cast<float>(tile_size.x * x_length),
			static_cast<float>(tile_size.y * y_length));
	};

	// chain length of wall tiles next to each other counted in tiles
	int32_t chain_length;
	int32_t height_idx, width_idx;

	// horizontal iteration
	for (height_idx = 0; height_idx < map_size.y; ++height_idx) {
		chain_length = 0;
		for (width_idx = 0; width_idx < map_size.x; ++width_idx) {
			int32_t index = map_size.x * height_idx + width_idx;

			// 0 means empty tile
			if (walls_layer[index] == 0) {
				if (chain_length > 0) {
					create_tile_hitbox(width_idx, height_idx, chain_length, 1, true);
					chain_length = 0;
				}
			}
			else {
				// if chain hasn't started
				if (chain_length == 0) {
					// if hitbox length would be > 1
					if (width_idx + 1 != map_size.x) {
						int32_t right_index = map_size.x * height_idx + width_idx + 1;
						if (walls_layer[right_index] != 0) {
							++chain_length;
							continue;
						}
					}

					// if there is a wall tile on top or on bottom from this tile, don't
					// create hitbox, because vertical iteration will create the hitbox
					bool top_or_bottom_tile_exists = false;

					if (height_idx != 0) {
						int32_t top_index = map_size.x * (height_idx - 1) + width_idx;
						if (walls_layer[top_index] != 0)
							top_or_bottom_tile_exists = true;
					}
					if (height_idx + 1 != map_size.y) {
						int32_t bottom_index = map_size.x * (height_idx + 1) + width_idx;
						if (walls_layer[bottom_index] != 0)
							top_or_bottom_tile_exists = true;
					}

					if (top_or_bottom_tile_exists == false)
						++chain_length;
				}
				else {
					++chain_length;
				}
			}
		}

		if (chain_length > 0) {
			create_tile_hitbox(width_idx, height_idx, chain_length, 1, true);
			chain_length = 0;
		}
	}

	// vertical iteration
	for (width_idx = 0; width_idx < map_size.x; ++width_idx) {
		chain_length = 0;
		for (height_idx = 0; height_idx < map_size.y; ++height_idx) {
			int32_t index = map_size.x * height_idx + width_idx;

			// 0 means empty tile
			if (walls_layer[index] == 0) {
				if (chain_length > 0) {
					create_tile_hitbox(width_idx, height_idx, 1, chain_length, false);
					chain_length = 0;
				}
			}
			else {
				// if chain hasn't started
				if (chain_length == 0) {
					// if hitbox length would be > 1
					if (height_idx + 1 != map_size.y) {
						int32_t bottom_index = map_size.x * (height_idx + 1) + width_idx;
						if (walls_layer[bottom_index] != 0)
							++chain_length;
					}
				}
				else {
					++chain_length;
				}
			}
		}

		if (chain_length > 0) {
			create_tile_hitbox(width_idx, height_idx, 1, chain_length, false);
			chain_length = 0;
		}
	}
}
