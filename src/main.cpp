// TODO:
// + shooting
// + enemy taking damage
// + explosion when enemy dies
// + dying
// + floating damage text
// + level loading
// + ship explosion particle
// + projectile trail particle
// + colliding damage
// + camera shake from ramming
// + explanation text
// + ship slowing down upon collision
// + AI moving
// + AI shooting
// + comic integration
// + add credits
// + add health regeneration (when out of combat)
// + particles

// TODO CLEANUP:
// - when drawing don't convert from radians to degrees and back
// - use radians everywhere

// TODO possible updates:
// - sound (AI cannon sound based on distance from player) (big ship big cannon sound)
// - desert map
// - more map decoration
// - fortress that you have to destroy (with cannons that shoot (they follow your movement and try to shoot at you))

#include "olcPixelGameEngine.h"
#include "olcSoundWaveEngine.h"
#include "olcPGEX_SplashScreen.h"
#include "olcPGEX_TransformedView.h"
#include "piraPGEX_MenuScreen.h"
#include "tutasPGEX_ComicArt.h"

#include "Animation.h"
#include "GameManager.h"
#include "GlobalPixelGameEngine.h"
#include "GlobalTransformedView.h"
#include "GlobalAssetManager.h"
#include "GlobalPhysicsEngine.h"
#include "GlobalAudioEngine.h"
#include "GlobalEntityManager.h"
#include "GlobalCamera2D.h"
#include "GlobalTileMap.h"
#include "GlobalScreenFader.h"
#include "GlobalGameState.h"
#include "ContactListener.h"
#include "PiraMath.h"

class Game : public olc::PixelGameEngine {
public:
	float total_time = 0.0f;
	float physics_time_step = 1.0f / 60.0f;
	float accumulated_delta = 0.0f;

	AssetManager asset_manager;

	olc::TransformedView transformed_view;

	std::unique_ptr<PhysicsEngine> physics_engine;
	std::unique_ptr<ContactListener> contact_listener;
	AudioEngine audio_engine;
	std::unique_ptr<GameManager> game_manager;
	EntityManager entity_manager;
	ScreenFader screen_fader;
	GameState game_state;

	Camera2D camera;
	TileMap tile_map;

	bool draw_physics_debug = false;

	// Automatically shows splash screen.
	// Comment out to disable it.
	olc::SplashScreen splash_screen;

	std::unique_ptr<olc::MenuScreen> menu_screen;
	std::unique_ptr<olc::ComicScreen> comic_screen;

	Game() {
		sAppName = "Pirate lagoon";
	}

public:
	bool OnUserCreate() override {
		try {
			//std::cout << sizeof(uintptr_t) << std::endl;
			register_globals();


			transformed_view.Initialise(this->GetScreenSize());
			asset_manager.init();
			physics_engine->init(physics_time_step);
			tile_map.init(asset_manager.get_renderable(ImageName::TILESHEET));

			comic_screen = std::make_unique<olc::ComicScreen>(
				asset_manager.get_renderable(ImageName::INTRO_SCREEN),
				asset_manager.get_renderable(ImageName::OUTRO_SCREEN)
			);

			game_manager = std::make_unique<GameManager>();
			game_manager->init();

			contact_listener = std::make_unique<ContactListener>(physics_engine.get());

			menu_screen = std::make_unique<olc::MenuScreen>(asset_manager.get_renderable(ImageName::MENU_SCREEN));

			//temporary_init();
			return true;
		}
		catch (std::exception e) {
			std::cout << e.what() << std::endl;
			return false;
		}
	}

