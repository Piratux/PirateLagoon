#pragma once
#include "AudioEngine.h"

class GlobalAudioEngine {
public:
	static AudioEngine* audio_engine;

public:
	static void set(AudioEngine* audio_engine);
	static AudioEngine* get();
};