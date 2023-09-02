#pragma once

#include "DataComponent.h"
#include "PhysicsEngine.h"
#include "AnimationGroup.h"

class ExampleDataComponent final : public DataComponent {
public:
	olc::vf2d pos = olc::vf2d{0.0f, 0.0f};

	b2Body* body = nullptr;
	olc::vf2d body_offset = olc::vf2d{0.0f, 0.0f};

	AnimationGroup animation_group;

public:
	ExampleDataComponent(olc::vf2d pos, AnimationGroup animation_group);
	~ExampleDataComponent() override;

	void set_rotation_degrees(float degrees);
	float get_rotation_degrees();
};