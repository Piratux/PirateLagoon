#pragma once

#include "EnumAudioName.h"
#include "olcSoundWaveEngine.h"

class AudioEngine {
private:
	//olc::sound::WaveEngine wave_engine;

public:
	AudioEngine();
	void play(AudioName audio_name);
};