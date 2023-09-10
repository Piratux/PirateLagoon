#include "ProjectileCodeComponent.h"
#include "GlobalTransformedView.h"
#include "GlobalPhysicsEngine.h"
#include "GlobalEntityManager.h"
#include "GlobalAssetManager.h"
#include "ParticleCodeComponent.h"
#include "ParticleDataComponent.h"
#include "PiraMath.h"
#include "Utils.h"

ProjectileCodeComponent::ProjectileCodeComponent(ProjectileDataComponent& data_component)
	: data_component(data_component) {
}

bool ProjectileCodeComponent::update(float delta) {
	if (data_component.destroy_on_next_frame) {
		return false;
	}

	data_component.lifespan_seconds -= delta;
	if (data_component.lifespan_seconds <= 0.0f) {
		create_water_splash();
		return false;
	}

	if (data_component.body == nullptr) {
		return false;
	}

	data_component.animation_group.update(delta);

	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	data_component.pos = physics_engine->get_position(data_component.body);

	// this is mainly needed for trailing projectile
	data_component.set_rotation_degrees(PiraMath::vec2_to_degrees(Utils::b2Vec2_to_vf2d(data_component.body->GetLinearVelocity())));

	return true;
}

void ProjectileCodeComponent::draw() {
	olc::TransformedView* transformed_view = GlobalTransformedView::get();

	const AnimationFrame& animation_frame = data_component.animation_group.get_frame();
	olc::Decal* decal = animation_frame.image->Decal();

	transformed_view->DrawPartialRotatedDecal(
		data_component.pos,
		decal,
		PiraMath::degrees_to_radians(data_component.get_rotation_degrees()),
		animation_frame.source_size / 2.0f,
		animation_frame.source_pos,
		animation_frame.source_size,
		data_component.texture_scale
	);
}

void ProjectileCodeComponent::create_water_splash() {
	EntityManager* entity_manager = GlobalEntityManager::get();
	AssetManager* asset_manager = GlobalAssetManager::get();

	Entity& entity = entity_manager->create_entity();
	AnimationGroup animation_group = asset_manager->get_animation_group(AnimationGroupName::PARTICLE_WATER_SPLASH);
	ParticleDataComponent& new_data_component = entity.add_data_component<ParticleDataComponent>(data_component.pos, animation_group);
	ParticleCodeComponent& new_code_component = entity.add_code_component<ParticleCodeComponent>(new_data_component);
	new_data_component.animation_group.set_looping(false);
	new_data_component.texture_scale = olc::vf2d{ 2.0f, 2.0f };
	new_data_component.particle_type = ParticleDataComponent::ParticleType::WATER_SPLASH;
}