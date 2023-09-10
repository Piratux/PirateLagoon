#pragma once

#include "olcPixelGameEngine.h"
#include "GlobalScreenFader.h"

namespace olc {
	class MenuScreen : public olc::PGEX {
	public:
		MenuScreen(olc::Renderable* menu_image);
		void play_menu();

	protected:
		virtual void OnAfterUserCreate() override;
		virtual bool OnBeforeUserUpdate(float& fElapsedTime) override;

	private:
		enum class State {
			START,
			FADE_IN,
			FADE_OUT,
		};

	private:
		olc::Renderable* menu_image = nullptr;
		State state = State::START;
		bool complete = true;
	};
}

#ifdef PIRA_PGEX_MENUSCREEN
#undef PIRA_PGEX_MENUSCREEN

namespace olc {
	MenuScreen::MenuScreen(olc::Renderable* menu_image) : olc::PGEX(true), menu_image(menu_image) {
	}

	void MenuScreen::play_menu() {
		state = State::START;
		complete = false;
	}

	void MenuScreen::OnAfterUserCreate() {
	}

	bool MenuScreen::OnBeforeUserUpdate(float& delta) {
		if (complete) {
			return false;
		}

		delta = std::clamp(delta, 0.0f, 0.1f);

		ScreenFader* screen_fader = GlobalScreenFader::get();

		if (menu_image) {
			pge->DrawDecal(olc::vf2d{0, 0}, menu_image->Decal(), (olc::vf2d)pge->GetScreenSize() / (olc::vf2d)menu_image->Sprite()->Size());
		}

		bool fade_is_complete = screen_fader->update(delta);
		switch (state) {
		case State::START:
			screen_fader->set_fade_in();
			state = State::FADE_IN;
			break;

		case State::FADE_IN:
			screen_fader->update(delta);
			if (pge->GetKey(olc::ENTER).bPressed) {
				state = State::FADE_OUT;
				screen_fader->set_fade_out();
			}
			break;

		case State::FADE_OUT:
			if (fade_is_complete) {
				complete = true;
				return false;
			}
			break;

		default:
			break;
		}

		screen_fader->draw();

		return true;
	}
}

#endif