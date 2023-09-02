#include "AssetManager.h"

#include <filesystem>
#include <fstream>

void AssetManager::init() {
	std::cout << "Loading assets\n";

	load_images();
	init_empty_animation_frame();
	load_audios();
	load_animation_groups();

	std::cout << "Assets loaded succesfully\n";
}

olc::sound::Wave* AssetManager::get_audio(AudioName audio_name) {
	return &audios[audio_name];
}

olc::Renderable* AssetManager::get_renderable(ImageName image_name) {
	if (images.count(image_name) == 0) {
		std::cout << "Error: image_name " << (int)image_name << " not found" << std::endl;
		exit(-1);
	}
	return &images[image_name];
}

olc::Decal* AssetManager::get_decal(ImageName image_name) {
	return get_renderable(image_name)->Decal();
}

olc::Sprite* AssetManager::get_sprite(ImageName image_name) {
	return get_renderable(image_name)->Sprite();
}

AnimationGroup AssetManager::get_animation_group(AnimationGroupName animation_group_name) {
	if (animation_groups.count(animation_group_name) == 0) {
		std::cout << "Error: animation_group_name " << (int)animation_group_name << " not found" << std::endl;
		exit(-1);
	}
	return animation_groups[animation_group_name];
}

void AssetManager::init_empty_animation_frame() {
	AnimationFrame::empty_frame = AnimationFrame{get_renderable(ImageName::EMPTY), 0, 0, 1, 1, 0.0f};
}

void AssetManager::load_audios() {
	std::string audio_root = "./assets/audio/";

	auto load_audio = [&](AudioName audio, const std::string& name) {
		bool result = audios[audio].LoadAudioWaveform(audio_root + name + ".wav");
		if (result != true) {
			std::cout << "Error: couldn't load audio: " + name << "\n";
			exit(-1);
		}
	};

	load_audio(AudioName::CANNON_BLAST, "cannon_blast");
}

void AssetManager::load_images() {
	std::string images_root = "./assets/images/";

	auto load_image = [&](ImageName image, const std::string& name) {
		olc::rcode result = images[image].Load(images_root + name + ".png");
		if (result != olc::rcode::OK) {
			std::cout << "Error: couldn't load image: " + name << "\n";
			exit(-1);
		}
	};

	load_image(ImageName::EMPTY, "empty");
	load_image(ImageName::MENU_SCREEN, "menu_screen");
	load_image(ImageName::INTRO_SCREEN, "intro_screen");
	load_image(ImageName::OUTRO_SCREEN, "outro_screen");
	load_image(ImageName::TILESHEET, "tilesheet");
}

void AssetManager::load_animation_groups() {
	std::string animation_root = "./assets/animations/";

	auto load_animation_group = [&](
		AnimationGroupName animation_group_name,
		std::vector<std::tuple<AnimationName, std::string, olc::vi2d>> animation_params,
		int animation_speed = 15
		) {
		for (auto& animation_param : animation_params) {
			AnimationName animation_name;
			std::string animation_file_path;
			olc::vi2d grid_size;
			std::tie(animation_name, animation_file_path, grid_size) = animation_param;

			animation_spritesheets.push_back(std::make_unique<olc::Renderable>());
			olc::Renderable& spritesheet = *animation_spritesheets.back();
			olc::rcode result = spritesheet.Load(animation_root + animation_file_path + ".png");
			if (result != olc::rcode::OK) {
				std::cout << "Error: couldn't spritesheet image: " + animation_file_path << "\n";
				exit(-1);
			}

			Animation animation;
			animation.create(&spritesheet, spritesheet.Sprite()->Size(), grid_size, {}, animation_speed);

			animation_groups[animation_group_name].add_animation(animation_name, std::move(animation));
		}
	};

	auto load_image_as_animation_group = [&](AnimationGroupName animation_group_name, const std::string& path, olc::vi2d grid_size = {1, 1}, int animation_speed = 15) {
		load_animation_group(
			animation_group_name,
			{{AnimationName::DEFAULT, path, grid_size}, },
			animation_speed
		);
	};

	load_image_as_animation_group(AnimationGroupName::PARTICLE_CANNON_BALL_TRAIL, "trail");
	load_image_as_animation_group(AnimationGroupName::PROJECTILE_CANNON_BALL, "cannon_ball");
	load_image_as_animation_group(AnimationGroupName::PARTICLE_EXPLOSION, "animation_explosion", olc::vi2d{10, 1});
	load_image_as_animation_group(AnimationGroupName::PARTICLE_WATER_SPLASH, "water_splash3", olc::vi2d{8, 8}, 30);

	load_animation_group(
		AnimationGroupName::PARTICLE_FLYING_CREW_MEMBER,
		{{AnimationName::FLYING_CREW_MEMBER_1, "crew (1)", olc::vi2d{1, 1}},
		{AnimationName::FLYING_CREW_MEMBER_2, "crew (2)", olc::vi2d{1, 1}},
		{AnimationName::FLYING_CREW_MEMBER_3, "crew (3)", olc::vi2d{1, 1}},
		{AnimationName::FLYING_CREW_MEMBER_4, "crew (4)", olc::vi2d{1, 1}},
		{AnimationName::FLYING_CREW_MEMBER_5, "crew (5)", olc::vi2d{1, 1}},
		{AnimationName::FLYING_CREW_MEMBER_6, "crew (6)", olc::vi2d{1, 1}}, }
	);

	load_animation_group(
		AnimationGroupName::SHIP_WHITE,
		{{AnimationName::SHIP_HEALTHY, "ship_white (1)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_1, "ship_white (2)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_2, "ship_white (3)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DESTROYED, "ship_white (4)", olc::vi2d{1, 1}}, }
	);

	load_animation_group(
		AnimationGroupName::SHIP_YELLOW,
		{{AnimationName::SHIP_HEALTHY, "ship_yellow (1)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_1, "ship_yellow (2)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_2, "ship_yellow (3)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DESTROYED, "ship_yellow (4)", olc::vi2d{1, 1}}, }
	);

	load_animation_group(
		AnimationGroupName::SHIP_RED,
		{{AnimationName::SHIP_HEALTHY, "ship_red (1)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_1, "ship_red (2)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_2, "ship_red (3)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DESTROYED, "ship_red (4)", olc::vi2d{1, 1}}, }
	);

	load_animation_group(
		AnimationGroupName::SHIP_GREEN,
		{{AnimationName::SHIP_HEALTHY, "ship_green (1)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_1, "ship_green (2)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_2, "ship_green (3)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DESTROYED, "ship_green (4)", olc::vi2d{1, 1}}, }
	);

	load_animation_group(
		AnimationGroupName::SHIP_BLUE,
		{{AnimationName::SHIP_HEALTHY, "ship_blue (1)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_1, "ship_blue (2)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_2, "ship_blue (3)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DESTROYED, "ship_blue (4)", olc::vi2d{1, 1}}, }
	);

	load_animation_group(
		AnimationGroupName::SHIP_BLACK,
		{{AnimationName::SHIP_HEALTHY, "ship_black (1)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_1, "ship_black (2)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DAMAGED_2, "ship_black (3)", olc::vi2d{1, 1}},
		{AnimationName::SHIP_DESTROYED, "ship_black (4)", olc::vi2d{1, 1}}, }
	);
}