#include "GlobalScreenFader.h"
#include "Utils.h"

ScreenFader* GlobalScreenFader::screen_fader = nullptr;

void GlobalScreenFader::set(ScreenFader* screen_fader) {
	Utils::check_null(screen_fader);
	GlobalScreenFader::screen_fader = screen_fader;
}

ScreenFader* GlobalScreenFader::get() {
	Utils::check_null(GlobalScreenFader::screen_fader);
	return GlobalScreenFader::screen_fader;
}
