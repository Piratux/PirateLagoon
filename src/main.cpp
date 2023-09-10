// TODO CLEANUP:
// - check if camera shake works when zoomed in/out
// - when drawing don't convert from radians to degrees and back
// - use radians everywhere
// - figure out why 0 length vector passed to PiraMath functions and why bad stuff happen with .norm() there
// - add debug toggle (implement it as global setting)

// TODO possible updates:
// - boss bigger cannon ball (maybe one big and few small ones)
// - timer for each map?
// - special weather (winds that make it harder to go in certain direction)
// - buff green enemy damage (5 to 10 maybe)
// - mini map (you could find it for each map) (one of early enemy ships could drop it)
// - more interesting AI (yellow/red/green ships all look very same)
// - ship upgrades? (player special ability once a while) (special areas that spawn ship boost (this would have text on first one explaining how it works))
// - sound (AI cannon sound based on distance from player) (big ship big cannon sound)
// -- use audio from here https://github.com/justinrichardsmusic/PGEv2_Extensions/tree/master/Audio (or moros mini audio PGEX)
// - more map decoration (desert map, night map, decorations on shore and sea)
// - fortress that you have to destroy (with cannons that shoot (they follow your movement and try to shoot at you)) (maybe even in middle of sea)

#include "olcPixelGameEngine.h"
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
	bool draw_physics_debug = false;

	// NOTE: sizeof(PhysicsEngine) is quite big because of "b2World", so better keep it on heap
	std::unique_ptr<PhysicsEngine> physics_engine;
	std::unique_ptr<ContactListener> contact_listener;
	std::unique_ptr<GameManager> game_manager;

	olc::TransformedView transformed_view;
	AssetManager asset_manager;
	AudioEngine audio_engine;
	EntityManager entity_manager;
	ScreenFader screen_fader;
	GameState game_state;
	Camera2D camera;
	TileMap tile_map;

	// Screen "hijackers" that "override" OnUserUpdate()

	// Automatically shows splash screen.
	// Comment out to disable it (just splash screen).
	olc::SplashScreen splash_screen;
	std::unique_ptr<olc::MenuScreen> menu_screen;
	std::unique_ptr<olc::ComicScreen> comic_screen;

public:
	Game() {
		sAppName = "Pirate lagoon";
	}

	bool OnUserCreate() override {
		try {
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

			return true;
		}
		catch (std::exception e) {
			std::cout << e.what() << std::endl;
			return false;
		}
	}

	bool OnUserUpdate(float delta) override {
		try {
			// We don't want crazy stuff to happen with big or negative deltas (although preferable solution is fixed ticks for everything)
			delta = std::clamp(delta, 0.0f, 0.1f);
			accumulated_delta += delta;

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
				game_manager = std::make_unique<GameManager>(); // easy and robust way to soft reset it
				game_manager->init();
				break;

			default:
				// IMPORTANT: Physics should be updated before entities, because if physics body position is used in update and draw functions, the position lags 1 frame behind.
				// We want to keep stable timestep for physics, but we also don't wanna tie it to frame rate
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

	void temporary_update(float delta) {
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
};

int main() {
	Game game;
	if (game.Construct(960, 540, 1, 1, false, true, false))
		game.Start();
	return 0;
}