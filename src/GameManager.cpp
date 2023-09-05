#include "GameManager.h"
#include "ShipCodeComponent.h"
#include "ShipDataComponent.h"
#include "GlobalAssetManager.h"
#include "GlobalEntityManager.h"
#include "GlobalTransformedView.h"
#include "GlobalCamera2D.h"
#include "GlobalTileMap.h"
#include "GlobalPixelGameEngine.h"
#include "GlobalScreenFader.h"
#include "GlobalGameState.h"
#include "EntityManager.h"
#include "Events.h"

void GameManager::init() {
	AssetManager* asset_manager = GlobalAssetManager::get();

	set_default_camera_zoom();

	load_level(current_level);
	//load_level(11);
}

void GameManager::update(float delta) {
	olc::PixelGameEngine* pge = GlobalPixelGameEngine::get();
	ScreenFader* screen_fader = GlobalScreenFader::get();
	GameState* game_state = GlobalGameState::get();

	update_screen_fader(delta);
	update_events();

	switch (game_state->get()) {
	case GameState::State::PLAYER_IS_DEAD:
		if (pge->GetKey(olc::ENTER).bPressed) {
			reset_entities();
			game_state->set(GameState::State::ACTION);
		}
		break;

	case GameState::State::LEVEL_CLEARED_WAITING_FOR_ENTER:
		if (pge->GetKey(olc::ENTER).bPressed) {
			screen_fader->set_fade_out();
			game_state->set(GameState::State::LEVEL_CLEARED_FADING_OUT);
		}
		break;

	case GameState::State::ACTION:
		// last level
		if (current_level == total_levels && player_data_component->pos.x >= 2300) {
			screen_fader->set_fade_out();
			game_state->set(GameState::State::GAME_COMPLETED);
		}
		break;

	default:
		break;
	}

	// TEMP
	//if (pge->GetKey(olc::R).bPressed) {
	//	set_default_camera_zoom();
	//}
}

void GameManager::draw() {
	olc::PixelGameEngine* pge = GlobalPixelGameEngine::get();
	olc::TransformedView* transformed_view = GlobalTransformedView::get();
	GameState* game_state = GlobalGameState::get();

	olc::Pixel text_colour = olc::DARK_YELLOW;

	switch (game_state->get()) {
	case GameState::State::ACTION:
		if (current_level == 1) {
			transformed_view->DrawStringDecal(olc::vi2d{400, 1600}, "Use W/S or up/down\nto accelerate/de-accelerate", text_colour, olc::vf2d{5.0f, 5.0f});
			transformed_view->DrawStringDecal(olc::vi2d{450, 1100}, "Use A/D or left/right\nto steer ship left/right", text_colour, olc::vf2d{5.0f, 5.0f});
			transformed_view->DrawStringDecal(olc::vi2d{1300, 3000}, "Use K/L to use left/right cannon", text_colour, olc::vf2d{5.0f, 5.0f});
			transformed_view->DrawStringDecal(olc::vi2d{4400, 2400}, "Destroy all enemies\nto complete the level", text_colour, olc::vf2d{5.0f, 5.0f});
		}
		if (current_level == 2) {
			transformed_view->DrawStringDecal(olc::vi2d{400, 1400}, "Use SPACE to increase/decrease\nyour field of view", text_colour, olc::vf2d{5.0f, 5.0f});
			transformed_view->DrawStringDecal(olc::vi2d{2200, 3200}, "Arrow from your ship\nshows nearest enemy direction", text_colour, olc::vf2d{5.0f, 5.0f});
			transformed_view->DrawStringDecal(olc::vi2d{300, 4200}, "Tip: Staying out of combat for a while\nwill regenerate your health!", text_colour, olc::vf2d{5.0f, 5.0f});
		}
		if (very_first_enemy_killed && current_level != total_levels) {
			pge->DrawStringDecal(olc::vi2d{50, 10}, "Level: " + std::to_string(current_level) + "    Enemies left : " + std::to_string(total_enemies_left), olc::BLACK, olc::vf2d{3.0f, 3.0f});
		}
		break;

	case GameState::State::PLAYER_IS_DEAD:
		pge->DrawStringDecal(olc::vi2d{200, 400}, "You died\nPress ENTER to respawn", olc::VERY_DARK_RED, olc::vf2d{3.0f, 3.0f});
		break;

	case GameState::State::LEVEL_CLEARED_WAITING_FOR_ENTER:
		pge->DrawStringDecal(olc::vi2d{150, 400}, "Level " + std::to_string(current_level) + " cleared!\nPress ENTER to proceed", olc::DARK_GREEN, olc::vf2d{4.0f, 4.0f});
		break;

	default:
		break;
	}
}

