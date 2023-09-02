#pragma once
#include "ScreenFader.h"

class GlobalScreenFader {
public:
	static ScreenFader* screen_fader;

public:
	static void set(ScreenFader* screen_fader);
	static ScreenFader* get();
};
