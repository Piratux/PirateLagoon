#pragma once

#include "DataComponent.h"
#include "PhysicsEngine.h"
#include "AnimationGroup.h"

class ProjectileDataComponent final : public DataComponent {
public:
	olc::vf2d pos = olc::vf2d{0.0f, 0.0f};

	b2Body* body = nullptr;
	olc::vf2d body_offset = olc::vf2d{0.0f, 0.0f};

	AnimationGroup animation_group;

	float lifespan_seconds = 2.0f;
	int damage = 0;
	bool destroy_on_next_frame = false;

	olc::vf2d texture_scale = olc::vf2d{1.0f, 1.0f};

public:
	ProjectileDataComponent(olc::vf2d pos, AnimationGroup animation_group);
	~ProjectileDataComponent() override;

	void set_rotation_degrees(float degrees);
	float get_rotation_degrees();
};