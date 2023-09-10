#pragma once
#include "olcPixelGameEngine.h"
//#include "olcSoundWaveEngine.h"
#include "Animation.h"
#include "AnimationGroup.h"
#include "EnumAudioName.h"
#include "EnumImageName.h"
#include "EnumAnimationName.h"
#include "EnumAnimationGroupName.h"

class AssetManager {
private:
	//std::map<AudioName, olc::sound::Wave> audios;
	std::map<ImageName, olc::Renderable> images;

	// this is not ideal, but olc::Renderable doesn't like to be destroyed when vector re-allocated
	std::vector<std::unique_ptr<olc::Renderable>> animation_spritesheets;
	std::map<AnimationGroupName, AnimationGroup> animation_groups;

public:
	void init();

	//olc::sound::Wave* get_audio(AudioName audio_name);
	olc::Renderable* get_renderable(ImageName image_name);
	olc::Decal* get_decal(ImageName image_name);
	olc::Sprite* get_sprite(ImageName image_name);

	AnimationGroup get_animation_group(AnimationGroupName animation_group_name);

private:
	void init_empty_animation_frame();
	void load_audios();
	void load_images();
	void load_animation_groups();
};

