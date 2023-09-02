#include "ParticleDataComponent.h"
#include "PiraMath.h"
#include "GlobalPhysicsEngine.h"
#include "GlobalAssetManager.h"

ParticleDataComponent::ParticleDataComponent(olc::vf2d pos, AnimationGroup animation_group)
	: pos(pos)
	, animation_group(animation_group) {

	//PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	//body = physics_engine->create_body(b2BodyType::b2_dynamicBody, pos.x, pos.y);

	//ColliderData data;
	//physics_engine->add_circle_collider(body, &data, 50);
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
		//std::cout << "Body is null in set_rotation_degrees()" << std::endl;
		//exit(-1);
	}

	olc::vf2d pos = physics_engine->get_position(body);
	physics_engine->set_transform(body, pos.x, pos.y, PiraMath::degrees_to_radians(degrees));
}

float ParticleDataComponent::get_rotation_degrees() {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	if (body == nullptr) {
		return 0;
		//std::cout << "Body is null in get_rotation_degrees()" << std::endl;
		//exit(-1);
	}
	return PiraMath::radians_to_degrees(physics_engine->get_angle_radians(body));
}