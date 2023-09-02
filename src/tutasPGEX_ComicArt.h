#pragma once

#include "olcPixelGameEngine.h"

namespace olc {
	class ComicScreen : public olc::PGEX {
	public:
		ComicScreen(olc::Renderable* intro_screen, olc::Renderable* outro_screen);
		bool isRunning = false;
		int step = 0;
		bool isEnding = false;
		void startIntro();
		void startEnding();

	protected:
		int getProgress(float mult = 1.0);
		virtual void OnAfterUserCreate() override;
		virtual bool OnBeforeUserUpdate(float& fElapsedTime) override;

	private:
		olc::Renderable* bg;
		olc::Renderable* bgED;
		std::vector<std::pair<olc::vf2d, olc::vf2d>> vBoom;
		olc::vf2d vScale;
		olc::vf2d vPosition;
		float fDeltaTime = 0.0f;
		float fAspect = 0.0f;
		olc::vf2d steps[8] = {olc::vf2d(0, 0), olc::vf2d(-570, 0), olc::vf2d(-570, 0), olc::vf2d(0, -550), olc::vf2d(-570, -550), olc::vf2d(-320, -550 * 3 + 370), olc::vf2d(-320, -550 * 4 + 200), olc::vf2d(-320, -550 * 4 + 200)};
		olc::vf2d stepsED[5] = {olc::vf2d(0, -10), olc::vf2d(-570, 0), olc::vf2d(-320, -670), olc::vf2d(-320, -1290), olc::vf2d(-320, -550 * 4 + 220)};
	};


}

#ifdef OLC_PGEX_COMICSCREEN
#undef OLC_PGEX_COMICSCREEN

namespace olc {
	ComicScreen::ComicScreen(olc::Renderable* intro_screen, olc::Renderable* outro_screen)
		: bg(intro_screen)
		, bgED(outro_screen)
		, olc::PGEX(true) {
	}

	void ComicScreen::startIntro() {
		isRunning = true;
		isEnding = false;
		step = 0;
		vPosition = olc::vf2d{};
		vScale = olc::vf2d{};
		vBoom.clear();
		fDeltaTime = 0.0f;
		fAspect = 0.0f;
	}
	void ComicScreen::startEnding() {
		isRunning = true;
		isEnding = true;
		step = 0;
		vPosition = olc::vf2d{};
		vScale = olc::vf2d{};
		vBoom.clear();
		fDeltaTime = 0.0f;
		fAspect = 0.0f;
	}

	void ComicScreen::OnAfterUserCreate() {
	}

	int ComicScreen::getProgress(float mult) {
		int f = fDeltaTime * 150.0f * mult;
		if (f > 255) f = 255;
		return f;
	}

	bool ComicScreen::OnBeforeUserUpdate(float& fElapsedTime) {
		if (!isRunning)
			return false;
		if (isEnding) {
			//if (step == 5) {
			//	isRunning = false;
			//	step = 0;
			//	return false;
			//}
			fDeltaTime += fElapsedTime;

			vPosition = vPosition.lerp(stepsED[std::clamp(step, 0, 4)], fElapsedTime * 10);
			pge->DrawDecal(vPosition, bgED->Decal(), olc::vf2d(1, 1));
			//pge->DrawDecal(0.f,0.f, bg.Decal())

			if (step == 5) {
				int p = getProgress(0.5f);
				pge->FillRectDecal(olc::vf2d(), olc::vf2d(960, 540), olc::Pixel(0, 0, 0, p));
				if (p == 255) {
					isRunning = false;
					step = 0;
					return false;
				}
				return true;
			}

			if (step == 4) {
				vPosition = stepsED[step];
				int p = 255 - getProgress();
				pge->FillRectDecal(olc::vf2d(), olc::vf2d(960, 540), olc::Pixel(0, 0, 0, p));
			}
			if (fDeltaTime > 3.f) {
				pge->DrawStringDecal(olc::vf2d(580, 520), "PRESS ENTER TO CONTINUE", olc::RED, olc::vf2d(2, 2));
			}

			if (pge->GetKey(olc::ENTER).bPressed) {
				step++;
				fDeltaTime = 0;
				//if (step == 5) {
				//	vPosition = steps[4];
				//}
			}
		}
		else {
			if (step == 8) {
				isRunning = false;
				step = 0;
				return false;
			}
			fDeltaTime += fElapsedTime;
			vPosition = vPosition.lerp(steps[step], fElapsedTime * 10);
			//pge->DrawDecal(0.f,0.f, bg.Decal())

			if (step == 5) {
				vPosition = olc::vf2d(5 * sin(fDeltaTime * 20) * (1 - (255.0f / getProgress(0.75f))), 5 * cos(fDeltaTime * 20) * (1 - (255.0f / getProgress(0.75f)))) + steps[step];
			}
			pge->DrawDecal(vPosition, bg->Decal(), olc::vf2d(1, 1));
			if (step == 0) {
				pge->FillRectDecal(olc::vf2d(), olc::vf2d(960, 540), olc::Pixel(0, 0, 0, 255 - getProgress()));
			}
			if (step == 2) {
				pge->FillRectDecal(olc::vf2d(), olc::vf2d(960, 540), olc::Pixel(0, 0, 0, getProgress()));
			}
			if (step == 3) {
				pge->FillRectDecal(olc::vf2d(), olc::vf2d(960, 540), olc::Pixel(0, 0, 0, 255 - getProgress()));
			}
			if (step == 7) {
				int p = getProgress(0.5f);
				pge->FillRectDecal(olc::vf2d(), olc::vf2d(960, 540), olc::Pixel(0, 0, 0, p));
				if (p == 255) {
					fDeltaTime = 0;
					isRunning = false;
					step = 0;
					return false;
				}
				return true;
			}


			if (fDeltaTime > 3.f) {
				pge->DrawStringDecal(olc::vf2d(580, 520), "PRESS ENTER TO CONTINUE", olc::RED, olc::vf2d(2, 2));
			}

			if (pge->GetKey(olc::ENTER).bPressed && step != 7) {
				step++;
				fDeltaTime = 0;
				if (step == 5 || step == 3) {
					vPosition = steps[step];
				}
			}
		}

		return true;
	}

}

#endif