	bool OnUserUpdate(float delta) override {
		try {
			switch (game_state.get()) {
			case GameState::State::MENU_SCREEN:
				menu_screen->play_menu();
				game_state.set(GameState::State::INTRO_SCREEN);
				break;

			case GameState::State::INTRO_SCREEN:
				comic_screen->startIntro();
				game_state.set(GameState::State::ACTION);
				break;

			case GameState::State::OUTRO_SCREEN:
				comic_screen->startEnding();
				game_state.set(GameState::State::MENU_SCREEN);
				game_manager = std::make_unique<GameManager>();
				game_manager->init();
				break;

			default:
				//audio_engine.play(AudioName::CANNON_BLAST);
				//Clear(olc::BLACK);

				// We don't want crazy stuff to happen with big and negative deltas (especially physics engine getting stuck)
				delta = std::clamp(delta, 0.0f, 0.1f);
				accumulated_delta += delta;

				// IMPORTANT: physics should be updated before entities, because if physics body position is used in update and draw functions, the position lags 1 frame behind
				// we want to keep stable timestep for physics, but we also don't wanna tie it to frame rate
				while (accumulated_delta >= physics_time_step) {
					accumulated_delta -= physics_time_step;
					physics_engine->update();
				}

				entity_manager.update(delta);
				game_manager->update(delta);
				tile_map.update(delta);
				camera.update(delta);


				tile_map.draw();
				camera.draw();
				entity_manager.draw();
				game_manager->draw();

				if (draw_physics_debug) {
					physics_engine->draw();
				}

				// TEMP
				//temporary_update(delta);

				// temporary, since emscripten has this by default
				//if (GetKey(olc::ESCAPE).bPressed) {
				//	return false;
				//}

				break;
			}

			screen_fader.draw();

			return true;
		}
		catch (std::exception e) {
			std::cout << e.what() << std::endl;
			return false;
		}
	}

	void temporary_init() {
		//for (int degrees = -720; degrees <= 720; degrees += 45) {
		//	std::cout << degrees << " " << PiraMath::degrees_to_vec2(degrees) << " " << PiraMath::vec2_to_degrees(PiraMath::degrees_to_vec2(degrees)) << std::endl;
		//	float radians = PiraMath::degrees_to_radians(degrees);
		//	std::cout << radians << " " << PiraMath::radians_to_vec2(radians) << " " << PiraMath::vec2_to_radians(PiraMath::radians_to_vec2(radians)) << std::endl;
		//	std::cout << std::endl;
		//}
		//std::cout << "AssetManager size: " << sizeof(AssetManager) << std::endl;
		//std::cout << "AudioEngine size: " << sizeof(AudioEngine) << std::endl;
		//std::cout << "Animation size: " << sizeof(Animation) << std::endl;
		//std::cout << "TransformedView size: " << sizeof(olc::TransformedView) << std::endl;
		//std::cout << "PhysicsEngine size: " << sizeof(PhysicsEngine) << std::endl;
		//std::cout << "GameManager size: " << sizeof(GameManager) << std::endl;
		//std::cout << "PixelGameEngine size: " << sizeof(olc::PixelGameEngine) << std::endl;
	}

	void temporary_update(float delta) {
		//draw_background(delta);

		transformed_view.HandlePanAndZoom(0);

		// TEMP
		//int fps = (int)(1.0f / delta);
		//DrawStringDecal(olc::vi2d{10, 10}, "FPS: " + std::to_string(fps), olc::RED);

		if (GetKey(olc::SPACE).bPressed) {
			draw_physics_debug = !draw_physics_debug;
		}
	}

	void register_globals() {
		GlobalPixelGameEngine::set(this);
		GlobalTransformedView::set(&transformed_view);
		GlobalAssetManager::set(&asset_manager);

		physics_engine = std::make_unique<PhysicsEngine>();
		GlobalPhysicsEngine::set(physics_engine.get());

		GlobalAudioEngine::set(&audio_engine);
		GlobalEntityManager::set(&entity_manager);
		GlobalCamera2D::set(&camera);
		GlobalTileMap::set(&tile_map);
		GlobalScreenFader::set(&screen_fader);
		GlobalGameState::set(&game_state);
	}

	void draw_background(float delta) {
		total_time += delta * 50;
		uint8_t time = (uint8_t)total_time;
		for (int x = 0; x < ScreenWidth(); x++) {
			for (int y = 0; y < ScreenHeight(); y++) {
				uint8_t R = (time + x + y) % 256;
				uint8_t G = 0;
				uint8_t B = 255 - (time - x - y) % 256;
				Draw(x, y, olc::Pixel(R, G, B));
			}
		}
	}
};

int main() {
	Game game;
	if (game.Construct(960, 540, 1, 1, false, true, false))
		game.Start();
	return 0;
}