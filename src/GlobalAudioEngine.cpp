#include "GlobalAudioEngine.h"
#include "Utils.h"

AudioEngine* GlobalAudioEngine::audio_engine = nullptr;

void GlobalAudioEngine::set(AudioEngine* audio_engine) {
	Utils::check_null(audio_engine);
	GlobalAudioEngine::audio_engine = audio_engine;
}

AudioEngine* GlobalAudioEngine::get() {
	Utils::check_null(GlobalAudioEngine::audio_engine);
	return GlobalAudioEngine::audio_engine;
}