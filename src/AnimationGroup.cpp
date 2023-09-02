#include "AnimationGroup.h"

void AnimationGroup::update(float delta) {
	check_animations_size();

	animations[current_animation].set_looping(looping);
	animations[current_animation].update(delta);
}

const AnimationFrame& AnimationGroup::get_frame() {
	check_animations_size();

	return animations[current_animation].get_frame();
}

void AnimationGroup::add_animation(AnimationName animation_name, Animation&& animation) {
	animations[animation_name] = std::move(animation);

	if (animations.size() == 1) {
		play_animation(animation_name);
	}
}

void AnimationGroup::play_animation(AnimationName animation_name) {
	if (animations.count(animation_name) == 0) {
		std::cout << "Error 'play_animation': Can't find animation" << std::endl;
		exit(-1);
	}
	current_animation = animation_name;
}

AnimationName AnimationGroup::get_current_animation_name() {
	check_animations_size();

	return current_animation;
}

void AnimationGroup::set_looping(bool value) {
	looping = value;
}

bool AnimationGroup::animation_finished_once() {
	return animations[current_animation].animation_finished_once();
}

void AnimationGroup::check_animations_size() {
	if (animations.size() == 0) {
		std::cout << "Error 'check_animations_size': AnimationGroup has no animations" << std::endl;
		exit(-1);
	}
}