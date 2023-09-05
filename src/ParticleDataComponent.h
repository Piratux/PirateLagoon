#pragma once

#include "DataComponent.h"
#include "PhysicsEngine.h"
#include "AnimationGroup.h"
#include "Utils.h"

class ParticleDataComponent final : public DataComponent {
public:
	enum class ParticleType {
		NONE,
		EXPLOSION,
		WATER_SPLASH,
		FLOATING_TEXT,
		TRAIL,
		FLYING_CREW_MEMBER,
	};

	ParticleType particle_type = ParticleType::NONE;


	olc::vf2d pos = olc::vf2d{ 0.0f, 0.0f };

	b2Body* body = nullptr;
	olc::vf2d body_offset = olc::vf2d{ 0.0f, 0.0f };

	AnimationGroup animation_group;

	olc::vf2d pos_offset = olc::vf2d{ 0, 0 }; // additional applied offset after calculating final position
	float rotation = 0;

	olc::vf2d texture_scale = olc::vf2d{ 1, 1 };

	olc::vf2d* follow_pos = nullptr;
	float lifespan_seconds = 2.0f;

	// floating particle params
	float floating_text_offset_x = Utils::random_float(-20, 20);
	float floating_text_up_speed = 100.0f;
	float text_scale = 3.0f;
	std::string text;
	olc::Pixel text_color = olc::DARK_RED;

	// trailing particle params
	olc::vf2d start_pos = olc::vf2d{ 0.0f, 0.0f };
	b2Body* follow_body = nullptr;
	float max_trail_scale = 0.2f;
	float follow_body_speed = 0;
	float max_recorded_follow_body_speed = 0;
	int follow_object_texture_width = 0;
	float cannon_ball_scale = 1.0f;

public:
	ParticleDataComponent(olc::vf2d pos, AnimationGroup animation_group);
	~ParticleDataComponent() override;

	void set_rotation_degrees(float degrees);
	float get_rotation_degrees();
};