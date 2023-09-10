#include "ParticleDataComponent.h"
#include "PiraMath.h"
#include "GlobalPhysicsEngine.h"
#include "GlobalAssetManager.h"

ParticleDataComponent::ParticleDataComponent(olc::vf2d pos, AnimationGroup animation_group)
	: pos(pos)
	, animation_group(animation_group) {
}

ParticleDataComponent::~ParticleDataComponent() {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	if (body != nullptr) {
		physics_engine->destroy_body(body);
	}
}

void ParticleDataComponent::set_rotation_degrees(float degrees) {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	if (body == nullptr) {
		return;
	}

	olc::vf2d pos = physics_engine->get_position(body);
	physics_engine->set_transform(body, pos.x, pos.y, PiraMath::degrees_to_radians(degrees));
}

float ParticleDataComponent::get_rotation_degrees() {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	if (body == nullptr) {
		return 0;
	}
	return PiraMath::radians_to_degrees(physics_engine->get_angle_radians(body));
}