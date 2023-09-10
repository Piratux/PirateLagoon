#include "ProjectileDataComponent.h"
#include "PiraMath.h"
#include "GlobalPhysicsEngine.h"
#include "GlobalAssetManager.h"

ProjectileDataComponent::ProjectileDataComponent(olc::vf2d pos, AnimationGroup animation_group)
	: pos(pos)
	, animation_group(animation_group) {
}

ProjectileDataComponent::~ProjectileDataComponent() {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	if (body != nullptr) {
		physics_engine->destroy_body(body);
	}
}

void ProjectileDataComponent::set_rotation_degrees(float degrees) {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	if (body == nullptr) {
		std::cout << "Body is null in set_rotation_degrees()" << std::endl;
		exit(-1);
	}

	olc::vf2d pos = physics_engine->get_position(body);
	physics_engine->set_transform(body, pos.x, pos.y, PiraMath::degrees_to_radians(degrees));
}

float ProjectileDataComponent::get_rotation_degrees() {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	if (body == nullptr) {
		std::cout << "Body is null in get_rotation_degrees()" << std::endl;
		exit(-1);
	}
	return PiraMath::radians_to_degrees(physics_engine->get_angle_radians(body));
}