void GameManager::create_ships() {
	TileMap* tile_map = GlobalTileMap::get();

	// Enemies need to know about player, but it's easiest to make that happen if player is created first.
	// Find and create player first.
	for (int x = 0; x < tile_map->get_map_size().x; x++) {
		for (int y = 0; y < tile_map->get_map_size().y; y++) {
			int tile_idx = tile_map->get_tile_idx(TileMap::Layer::Objects, x, y);
			if (tile_idx != 0) {
				olc::vi2d pos = olc::vi2d{ x * tile_map->get_tile_size().x, y * tile_map->get_tile_size().y };

				switch ((TileMap::TileIdx)tile_idx) {
				case TileMap::TileIdx::WhiteShip:
					create_player_ship(pos);
					break;
				default:
					break;
				}
			}
		}
	}


	// Create enemies
	for (int x = 0; x < tile_map->get_map_size().x; x++) {
		for (int y = 0; y < tile_map->get_map_size().y; y++) {
			int tile_idx = tile_map->get_tile_idx(TileMap::Layer::Objects, x, y);
			if (tile_idx != 0) {
				olc::vi2d pos = olc::vi2d{ x * tile_map->get_tile_size().x, y * tile_map->get_tile_size().y };

				switch ((TileMap::TileIdx)tile_idx) {
				case TileMap::TileIdx::YellowShip:
					create_enemy_ship(pos, ShipType::ENEMY_YELLOW);
					break;
				case TileMap::TileIdx::RedShip:
					create_enemy_ship(pos, ShipType::ENEMY_RED);
					break;
				case TileMap::TileIdx::GreenShip:
					create_enemy_ship(pos, ShipType::ENEMY_GREEN);
					break;
				case TileMap::TileIdx::BlueShip:
					create_enemy_ship(pos, ShipType::ENEMY_BLUE);
					break;
				case TileMap::TileIdx::BlackShip:
					create_enemy_ship(pos, ShipType::ENEMY_BLACK);
					break;
				default:
					break;
				}
			}
		}
	}
}

void GameManager::create_player_ship(olc::vf2d pos) {
	EntityManager* entity_manager = GlobalEntityManager::get();
	Camera2D* camera = GlobalCamera2D::get();

	Entity& entity = entity_manager->create_entity();

	ShipDataComponent& data_component = entity.add_data_component<ShipDataComponent>(pos, ShipType::PLAYER);
	ShipCodeComponent& code_component = entity.add_code_component<ShipCodeComponent>(data_component, data_component);

	player_data_component = &data_component;

	camera->set_follow_pos(&data_component.pos);
}

void GameManager::create_enemy_ship(olc::vf2d pos, ShipType ship_type) {
	EntityManager* entity_manager = GlobalEntityManager::get();

	Entity& entity = entity_manager->create_entity();

	ShipDataComponent& data_component = entity.add_data_component<ShipDataComponent>(pos, ship_type);
	ShipCodeComponent& code_component = entity.add_code_component<ShipCodeComponent>(data_component, *player_data_component);

	total_enemies_left++;
}

void GameManager::update_screen_fader(float delta) {
	ScreenFader* screen_fader = GlobalScreenFader::get();
	GameState* game_state = GlobalGameState::get();

	bool screen_fader_complete = screen_fader->update(delta);

	switch (game_state->get()) {
	case GameState::State::LEVEL_CLEARED_FADING_OUT:
		if (screen_fader_complete) {
			load_level(current_level + 1);
			screen_fader->set_fade_in();
			game_state->set(GameState::State::ACTION);
		}
		break;

	case GameState::State::GAME_COMPLETED:
		if (screen_fader_complete) {
			game_state->set(GameState::State::OUTRO_SCREEN);
		}
		break;

	default:
		break;
	}

	if (very_first_fade_in_queued == false) {
		screen_fader->set_fade_in();
		very_first_fade_in_queued = true;
	}
}

void GameManager::update_events() {
	GameState* game_state = GlobalGameState::get();

	while (Events::have_events()) {
		Events::Type type = Events::get_next();

		switch (type) {
		case Events::Type::ENEMY_DESTROYED:
			enemy_destroyed();
			break;

		case Events::Type::PLAYER_DESTROYED:
			game_state->set(GameState::State::PLAYER_IS_DEAD);
			break;

		default:
			break;
		}
	}
}

void GameManager::enemy_destroyed() {
	GameState* game_state = GlobalGameState::get();

	very_first_enemy_killed = true;
	total_enemies_left--;

	if (total_enemies_left < 0) {
		std::cout << "Error: negative amount of enemies left" << std::endl;
		exit(-1);
	}

	if (total_enemies_left > 0) {
		return;
	}

	if (game_state->get() == GameState::State::PLAYER_IS_DEAD) {
		return;
	}

	if (current_level == total_levels) {
		game_state->set(GameState::State::GAME_COMPLETED);
	}
	else {
		game_state->set(GameState::State::LEVEL_CLEARED_WAITING_FOR_ENTER);
	}
}

void GameManager::load_level(int level_idx) {
	TileMap* tile_map = GlobalTileMap::get();

	level_idx = std::clamp(level_idx, 1, total_levels);

	current_level = level_idx;

	tile_map->load_map(level_idx);
	set_camera_boundaries();

	reset_entities();
}

void GameManager::reset_entities() {
	EntityManager* entity_manager = GlobalEntityManager::get();

	total_enemies_left = 0;

	entity_manager->destroy_all_entities();
	create_ships();
}

void GameManager::set_camera_boundaries() {
	Camera2D* camera = GlobalCamera2D::get();
	TileMap* tile_map = GlobalTileMap::get();

	camera->set_min_pos(olc::vf2d{0, 0});
	camera->set_max_pos(tile_map->get_map_size() * tile_map->get_tile_size());
}

void GameManager::set_default_camera_zoom() {
	//olc::TransformedView* transformed_view = GlobalTransformedView::get();

	////olc::vf2d screen_middle = transformed_view->GetWorldTL() + transformed_view->GetWorldVisibleArea() / 2.0f;
	////transformed_view->ZoomAtScreenPos(0.6f, screen_middle);

	//transformed_view->SetWorldScale(olc::vf2d{0.6f, 0.6f});
	Camera2D* camera = GlobalCamera2D::get();
	if (camera->zoomed_out) {
		camera->toggle_zoom();
	}
}