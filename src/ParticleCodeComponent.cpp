#include "ParticleCodeComponent.h"
#include "GlobalTransformedView.h"
#include "GlobalPhysicsEngine.h"
#include "GlobalPixelGameEngine.h"
#include "GlobalAssetManager.h"
#include "GlobalEntityManager.h"
#include "GlobalTileMap.h"
#include "PiraMath.h"

ParticleCodeComponent::ParticleCodeComponent(ParticleDataComponent& data_component)
	: data_component(data_component) {

}

bool ParticleCodeComponent::update(float delta) {
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	data_component.animation_group.update(delta);

	switch (data_component.particle_type) {
	case ParticleDataComponent::ParticleType::EXPLOSION:
		if (data_component.follow_pos != nullptr) {
			data_component.pos = *data_component.follow_pos;
		}
		break;

	case ParticleDataComponent::ParticleType::FLOATING_TEXT:
		data_component.lifespan_seconds -= delta;
		if (data_component.lifespan_seconds <= 0.0f) {
			return false;
		}

		data_component.pos.y -= delta * data_component.floating_text_up_speed;
		break;

	case ParticleDataComponent::ParticleType::TRAIL:
	{
		if (data_component.follow_body == nullptr) {
			return false;
		}

		data_component.pos = physics_engine->get_position(data_component.follow_body);


		// update speed data
		data_component.follow_body_speed = data_component.follow_body->GetLinearVelocity().Length();
		data_component.max_recorded_follow_body_speed = std::max(data_component.max_recorded_follow_body_speed, data_component.follow_body_speed);

		// calculate rotation
		//data_component.rotation = data_component.get_rotation_degrees()

		// calculate scale
		float dist_travelled = PiraMath::distance(data_component.start_pos, data_component.pos);

		// this assumes that texture_scale.x == texture_scale.y
		float half_trailed_object_width = data_component.follow_object_texture_width / 2 * data_component.texture_scale.x;
		float max_scaling_distance = data_component.max_trail_scale * data_component.animation_group.get_frame().source_size.y + half_trailed_object_width;

		float trail_scale;

		if (dist_travelled < max_scaling_distance) {
			// Scale trail appropriately as to not cross ship's sprite
			trail_scale = data_component.max_trail_scale * (dist_travelled / max_scaling_distance);
		}
		else {
			// Set trail scale depending on parent's speed
			trail_scale = data_component.max_trail_scale * (data_component.follow_body_speed / data_component.max_recorded_follow_body_speed);
		}

		data_component.texture_scale.x = trail_scale * data_component.cannon_ball_scale;
		data_component.texture_scale.y = trail_scale * data_component.cannon_ball_scale;

		break;
	}

	case ParticleDataComponent::ParticleType::WATER_SPLASH:
	{
		if (data_component.animation_group.animation_finished_once()) {
			return false;
		}

		break;
	}

	case ParticleDataComponent::ParticleType::FLYING_CREW_MEMBER:
	{
		data_component.lifespan_seconds -= delta;
		if (data_component.lifespan_seconds <= 0.0f) {
			TileMap* tile_map = GlobalTileMap::get();
			olc::vi2d tile_idx = olc::vi2d{ data_component.pos / olc::vf2d{tile_map->get_tile_size()} };
			if (tile_map->get_tile_idx(TileMap::Layer::Floor, tile_idx.x, tile_idx.y) == (int32_t)TileMap::TileIdx::Water) {
				// TODO: duplicated code
				EntityManager* entity_manager = GlobalEntityManager::get();
				AssetManager* asset_manager = GlobalAssetManager::get();

				Entity& entity = entity_manager->create_entity();
				AnimationGroup animation_group = asset_manager->get_animation_group(AnimationGroupName::PARTICLE_WATER_SPLASH);
				ParticleDataComponent& new_data_component = entity.add_data_component<ParticleDataComponent>(data_component.pos, animation_group);
				ParticleCodeComponent& new_code_component = entity.add_code_component<ParticleCodeComponent>(new_data_component);
				new_data_component.animation_group.set_looping(false);
				new_data_component.texture_scale = olc::vf2d{ 3.0f, 3.0f };
				new_data_component.particle_type = ParticleDataComponent::ParticleType::WATER_SPLASH;
			}

			return false;
		}

		data_component.pos = physics_engine->get_position(data_component.body);

		float x = 1.5f - data_component.lifespan_seconds;
		float scale = -(x * x) + 0.8f * x + 2.6f;
		data_component.texture_scale.x = scale;
		data_component.texture_scale.y = scale;

		//data_component.lifespan_seconds -= delta;
		//if (data_component.lifespan_seconds <= 0.0f) {
		//	return false;
		//}

		//data_component.pos.y -= delta * data_component.floating_text_up_speed;
		break;
	}


	default:
		break;
	}


	return true;
}

void ParticleCodeComponent::draw() {
	olc::TransformedView* transformed_view = GlobalTransformedView::get();
	olc::PixelGameEngine* pge = GlobalPixelGameEngine::get();
	PhysicsEngine* physics_engine = GlobalPhysicsEngine::get();

	switch (data_component.particle_type) {
	case ParticleDataComponent::ParticleType::FLOATING_TEXT:
	{
		transformed_view->DrawStringDecal(
			data_component.pos - data_component.text_scale * pge->GetTextSize(data_component.text) / 2 + olc::vf2d{data_component.floating_text_offset_x, 0},
			data_component.text,
			data_component.text_color,
			olc::vf2d{data_component.text_scale, data_component.text_scale}
		);
		break;
	}

	case ParticleDataComponent::ParticleType::TRAIL:
	{
		if (data_component.follow_body == nullptr) {
			return;
		}

		const AnimationFrame& animation_frame = data_component.animation_group.get_frame();
		olc::Decal* decal = animation_frame.image->Decal();

		transformed_view->DrawPartialRotatedDecal(
			data_component.pos,
			decal,
			physics_engine->get_angle_radians(data_component.follow_body),
			olc::vf2d{animation_frame.source_size.x / 2, (((float)-data_component.follow_object_texture_width * data_component.cannon_ball_scale) / 2.0f) / data_component.texture_scale.y}, // the division by texture scale is needed, because center gets divided by texture scale inside the function
			//animation_frame.source_size / 2.0f,
			animation_frame.source_pos,
			animation_frame.source_size,
			data_component.texture_scale
		);
		break;
	}

	default:
	{
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
		break;
	}

	}
}