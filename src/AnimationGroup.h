#pragma once
#include <map>

#include "Animation.h"
#include "EnumAnimationName.h"

class AnimationGroup {
private:
	std::map<AnimationName, Animation> animations;
	AnimationName current_animation = AnimationName::DEFAULT;
	bool looping = true;

public:
	void update(float delta);

	const AnimationFrame& get_frame();

	void add_animation(AnimationName animation_name, Animation&& animation);
	void play_animation(AnimationName animation_name);
	AnimationName get_current_animation_name();

	// if false, animation returns empty frame when animation finishes
	void set_looping(bool value);
	bool animation_finished_once();

private:
	void check_animations_size();
};

