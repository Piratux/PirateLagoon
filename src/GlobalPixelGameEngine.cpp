#include "GlobalPixelGameEngine.h"
#include "Utils.h"

olc::PixelGameEngine* GlobalPixelGameEngine::pixel_game_engine = nullptr;

void GlobalPixelGameEngine::set(olc::PixelGameEngine* pixel_game_engine) {
	Utils::check_null(pixel_game_engine);
	GlobalPixelGameEngine::pixel_game_engine = pixel_game_engine;
}

olc::PixelGameEngine* GlobalPixelGameEngine::get() {
	Utils::check_null(GlobalPixelGameEngine::pixel_game_engine);
	return GlobalPixelGameEngine::pixel_game_engine;
}